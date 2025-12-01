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
#include <algorithm>
#include <tuple>
#include <chrono>

using namespace std;

// --- Estruturas Auxiliares Locais ---

struct Pixel { unsigned char r, g, b; };

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

// --- Métodos de Ajuda com Melhoria Perceptual (Redmean) ---

// Fórmula Redmean: muito melhor que Euclideana simples para o olho humano
double getRedmeanDiff(long r1, long g1, long b1, long r2, long g2, long b2) {
    long rmean = (r1 + r2) / 2;
    long r = r1 - r2;
    long g = g1 - g2;
    long b = b1 - b2;
    return sqrt( (((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8) );
}

// Wrapper para acessar bytes brutos da imagem
double getPixelDiff(const unsigned char* img, int idx1, int idx2, int channels) {
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;
    return getRedmeanDiff(img[p1], img[p1+1], img[p1+2], 
                          img[p2], img[p2+1], img[p2+2]);
}

// Wrapper para tuplas (usado no grafo principal)
double getColorDiff(const tuple<int,int,int>& c1, const tuple<int,int,int>& c2) {
    auto [r1, g1, b1] = c1;
    auto [r2, g2, b2] = c2;
    return getRedmeanDiff(r1, g1, b1, r2, g2, b2);
}

// Suavização Gaussiana 5x5 (Melhor que a média simples)
void suavizarImagem(unsigned char* img, int w, int h, int channels) {
    if(channels < 3) return; 
    vector<unsigned char> buffer(w * h * channels);

    // Kernel Gaussiano 5x5 aproximado
    float kernel[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}
    };
    float kernelSum = 256.0f;

    // Ignora bordas para simplificar (crop de 2px não afeta o resultado visual)
    for(int y=2; y<h-2; ++y) {
        for(int x=2; x<w-2; ++x) {
            float sum[3] = {0,0,0};
            
            for(int ky=-2; ky<=2; ++ky) {
                for(int kx=-2; kx<=2; ++kx) {
                    int idx = ((y+ky) * w + (x+kx)) * channels;
                    float weight = kernel[ky+2][kx+2];
                    sum[0] += img[idx] * weight;
                    sum[1] += img[idx+1] * weight;
                    sum[2] += img[idx+2] * weight;
                }
            }
            
            int idx = (y * w + x) * channels;
            for(int c=0; c<3; ++c) 
                buffer[idx+c] = (unsigned char)(sum[c] / kernelSum);
        }
    }
    // Copia de volta, mantendo as bordas originais intactas onde não calculamos
    for(int y=2; y<h-2; ++y) {
        for(int x=2; x<w-2; ++x) {
            int idx = (y * w + x) * channels;
            for(int c=0; c<3; ++c) img[idx+c] = buffer[idx+c];
        }
    }
}

// --- Implementação Principal ---

void ImageSegmentation::runSegmentation(const string& inputPath, const string& outputPath, Strategy strategy, double threshold, PreprocessingOptions options) {
    // 1. Carregar Imagem
    int w, h, ch;
    unsigned char* img = stbi_load(inputPath.c_str(), &w, &h, &ch, 0);
    if (!img) { cerr << "Erro ao carregar: " << inputPath << endl; return; }

    int numPixels = w * h;
    cout << "[" << inputPath << "] " << w << "x" << h << " pixels." << endl;

    // --- ETAPA A: Pré-processamento ---
    if (options.enableBlur) {
        cout << "1. Aplicando Gaussian Blur..." << endl;
        suavizarImagem(img, w, h, ch);
    }

    // --- ETAPA B: Geração de Superpixels ---
    cout << "2. Gerando Superpixels..." << endl;
    DSU_Pre dsu(numPixels);
    
    // Usa o valor definido nas opções ou um padrão seguro
    double preThreshold = (options.minSuperpixelSize > 0) ? options.minSuperpixelSize : 40.0;

    // Conecta vizinhos muito similares
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int u = y * w + x;
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

    // Mapeamento Pixel -> Superpixel
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

    // --- ETAPA C: Construção do Grafo ---
    cout << "3. Construindo Grafo de Adjacencia..." << endl;
    auto start = chrono::high_resolution_clock::now();

    bool directed = (strategy != Strategy::KRUSKAL_MST);
    WeightedGraph graph(numSupernodes, directed);
    set<pair<int,int>> edgesAdded;

    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    int k_limite = directed ? 4 : 2; 

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

    // --- ETAPA D: Algoritmo MST/MSA ---
    cout << "4. Executando Algoritmo..." << endl;
    WeightedGraph resultGraph(numSupernodes, directed);
    int root = 0; 

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

    // --- ETAPA E: Segmentação e Pintura (Average Color) ---
    cout << "5. Gerando Imagem Final..." << endl;
    
    // Lista de adjacência baseada no threshold final
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

    // BFS para identificar componentes conexos (segmentos finais)
    vector<int> superToSegment(numSupernodes, -1);
    int segmentCount = 0;
    
    for(int i=0; i<numSupernodes; ++i) {
        if(superToSegment[i] == -1) {
            queue<int> q;
            q.push(i);
            superToSegment[i] = segmentCount;
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

    // --- MELHORIA VISUAL: Pintar com a cor média do segmento ---
    // Em vez de cor aleatória, calculamos a média real dos pixels daquele segmento
    vector<long long> segR(segmentCount, 0), segG(segmentCount, 0), segB(segmentCount, 0);
    vector<int> segPixelCount(segmentCount, 0);

    for(int i=0; i<numPixels; ++i) {
        int sId = pixelToSuper[i];
        int segId = superToSegment[sId];
        if(segId == -1) continue;

        int idx = i * ch;
        segR[segId] += img[idx];
        segG[segId] += img[idx+1];
        segB[segId] += img[idx+2];
        segPixelCount[segId]++;
    }

    vector<Pixel> finalColors(segmentCount);
    for(int i=0; i<segmentCount; ++i) {
        if(segPixelCount[i] > 0) {
            finalColors[i] = {
                (unsigned char)(segR[i] / segPixelCount[i]),
                (unsigned char)(segG[i] / segPixelCount[i]),
                (unsigned char)(segB[i] / segPixelCount[i])
            };
        } else {
            finalColors[i] = {0, 0, 0};
        }
    }

    // Gera o buffer de saída
    vector<unsigned char> outData(w * h * ch);
    for(int i=0; i<numPixels; ++i) {
        int sId = pixelToSuper[i];
        int segId = superToSegment[sId];
        if(segId == -1) segId = 0;

        Pixel p = finalColors[segId];
        int idx = i * ch;
        outData[idx] = p.r;
        outData[idx+1] = p.g;
        outData[idx+2] = p.b;
        if(ch == 4) outData[idx+3] = 255;
    }

    cout << "   -> Total Segmentos: " << segmentCount << endl;
    stbi_write_png(outputPath.c_str(), w, h, ch, outData.data(), w * ch);
    stbi_image_free(img);
}