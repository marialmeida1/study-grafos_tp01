#include "ImageSegmentation.h"
#include "EdmondsMST.h"
#include "TarjanMST.h"
#include "GabowMST.h"

// Definições necessárias para stb_image (apenas em um arquivo .cpp do projeto)
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

#include <iostream>
#include <cmath>
#include <queue>
#include <map>
#include <random>
#include <algorithm>
#include <chrono>

// Estrutura auxiliar para manipulação de cores
struct Pixel {
    unsigned char r, g, b;
};

// --- Implementação dos Métodos ---

double ImageSegmentation::getDissimilarity(const unsigned char* img, int w, int idx1, int idx2, int channels, bool directed) {
    // Índices no array de bytes da imagem
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;

    double diffSum = 0.0;
    // Distância Euclidiana no espaço de cores RGB
    for (int c = 0; c < 3; ++c) { // Considera R, G, B (ignora Alpha se houver)
        double diff = static_cast<double>(img[p1 + c]) - static_cast<double>(img[p2 + c]);
        diffSum += diff * diff;
    }

    return std::sqrt(diffSum);
}

void ImageSegmentation::saveSegmentedImage(const std::string& path, int width, int height, 
                                           const std::vector<int>& componentMap) {
    int channels = 3; // Saída sempre RGB
    std::vector<unsigned char> outputData(width * height * channels);

    // Mapa para armazenar cores aleatórias para cada componente (segmento)
    std::map<int, Pixel> colorMap;
    std::mt19937 rng(42); // Seed fixa para reprodutibilidade
    std::uniform_int_distribution<int> uni(0, 255);

    for (int i = 0; i < width * height; ++i) {
        int compId = componentMap[i];

        // Se é um novo segmento, gera uma cor nova
        if (colorMap.find(compId) == colorMap.end()) {
            colorMap[compId] = { 
                static_cast<unsigned char>(uni(rng)), 
                static_cast<unsigned char>(uni(rng)), 
                static_cast<unsigned char>(uni(rng)) 
            };
        }

        Pixel p = colorMap[compId];
        int pixelIdx = i * channels;
        outputData[pixelIdx] = p.r;
        outputData[pixelIdx + 1] = p.g;
        outputData[pixelIdx + 2] = p.b;
    }

    if (stbi_write_png(path.c_str(), width, height, channels, outputData.data(), width * channels)) {
        std::cout << "Imagem segmentada salva em: " << path << std::endl;
    } else {
        std::cerr << "Erro ao salvar a imagem." << std::endl;
    }
}

void ImageSegmentation::runSegmentation(const std::string& inputPath, 
                                        const std::string& outputPath, 
                                        Strategy strategy,
                                        double threshold) {
    // 1. Carregar Imagem
    int width, height, channels;
    unsigned char* imgData = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);

    if (!imgData) {
        std::cerr << "Erro ao carregar imagem: " << inputPath << std::endl;
        return;
    }

    int numPixels = width * height;
    std::cout << "Imagem carregada: " << width << "x" << height << " (" << numPixels << " pixels)" << std::endl;

    std::cout << "Construindo Grafo (" << width << "x" << height << ")..." << std::endl;
    auto Tbuild0 = std::chrono::steady_clock::now();

    // 2. Construir Grafo (WeightedGraph)
    // Cada pixel é um vértice. Conectamos pixels vizinhos (4-vizinhança).
    WeightedGraph graph(numPixels, true); // Grafo direcionado

    // Offsets para vizinhos: Direita, Baixo, Esquerda, Cima
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int u = y * width + x;

            for (int k = 0; k < 4; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];

                // Verifica limites da imagem
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int v = ny * width + nx;
                    double weight = getDissimilarity(imgData, width, u, v, channels, true);
                    
                    // Adiciona aresta u -> v
                    graph.insertEdge(u, v, weight);
                }
            }
        }
    }

    // 3. Executar Algoritmo de MSA (Minimum Spanning Arborescence)
    // Precisamos definir uma raiz. Em segmentação, geralmente usamos um nó virtual
    // ou escolhemos arbitrariamente o pixel (0,0) assumindo que o grafo é fortemente conexo (grid é).
    int root = 0; 
    
    // Lista de arestas resultantes da arborescência
    struct SimpleEdge { int u, v; double w; };
    std::vector<SimpleEdge> msaEdges;

    std::cout << "Executando algoritmo de Arborescência..." << std::endl;

    if (strategy == Strategy::MSA_DIRECTED) {
        TarjanMST tarjan(graph);
        auto result = tarjan.findMinimumSpanningArborescence(root);
        for(const auto& e : result) msaEdges.push_back({e.from, e.to, e.weight});
    } 
    else if (strategy == Strategy::MST_UNDIRECTED) {
        // Usando Edmonds como proxy para "MST direcionada clássica"
        EdmondsMST edmonds(graph);
        auto result = edmonds.compute(root);
        for(const auto& e : result) msaEdges.push_back({e.from, e.to, e.weight});
    }
    else if (strategy == Strategy::GABOW_MSA) {
        GabowMST gabow(graph);
        auto result = gabow.compute(root);
        for(const auto& e : result) msaEdges.push_back({e.from, e.to, e.weight});
    }

    std::cout << "Arborescência calculada. Total de arestas: " << msaEdges.size() << std::endl;

    // 4. Segmentação (Corte de Arestas por Threshold e Busca de Componentes)
    
    // Construir lista de adjacência APENAS com arestas que satisfazem o threshold.
    // Para colorir a região inteira, tratamos a conexão como não-direcionada na hora de buscar componentes.
    std::vector<std::vector<int>> adj(numPixels);
    
    for (const auto& edge : msaEdges) {
        if (edge.w <= threshold) {
            adj[edge.u].push_back(edge.v);
            adj[edge.v].push_back(edge.u); // Torna bidirecional para o Flood Fill alcançar o segmento todo
        }
    }

    // BFS/Flood Fill para identificar componentes desconexos
    std::vector<int> componentMap(numPixels, -1);
    int componentCount = 0;

    for (int i = 0; i < numPixels; ++i) {
        if (componentMap[i] == -1) {
            // Novo componente encontrado
            std::queue<int> q;
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

    std::cout << "Segmentação concluída. Número de segmentos: " << componentCount << std::endl;

    // 5. Salvar Resultado
    saveSegmentedImage(outputPath, width, height, componentMap);

    // Limpeza
    stbi_image_free(imgData);
}