#include "ImageSegmentation.h"

// Includes dos algoritmos de grafos
#include "WeightedGraph.h"
#include "KruskalMST.h"
#include "EdmondsMST.h"
#include "TarjanMST.h"
#include "GabowMST.h"

// Definições das bibliotecas de imagem (STB)
// Se houver erro de "multiple definition", mova estes defines para um arquivo separado
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

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

// Union-Find (DSU) Local otimizado para a etapa de pré-processamento (Superpixels)
struct DSU_Pre {
    vector<int> pai;
    DSU_Pre(int n) {
        pai.resize(n);
        for(int i=0; i<n; ++i) pai[i] = i;
    }
    int find(int i) {
        if(pai[i] == i) return i;
        return pai[i] = find(pai[i]); // Path compression
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if(root_i != root_j) pai[root_i] = root_j;
    }
};

// --- Métodos de Ajuda (Cálculos de Cor e Imagem) ---

// Calcula diferença Euclidiana de cor entre dois pixels na imagem bruta
double getPixelDiff(const unsigned char* img, int idx1, int idx2, int channels) {
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;
    double sum = 0.0;
    for(int c=0; c<3; ++c) { // Apenas RGB
        double d = (double)img[p1+c] - (double)img[p2+c];
        sum += d*d;
    }
    return sqrt(sum);
}

// Calcula diferença entre duas cores médias (Tuplas RGB dos Superpixels)
double getColorDiff(const tuple<int,int,int>& c1, const tuple<int,int,int>& c2) {
    auto [r1, g1, b1] = c1;
    auto [r2, g2, b2] = c2;
    return sqrt(pow(r1-r2, 2) + pow(g1-g2, 2) + pow(b1-b2, 2));
}

// Suavização (Box Blur) para remover ruído de textura antes da segmentação
void suavizarImagem(unsigned char* img, int w, int h, int channels) {
    if(channels < 3) return; 
    vector<unsigned char> buffer(w * h * channels);
    
    // Kernel de vizinhança (Cruz)
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
    // Aplica o buffer de volta na imagem original
    for(size_t i=0; i<buffer.size(); ++i) img[i] = buffer[i];
}

// --- Implementação Principal da Segmentação ---

void ImageSegmentation::runSegmentation(const string& inputPath, const string& outputPath, Strategy strategy, double threshold) {
    // 1. Carregar Imagem
    int w, h, ch;
    unsigned char* img = stbi_load(inputPath.c_str(), &w, &h, &ch, 0);
    if (!img) { cerr << "Erro ao carregar: " << inputPath << endl; return; }

    int numPixels = w * h;
    cout << "[" << inputPath << "] Carregada: " << w << "x" << h << " pixels (" << numPixels << " nos)." << endl;

    // --- ETAPA A: Pré-processamento (Suavização) ---
    // Remove detalhes finos (como fios de cabelo) que geram arestas desnecessárias
    cout << "1. Aplicando suavizacao (Blur)..." << endl;
    suavizarImagem(img, w, h, ch);

    // --- ETAPA B: Geração de Superpixels (Redução Drástica do Grafo) ---
    cout << "2. Gerando Superpixels..." << endl;
    DSU_Pre dsu(numPixels);
    
    // Pixels vizinhos com diferença de cor < 15.0 são fundidos imediatamente
    double preThreshold = 15.0; 

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int u = y * w + x;
            // Conecta Direita
            if (x + 1 < w) {
                int v = y * w + (x + 1);
                if (getPixelDiff(img, u, v, ch) < preThreshold) dsu.unite(u, v);
            }
            // Conecta Baixo
            if (y + 1 < h) {
                int v = (y + 1) * w + x;
                if (getPixelDiff(img, u, v, ch) < preThreshold) dsu.unite(u, v);
            }
        }
    }

    // Mapeamento: Pixel ID -> Superpixel ID
    map<int, int> rootToSuperId;
    vector<int> pixelToSuper(numPixels);
    
    // Acumuladores para calcular a cor média de cada superpixel
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
        
        int idx = i * ch;
        sumR[sId] += img[idx]; 
        sumG[sId] += img[idx+1]; 
        sumB[sId] += img[idx+2];
        countP[sId]++;
    }

    int numSupernodes = nextSuperId;
    
    // Vetor com a cor média de cada superpixel (Nós do novo grafo)
    vector<tuple<int,int,int>> superColors(numSupernodes);
    for(int i=0; i<numSupernodes; ++i) {
        if(countP[i] > 0)
            superColors[i] = { sumR[i]/countP[i], sumG[i]/countP[i], sumB[i]/countP[i] };
    }

    cout << "   -> Grafo reduzido de " << numPixels << " para " << numSupernodes << " superpixels." << endl;

    // --- ETAPA C: Construção do Grafo Reduzido (Baseado nos Superpixels) ---
    cout << "3. Construindo Grafo de Superpixels..." << endl;
    auto start = chrono::high_resolution_clock::now();

    bool directed = (strategy != Strategy::KRUSKAL_MST);
    WeightedGraph graph(numSupernodes, directed);
    set<pair<int,int>> edgesAdded; // Evita múltiplas arestas entre os mesmos blocos

    // Offsets de vizinhança
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    int k_limite = directed ? 4 : 2; // Kruskal só precisa de 2 direções para cobrir grade

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int uPix = y * w + x;
            int uSuper = pixelToSuper[uPix];

            // Verifica vizinhos do pixel para conectar superpixels diferentes
            for(int k=0; k<k_limite; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];
                if(nx >= 0 && nx < w && ny >= 0 && ny < h) {
                    int vPix = ny * w + nx;
                    int vSuper = pixelToSuper[vPix];

                    if (uSuper != vSuper) {
                        // Se são superpixels diferentes, cria aresta no grafo reduzido
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
    // Agora é seguro rodar Edmonds, pois N é pequeno (~1000 nós em vez de 250k)
    cout << "4. Executando Algoritmo no Grafo Reduzido..." << endl;
    WeightedGraph resultGraph(numSupernodes, directed);
    int root = 0; // Raiz arbitrária

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
    cout << "   -> Tempo Algoritmo: " << elapsed.count() << "s" << endl;

    // --- ETAPA E: Segmentação Final (Corte por Threshold e Pintura) ---
    cout << "5. Gerando Imagem Final..." << endl;
    
    // Lista de adjacência do grafo resultante (após corte das arestas caras)
    vector<vector<int>> adj(numSupernodes);
    
    for(int i=0; i<numSupernodes; ++i) {
        WeightedGraph::AdjIterator it(resultGraph, i);
        WeightedEdge e = it.begin();
        while(e.v != -1) {
            // AQUI É A SEGMENTAÇÃO: Corta se peso > threshold
            if (e.weight <= threshold) {
                adj[e.v].push_back(e.w);
                adj[e.w].push_back(e.v);
            }
            if(it.end()) break;
            e = it.next();
        }
    }

    // BFS para identificar os componentes conectados e atribuir cores
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
            
            // Gera cor única para este novo segmento
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

    // Monta imagem de saída mapeando: Pixel -> Superpixel -> Segmento -> Cor
    vector<unsigned char> outData(w * h * ch);
    for(int i=0; i<numPixels; ++i) {
        int sId = pixelToSuper[i];
        int segId = superToSegment[sId];
        if(segId == -1) segId = 0; 

        Pixel p = segmentColors[segId];
        int idx = i * ch;
        outData[idx] = p.r;
        outData[idx+1] = p.g;
        outData[idx+2] = p.b;
    }

    cout << "   -> Total Segmentos Finais: " << segmentCount << endl;
    stbi_write_png(outputPath.c_str(), w, h, ch, outData.data(), w * ch);
    stbi_image_free(img);
}