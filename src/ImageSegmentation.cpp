#include "ImageSegmentation.h"

#include "WeightedGraph.h"
#include "KruskalMST.h"
#include "EdmondsMST.h"
#include "TarjanMST.h"
#include "GabowMST.h"

// Definições do STB Image (Apenas neste arquivo)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <random>
#include <chrono>
#include <WeightedEdge.h>

using namespace std;

// Estrutura para cores na imagem de saída
struct Pixel { unsigned char r, g, b; };

// --- Métodos Auxiliares ---

// Calcula a distância Euclidiana de cor entre dois pixels
double ImageSegmentation::getDissimilarity(const unsigned char* img, int idx1, int idx2, int channels) {
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;
    double diffSum = 0.0;
    
    // Calcula diferença R, G, B
    for (int c = 0; c < 3; ++c) { 
        double diff = (double)img[p1 + c] - (double)img[p2 + c];
        diffSum += diff * diff;
    }
    return sqrt(diffSum);
}

void ImageSegmentation::saveSegmentedImage(const string& path, int width, int height, const vector<int>& componentMap) {
    int channels = 3;
    vector<unsigned char> outputData(width * height * channels);

    map<int, Pixel> colorMap;
    mt19937 rng(42); // Seed fixa para consistência
    uniform_int_distribution<int> uni(0, 255);

    for (int i = 0; i < width * height; ++i) {
        int compId = componentMap[i];

        // Gera cor aleatória para novo segmento
        if (colorMap.find(compId) == colorMap.end()) {
            colorMap[compId] = { 
                (unsigned char)uni(rng), 
                (unsigned char)uni(rng), 
                (unsigned char)uni(rng) 
            };
        }

        Pixel p = colorMap[compId];
        int idx = i * channels;
        outputData[idx]   = p.r;
        outputData[idx+1] = p.g;
        outputData[idx+2] = p.b;
    }

    if (stbi_write_png(path.c_str(), width, height, channels, outputData.data(), width * channels)) {
        cout << "Imagem salva: " << path << endl;
    } else {
        cerr << "Erro ao salvar imagem." << endl;
    }
}

// --- Método Principal ---

void ImageSegmentation::runSegmentation(const string& inputPath, const string& outputPath, Strategy strategy, double threshold) {
    // 1. Carregar Imagem
    int width, height, channels;
    unsigned char* imgData = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);

    if (!imgData) {
        cerr << "Erro ao carregar: " << inputPath << endl;
        return;
    }

    int numPixels = width * height;
    cout << "[" << inputPath << "] " << width << "x" << height << " pixels." << endl;

    // 2. Construir Grafo
    // Se for Kruskal, o grafo é não-direcionado. Caso contrário, direcionado.
    bool directed = (strategy != Strategy::KRUSKAL_MST);
    WeightedGraph graph(numPixels, directed);

    // Offsets: Direita, Baixo, Esquerda, Cima
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    
    // Otimização: Para grafo não direcionado (Kruskal), olhamos apenas Direita e Baixo
    // para evitar arestas duplicadas (u->v e v->u).
    int k_limit = directed ? 4 : 2; 

    cout << "Construindo grafo..." << endl;
    auto start = chrono::high_resolution_clock::now();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int u = y * width + x;

            for (int k = 0; k < k_limit; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int v = ny * width + nx;
                    double weight = getDissimilarity(imgData, u, v, channels);
                    graph.insertEdge(u, v, weight);
                }
            }
        }
    }

    // 3. Executar Algoritmo (MST/MSA)
    WeightedGraph resultGraph(numPixels, directed);
    int root = 0; // Raiz arbitrária (pixel 0,0) para algoritmos direcionados

    cout << "Executando algoritmo..." << endl;

    if (strategy == Strategy::KRUSKAL_MST) {
        resultGraph = KruskalMST::obterArvoreGeradoraMinima(graph);
    }
    else if (strategy == Strategy::EDMONDS_MSA) {
        resultGraph = EdmondsMST::obterArborescencia(graph, root);
    }
    else if (strategy == Strategy::TARJAN_MSA) {
        resultGraph = TarjanMST::obterArborescencia(graph, root);
    }
    else if (strategy == Strategy::GABOW_MSA) {
        resultGraph = GabowMST::obterArborescencia(graph, root);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Tempo processamento: " << elapsed.count() << "s" << endl;

    // 4. Segmentação (Thresholding e Busca de Componentes)
    // Convertemos o grafo resultante em uma lista de adjacência simples para BFS
    vector<vector<int>> adj(numPixels);
    int arestasMantidas = 0;

    for (int i = 0; i < numPixels; ++i) {
        WeightedGraph::AdjIterator it(resultGraph, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.weight <= threshold) {
                // Aresta mantida! Adiciona bidirecional para o BFS preencher a região
                adj[e.v].push_back(e.w);
                adj[e.w].push_back(e.v);
                arestasMantidas++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    // 5. Identificar Componentes Conexos (Flood Fill / BFS)
    vector<int> componentMap(numPixels, -1);
    int componentCount = 0;

    for (int i = 0; i < numPixels; ++i) {
        if (componentMap[i] == -1) {
            // Novo segmento encontrado
            queue<int> q;
            q.push(i);
            componentMap[i] = componentCount;

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (int v : adj[u]) {
                    if (componentMap[v] == -1) {
                        componentMap[v] = componentCount;
                        q.push(v);
                    }
                }
            }
            componentCount++;
        }
    }

    cout << "Segmentos gerados: " << componentCount << endl;
    saveSegmentedImage(outputPath, width, height, componentMap);

    stbi_image_free(imgData);
}