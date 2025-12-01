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

// Calcula a distância Euclidiana de cor entre dois pixels (versão básica)
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

// Versão melhorada com análise de gradientes, textura e espaço de cores
double ImageSegmentation::getDissimilarityEnhanced(const unsigned char* img, const vector<double>& gradients,
                                                   int idx1, int idx2, int channels, int width, int height,
                                                   const PreprocessingOptions& options) {
    int p1 = idx1 * channels;
    int p2 = idx2 * channels;
    double colorDiff = 0.0;
    
    // 1. Diferença de cor básica
    for (int c = 0; c < min(3, channels); ++c) { 
        double diff = (double)img[p1 + c] - (double)img[p2 + c];
        colorDiff += diff * diff;
    }
    colorDiff = sqrt(colorDiff);
    
    // 2. Peso baseado em gradientes (bordas são mais importantes)
    double edgeWeight = 1.0;
    if (options.enableEdgeWeighting && !gradients.empty()) {
        double grad1 = gradients[idx1];
        double grad2 = gradients[idx2];
        double avgGrad = (grad1 + grad2) * 0.5;
        edgeWeight = 1.0 + options.edgeWeight * (avgGrad / 255.0);
    }
    
    // 3. Análise de textura local
    double textureWeight = 1.0;
    if (options.textureWeight > 0) {
        int x1 = idx1 % width, y1 = idx1 / width;
        int x2 = idx2 % width, y2 = idx2 / width;
        double var1 = computeTextureVariance(img, x1, y1, width, height, channels);
        double var2 = computeTextureVariance(img, x2, y2, width, height, channels);
        double textureDiff = abs(var1 - var2);
        textureWeight = 1.0 + options.textureWeight * textureDiff;
    }
    
    return colorDiff * edgeWeight * textureWeight;
}

// --- Métodos de Pré-processamento ---

vector<unsigned char> ImageSegmentation::applyGaussianBlur(const unsigned char* img, int width, int height, 
                                                           int channels, double sigma) {
    vector<unsigned char> result(width * height * channels);
    int radius = (int)(3 * sigma);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;
                double weightSum = 0.0;
                
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;
                        
                        if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                            double distance = sqrt(dx*dx + dy*dy);
                            double weight = gaussian(distance, sigma);
                            int idx = (ny * width + nx) * channels + c;
                            sum += img[idx] * weight;
                            weightSum += weight;
                        }
                    }
                }
                
                int resultIdx = (y * width + x) * channels + c;
                result[resultIdx] = (unsigned char)(sum / weightSum);
            }
        }
    }
    return result;
}

vector<unsigned char> ImageSegmentation::applyContrastNormalization(const unsigned char* img, int width, int height, int channels) {
    vector<unsigned char> result(width * height * channels);
    
    for (int c = 0; c < channels; ++c) {
        // Encontrar min e max para cada canal
        unsigned char minVal = 255, maxVal = 0;
        for (int i = 0; i < width * height; ++i) {
            unsigned char val = img[i * channels + c];
            minVal = min(minVal, val);
            maxVal = max(maxVal, val);
        }
        
        // Normalizar para usar toda a faixa [0, 255]
        double range = maxVal - minVal;
        if (range > 0) {
            for (int i = 0; i < width * height; ++i) {
                double normalized = 255.0 * (img[i * channels + c] - minVal) / range;
                result[i * channels + c] = (unsigned char)normalized;
            }
        } else {
            for (int i = 0; i < width * height; ++i) {
                result[i * channels + c] = img[i * channels + c];
            }
        }
    }
    return result;
}

vector<unsigned char> ImageSegmentation::applyBilateralFilter(const unsigned char* img, int width, int height, 
                                                             int channels, double sigmaColor, double sigmaSpace) {
    vector<unsigned char> result(width * height * channels);
    int radius = (int)(3 * sigmaSpace);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                double sum = 0.0;
                double weightSum = 0.0;
                unsigned char centerVal = img[(y * width + x) * channels + c];
                
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;
                        
                        if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
                            double spatialDist = sqrt(dx*dx + dy*dy);
                            unsigned char neighborVal = img[(ny * width + nx) * channels + c];
                            double colorDist = abs(centerVal - neighborVal);
                            
                            double spatialWeight = gaussian(spatialDist, sigmaSpace);
                            double colorWeight = gaussian(colorDist, sigmaColor);
                            double weight = spatialWeight * colorWeight;
                            
                            sum += neighborVal * weight;
                            weightSum += weight;
                        }
                    }
                }
                
                int resultIdx = (y * width + x) * channels + c;
                result[resultIdx] = (unsigned char)(sum / weightSum);
            }
        }
    }
    return result;
}

vector<double> ImageSegmentation::computeGradients(const unsigned char* img, int width, int height, int channels) {
    vector<double> gradients(width * height, 0.0);
    
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            double gradX = 0.0, gradY = 0.0;
            
            // Soma gradientes de todos os canais
            for (int c = 0; c < min(3, channels); ++c) {
                // Gradiente X (Sobel)
                double gx = img[(y * width + (x+1)) * channels + c] - img[(y * width + (x-1)) * channels + c];
                // Gradiente Y (Sobel)
                double gy = img[((y+1) * width + x) * channels + c] - img[((y-1) * width + x) * channels + c];
                
                gradX += gx;
                gradY += gy;
            }
            
            gradients[y * width + x] = sqrt(gradX * gradX + gradY * gradY);
        }
    }
    return gradients;
}

double ImageSegmentation::computeTextureVariance(const unsigned char* img, int x, int y, int width, int height, int channels) {
    int radius = 2;
    double sum = 0.0, sumSq = 0.0;
    int count = 0;
    
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                // Usa intensidade (média dos canais RGB)
                double intensity = 0.0;
                for (int c = 0; c < min(3, channels); ++c) {
                    intensity += img[(ny * width + nx) * channels + c];
                }
                intensity /= min(3, channels);
                
                sum += intensity;
                sumSq += intensity * intensity;
                count++;
            }
        }
    }
    
    if (count > 1) {
        double mean = sum / count;
        double variance = (sumSq / count) - (mean * mean);
        return max(0.0, variance);
    }
    return 0.0;
}

double ImageSegmentation::gaussian(double x, double sigma) {
    return exp(-(x * x) / (2 * sigma * sigma));
}

void ImageSegmentation::rgbToLab(unsigned char r, unsigned char g, unsigned char b, double& l, double& a, double& bVal) {
    // Conversão RGB -> LAB (simplificada)
    double rNorm = r / 255.0;
    double gNorm = g / 255.0;
    double bNorm = b / 255.0;
    
    // L representa a luminância
    l = 0.299 * rNorm + 0.587 * gNorm + 0.114 * bNorm;
    l *= 100.0;
    
    // A e B representam os componentes de cor (simplificado)
    a = (rNorm - gNorm) * 127.0;
    bVal = (gNorm - bNorm) * 127.0;
}

vector<unsigned char> ImageSegmentation::convertToLAB(const unsigned char* img, int width, int height) {
    vector<unsigned char> result(width * height * 3);
    
    for (int i = 0; i < width * height; ++i) {
        double l, a, b;
        rgbToLab(img[i*3], img[i*3+1], img[i*3+2], l, a, b);
        
        result[i*3] = (unsigned char)max(0.0, min(255.0, l * 2.55));
        result[i*3+1] = (unsigned char)max(0.0, min(255.0, a + 127.0));
        result[i*3+2] = (unsigned char)max(0.0, min(255.0, b + 127.0));
    }
    return result;
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

// --- Método Principal com Pré-processamento ---

void ImageSegmentation::runSegmentationWithPreprocessing(const string& inputPath, const string& outputPath, 
                                                          Strategy strategy, double threshold,
                                                          const PreprocessingOptions& options) {
    // 1. Carregar Imagem
    int width, height, channels;
    unsigned char* imgData = stbi_load(inputPath.c_str(), &width, &height, &channels, 0);

    if (!imgData) {
        cerr << "Erro ao carregar: " << inputPath << endl;
        return;
    }

    int numPixels = width * height;
    cout << "[" << inputPath << "] " << width << "x" << height << " pixels." << endl;
    cout << "Aplicando pré-processamento..." << endl;

    // 2. Aplicar Pré-processamento
    vector<unsigned char> processedImg(imgData, imgData + width * height * channels);
    
    if (options.enableGaussianBlur) {
        cout << "  - Aplicando suavização gaussiana (sigma=" << options.gaussianSigma << ")" << endl;
        processedImg = applyGaussianBlur(processedImg.data(), width, height, channels, options.gaussianSigma);
    }
    
    if (options.enableContrastNorm) {
        cout << "  - Aplicando normalização de contraste" << endl;
        processedImg = applyContrastNormalization(processedImg.data(), width, height, channels);
    }
    
    if (options.enableBilateralFilter) {
        cout << "  - Aplicando filtro bilateral" << endl;
        processedImg = applyBilateralFilter(processedImg.data(), width, height, channels, 
                                           options.bilateralSigmaColor, options.bilateralSigmaSpace);
    }
    
    if (options.enableColorSpaceConv && channels >= 3) {
        cout << "  - Convertendo para espaço LAB" << endl;
        processedImg = convertToLAB(processedImg.data(), width, height);
    }

    // 3. Calcular gradientes para análise de bordas
    vector<double> gradients;
    if (options.enableEdgeWeighting) {
        cout << "  - Computando gradientes de borda" << endl;
        gradients = computeGradients(processedImg.data(), width, height, channels);
    }

    // 4. Construir Grafo com função de dissimilaridade aprimorada
    bool directed = (strategy != Strategy::KRUSKAL_MST);
    WeightedGraph graph(numPixels, directed);

    // Offsets: Direita, Baixo, Esquerda, Cima
    int dx[] = {1, 0, -1, 0};
    int dy[] = {0, 1, 0, -1};
    
    // Para grafo não direcionado (Kruskal), olhamos apenas Direita e Baixo
    int k_limit = directed ? 4 : 2; 

    cout << "Construindo grafo com dissimilaridade aprimorada..." << endl;
    auto start = chrono::high_resolution_clock::now();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int u = y * width + x;

            for (int k = 0; k < k_limit; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int v = ny * width + nx;
                    
                    // Usar função de dissimilaridade aprimorada
                    double weight = getDissimilarityEnhanced(processedImg.data(), gradients, u, v, 
                                                            channels, width, height, options);
                    graph.insertEdge(u, v, weight);
                }
            }
        }
    }

    // 5. Executar Algoritmo (MST/MSA)
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

    // 6. Segmentação (Thresholding e Busca de Componentes)
    vector<vector<int>> adj(numPixels);
    int arestasMantidas = 0;

    for (int i = 0; i < numPixels; ++i) {
        WeightedGraph::AdjIterator it(resultGraph, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.weight <= threshold) {
                adj[e.v].push_back(e.w);
                adj[e.w].push_back(e.v);
                arestasMantidas++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    // 7. Identificar Componentes Conexos (Flood Fill / BFS)
    vector<int> componentMap(numPixels, -1);
    int componentCount = 0;

    for (int i = 0; i < numPixels; ++i) {
        if (componentMap[i] == -1) {
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