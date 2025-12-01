#include "ImageSegmentation.h"

// Includes dos algoritmos
#include "WeightedGraph.h"
#include "KruskalMST.h"
#include "EdmondsMST.h"
#include "TarjanMST.h"
#include "GabowMST.h"

// Definições do STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <set>
#include <random>
#include <chrono>
#include <tuple>

using namespace std;

// --- Estruturas Auxiliares Locais ---

struct Pixel { unsigned char r, g, b; };

struct SimpleEdge { int u, v; double w; };

// DSU Local para a etapa de pré-processamento (Superpixels)
struct DSU_Pre {
    vector<int> pai;
    DSU_Pre(int n) {
        pai.resize(n);
        for(int i=0; i<n; ++i) pai[i] = i;
    }
    int find(int i) {
        if(pai[i] == i) return i;
        return pai[i] = find(pai[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if(root_i != root_j) pai[root_i] = root_j;
    }
};

// --- Métodos de Ajuda ---

// Calcula diferença de cor entre dois índices de pixel
double getPixelDiff(const unsigned char* img, int idx1, int idx2, int channels) {
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;
    double sum = 0.0;
    for(int c=0; c<3; ++c) {
        double d = (double)img[p1+c] - (double)img[p2+c];
        sum += d*d;
    }
    return sqrt(sum);
}

// Calcula diferença entre duas cores médias (Tuplas RGB)
double getColorDiff(const tuple<int,int,int>& c1, const tuple<int,int,int>& c2) {
    auto [r1, g1, b1] = c1;
    auto [r2, g2, b2] = c2;
    return sqrt(pow(r1-r2, 2) + pow(g1-g2, 2) + pow(b1-b2, 2));
}

// Suavização simples (Box Blur 3x3)
void suavizarImagem(unsigned char* img, int w, int h, int channels) {
    if(channels < 3) return; // Suporta apenas RGB/RGBA
    vector<unsigned char> buffer(w * h * channels);
    
    int dx[] = {0, 1, -1, 0, 0};
    int dy[] = {0, 0, 0, 1, -1};

    for(int y=0; y<h; ++y) {
        for(int x=0; x<w; ++x) {
            long long sum[3] = {0,0,0};
            int count = 0;
            
            for(int k=0; k<5; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];
                if(nx >= 0 && nx < w && ny >= 0 && ny < h) {
                    int idx = (ny * w + nx) * channels;
                    for(int c=0; c<3; ++c) sum[c] += img[idx+c];
                    count++;
                }
            }
            int currentIdx = (y * w + x) * channels;
            for(int c=0; c<3; ++c) buffer[currentIdx+c] = sum[c]/count;
        }
    }
    // Copia de volta
    for(size_t i=0; i<buffer.size(); ++i) img[i] = buffer[i];
}

// --- Implementação Principal Refatorada ---

void ImageSegmentation::runSegmentation(const string& inputPath, const string& outputPath, Strategy strategy, double threshold) {
    // 1. Carregar Imagem
    int w, h, ch;
    unsigned char* img = stbi_load(inputPath.c_str(), &w, &h, &ch, 0);
    if (!img) { cerr << "Erro ao carregar: " << inputPath << endl; return; }

    int numPixels = w * h;
    cout << "[" << inputPath << "] " << w << "x" << h << " pixels." << endl;

    // --- ETAPA A: Pré-processamento (Melhoria da Referência) ---
    cout << "1. Aplicando suavizacao..." << endl;
    suavizarImagem(img, w, h, ch);

    // --- ETAPA B: Geração de Superpixels (Melhoria da Referência) ---
    cout << "2. Gerando Superpixels..." << endl;
    DSU_Pre dsu(numPixels);
    double preThreshold = 15.0; // Pixels muito parecidos viram um só nó

    // Conecta vizinhos muito similares
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int u = y * w + x;
            // Tenta conectar com Direita e Baixo
            if (x + 1 < w) {
                int v = y * w + (x + 1);
                if (getPixelDiff(img, u, v, ch) < preThreshold) dsu.unite(u, v);
            }
            if (y + 1 < h) {
                int v = (y + 1) * w + x;
                if (getPixelDiff(img, u, v, ch) < preThreshold) dsu.unite(u, v);
            }
        }
    }

    // Identifica e Mapeia Superpixels
    map<int, int> rootToSuperId;
    vector<int> pixelToSuper(numPixels);
    vector<long long> sumR(numPixels, 0), sumG(numPixels, 0), sumB(numPixels, 0);
    vector<int> countP(numPixels, 0);
    int nextSuperId = 0;

    for (int i = 0; i < numPixels; ++i) {
        int root = dsu.find(i);
        if (rootToSuperId.find(root) == rootToSuperId.end()) {
            rootToSuperId[root] = nextSuperId++;
        }
        int sId = rootToSuperId[root];
        pixelToSuper[i] = sId;
        
        // Acumula cor
        int idx = i * ch;
        sumR[sId] += img[idx]; 
        sumG[sId] += img[idx+1]; 
        sumB[sId] += img[idx+2];
        countP[sId]++;
    }

    int numSupernodes = nextSuperId;
    vector<tuple<int,int,int>> superColors(numSupernodes);
    for(int i=0; i<numSupernodes; ++i) {
        if(countP[i] > 0)
            superColors[i] = { sumR[i]/countP[i], sumG[i]/countP[i], sumB[i]/countP[i] };
    }

    cout << "   -> Reduzido de " << numPixels << " pixels para " << numSupernodes << " superpixels." << endl;

    // --- ETAPA C: Construção do Grafo Reduzido ---
    cout << "3. Construindo Grafo Reduzido..." << endl;
    auto start = chrono::high_resolution_clock::now();

    bool directed = (strategy != Strategy::KRUSKAL_MST);
    WeightedGraph graph(numSupernodes, directed);
    set<pair<int,int>> edgesAdded; // Evita múltiplas arestas entre os mesmos superpixels

    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    int k_limite = directed ? 4 : 2; // Kruskal só precisa de 2 direções para cobrir grid

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int uPix = y * w + x;
            int uSuper = pixelToSuper[uPix];

            for(int k=0; k<k_limite; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];
                if(nx >= 0 && nx < w && ny >= 0 && ny < h) {
                    int vPix = ny * w + nx;
                    int vSuper = pixelToSuper[vPix];

                    if (uSuper != vSuper) {
                        // Se ainda não adicionamos aresta entre esses dois blocos
                        if (edgesAdded.find({uSuper, vSuper}) == edgesAdded.end()) {
                            double w = getColorDiff(superColors[uSuper], superColors[vSuper]);
                            graph.insertEdge(uSuper, vSuper, w);
                            edgesAdded.insert({uSuper, vSuper});
                        }
                    }
                }
            }
        }
    }

    // --- ETAPA D: Execução do Algoritmo (MST/MSA) ---
    cout << "4. Executando Algoritmo..." << endl;
    WeightedGraph resultGraph(numSupernodes, directed);
    int root = 0; 

    if (strategy == Strategy::KRUSKAL_MST) {
        resultGraph = KruskalMST::obterArvoreGeradoraMinima(graph);
    } 
    else if (strategy == Strategy::EDMONDS_MSA) {
        // Agora Edmonds vai rodar rápido, pois N é pequeno (~1000 nós)
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
    cout << "   -> Tempo Algoritmo: " << elapsed.count() << "s" << endl;

    // --- ETAPA E: Segmentação Final (Corte e Pintura) ---
    cout << "5. Gerando Imagem Final..." << endl;
    
    // Lista de adjacência do grafo resultante (após corte)
    vector<vector<int>> adj(numSupernodes);
    
    for(int i=0; i<numSupernodes; ++i) {
        WeightedGraph::AdjIterator it(resultGraph, i);
        WeightedEdge e = it.begin();
        while(e.v != -1) {
            if (e.weight <= threshold) {
                adj[e.v].push_back(e.w);
                adj[e.w].push_back(e.v);
            }
            if(it.end()) break;
            e = it.next();
        }
    }

    // BFS nos Supernodes
    vector<int> superToSegment(numSupernodes, -1);
    int segmentCount = 0;
    
    // Cores aleatórias para os segmentos
    vector<Pixel> segmentColors;
    mt19937 rng(42);
    uniform_int_distribution<int> uni(0, 255);

    for(int i=0; i<numSupernodes; ++i) {
        if(superToSegment[i] == -1) {
            queue<int> q;
            q.push(i);
            superToSegment[i] = segmentCount;
            
            // Gera cor para este novo segmento
            segmentColors.push_back({(unsigned char)uni(rng), (unsigned char)uni(rng), (unsigned char)uni(rng)});

            while(!q.empty()) {
                int u = q.front(); q.pop();
                for(int v : adj[u]) {
                    if(superToSegment[v] == -1) {
                        superToSegment[v] = segmentCount;
                        q.push(v);
                    }
                }
            }
            segmentCount++;
        }
    }

    // Monta imagem de saída mapeando Pixel -> Superpixel -> Segmento -> Cor
    vector<unsigned char> outData(w * h * ch);
    for(int i=0; i<numPixels; ++i) {
        int sId = pixelToSuper[i];
        int segId = superToSegment[sId];
        if(segId == -1) segId = 0; // fallback

        Pixel p = segmentColors[segId];
        int idx = i * ch;
        outData[idx] = p.r;
        outData[idx+1] = p.g;
        outData[idx+2] = p.b;
    }

    cout << "   -> Total Segmentos: " << segmentCount << endl;
    stbi_write_png(outputPath.c_str(), w, h, ch, outData.data(), w * ch);
    stbi_image_free(img);
}