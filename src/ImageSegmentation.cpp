#include "ImageSegmentation.h"
#include "EdmondsMST.h"
#include "TarjanMST.h"
// #include "GabowMST.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <cmath>
#include <map>
#include <random>
#include <array>
#include <chrono>

void ImageSegmentation::runSegmentation(const std::string& inputPath, 
                                        const std::string& outputPath, 
                                        Strategy strategy,
                                        double threshold) {
    auto Ttotal0 = std::chrono::steady_clock::now();

    int width, height, channels;
    unsigned char* img = stbi_load(inputPath.c_str(), &width, &height, &channels, 3); // Força RGB
    if (!img) {
        std::cerr << "Erro ao carregar imagem: " << inputPath << std::endl;
        return;
    }

    int numPixels = width * height;
    // Se for MSA, precisamos de um nó raiz virtual extra
    int rootNode = numPixels; 
    int numVertices = (strategy == MSA_DIRECTED)? numPixels + 1 : numPixels;
    bool isDirected = (strategy == MSA_DIRECTED);

    std::cout << "Construindo Grafo (" << width << "x" << height << ")..." << std::endl;
    auto Tbuild0 = std::chrono::steady_clock::now();

    WeightedGraph g(numVertices, isDirected);

    // Construção do Grid (4-vizinhança)
    for (int y = 0; y < height; ++y) {
        // progresso a cada 50 linhas
        if (y % 50 == 0) {
            std::cout << "  linha " << y << "/" << height << "\r" << std::flush;
        }

        for (int x = 0; x < width; ++x) {
            int u = y * width + x;

            // Vizinhos: Baixo e Direita são suficientes para não-direcionado (evita duplicatas)
            // Para direcionado, precisamos de todos os 4 vizinhos explicitamente se a lógica de peso for assimétrica
            static constexpr std::array<int,4> dx{ 1, 0, -1, 0 };
            static constexpr std::array<int,4> dy{ 0, 1, 0, -1 };
            
            // Otimização: Para grafo não direcionado, basta olhar direita e baixo
            int k_limit = isDirected? 4 : 2; 

            for (int k = 0; k < k_limit; ++k) {
                int nx = x + dx[k];
                int ny = y + dy[k];

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int v = ny * width + nx;
                    double w = getDissimilarity(img, width, u, v, 3, isDirected);
                    g.insertEdge(u, v, w);
                }
            }
            
            // Conecta Raiz Virtual -> Pixel (Apenas MSA)
            if (strategy == MSA_DIRECTED) {
                // Peso alto para desencorajar o uso da raiz, forçando agrupamento local
                g.insertEdge(rootNode, u, 10000.0); 
            }
        }
    }
    std::cout << std::string(30, ' ') << "\r" << std::flush; // limpa linha de progresso
    auto Tbuild1 = std::chrono::steady_clock::now();
    std::cout << "Grafo pronto em "
              << std::chrono::duration_cast<std::chrono::milliseconds>(Tbuild1 - Tbuild0).count()
              << " ms" << std::endl;

    std::cout << "\nExecutando Algoritmo de Arborescencia/MST..." << std::endl;
    auto Talgo0 = std::chrono::steady_clock::now();
    
    // Vetor para armazenar o resultado (floresta)
    // Usaremos um mapa de pai para Union-Find ou reconstrução
    std::vector<int> parent(numVertices);
    for(int i=0; i<numVertices; ++i) parent[i] = i;

    if (strategy == MSA_DIRECTED) {
        // Usa Tarjan ou Edmonds (escolha o que estiver mais estável)
        TarjanMST solver(g); 
        auto arborescence = solver.findMinimumSpanningArborescence(rootNode);
        
        for (const auto& edge : arborescence) {
            // CORTAR A ARBORESCÊNCIA (Segmentation Logic)
            // 1. Se a aresta vem da raiz virtual, é o início de um novo componente
            if (edge.from == rootNode) continue; 
            
            // 2. Se o peso for muito alto (borda forte), corta
            if (edge.weight > threshold) continue;

            parent[edge.to] = edge.from; // Constrói a árvore de componentes
        }
    } else {
        // Para MST não direcionada, idealmente Kruskal/Prim.
        EdmondsMST solver(g);
        // Escolhe um pixel arbitrário como raiz para cobrir o grafo conexo
        auto mst = solver.compute(0); 
        
        for (const auto& edge : mst) {
            if (edge.weight > threshold) continue;
            parent[edge.to] = edge.from;
        }
    }
    auto Talgo1 = std::chrono::steady_clock::now();
    std::cout << "\nAlgoritmo concluído em "
              << std::chrono::duration_cast<std::chrono::milliseconds>(Talgo1 - Talgo0).count()
              << " ms" << std::endl;

    // Pós-processamento: Union-Find para colorir componentes conectados
    // (Implementação simplificada de "flattening" dos pais)
    bool changed = true;
    while(changed) {
        changed = false;
        for(int i=0; i<numPixels; ++i) {
            if (parent[i]!= i && parent[parent[i]]!= parent[i]) {
                parent[i] = parent[parent[i]];
                changed = true;
            }
        }
    }

    std::cout << "\nSalvando resultado em " << outputPath << std::endl;
    auto Tout0 = std::chrono::steady_clock::now();
    saveSegmentedImage(outputPath, width, height, parent);
    auto Tout1 = std::chrono::steady_clock::now();
    std::cout << "Imagem salva em "
              << std::chrono::duration_cast<std::chrono::milliseconds>(Tout1 - Tout0).count()
              << " ms" << std::endl;

    stbi_image_free(img);

    auto Ttotal1 = std::chrono::steady_clock::now();
    std::cout << "Tempo total: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(Ttotal1 - Ttotal0).count()
              << " ms" << std::endl;
}

double ImageSegmentation::getDissimilarity(const unsigned char* img, int w, int u, int v, int channels, bool directed) {
    // Acesso aos pixels
    int idxU = u * channels;
    int idxV = v * channels;
    
    double dist = 0;
    // Distância Euclidiana RGB
    double r = img[idxU] - img[idxV];
    double g = img[idxU+1] - img[idxV+1];
    double b = img[idxU+2] - img[idxV+2];
    dist = std::sqrt(r*r + g*g + b*b);

    if (directed) {
        // Truque para MSA (Cousty et al): 
        // Favorecer arestas que descem o gradiente (regiões claras para escuras ou vice-versa)
        // Se U é muito diferente de V, o peso é alto.
        // Mas se quisermos "Waterhsed", podemos fazer peso direcional:
        // weight(u->v) = max(0, intensity(v) - intensity(u))
        // Isso cria bacias de atração nos mínimos locais.
        return dist; // Por enquanto, mantemos simétrico para teste básico
    }
    return dist;
}

void ImageSegmentation::saveSegmentedImage(const std::string& path, int width, int height, const std::vector<int>& labels) {
    std::vector<unsigned char> data(width * height * 3);
    std::map<int, std::vector<unsigned char>> colors;
    
    // Gerador de cores aleatórias para os segmentos
    std::mt19937 rng(42); 
    std::uniform_int_distribution<int> uni(0, 255);

    for (int i = 0; i < width * height; ++i) {
        int label = labels[i];
        
        if (colors.find(label) == colors.end()) {
            colors[label] = { (unsigned char)uni(rng), (unsigned char)uni(rng), (unsigned char)uni(rng) };
        }
        
        data[i*3]   = colors[label][0];
        data[i*3+1] = colors[label][1];
        data[i*3+2] = colors[label][2];
    }
    
    stbi_write_png(path.c_str(), width, height, 3, data.data(), width * 3);
}