#ifndef IMAGE_SEGMENTATION_H
#define IMAGE_SEGMENTATION_H

#include <string>
#include <vector>

// Enum para selecionar qual algoritmo de grafo usar
enum class Strategy {
    KRUSKAL_MST, // Não Direcionado (MST clássica)
    EDMONDS_MSA, // Direcionado (Recursivo)
    TARJAN_MSA,  // Direcionado (Otimizado)
    GABOW_MSA    // Direcionado (Path Growing)
};

class ImageSegmentation {
public:
    /**
     * Executa o pipeline de segmentação:
     * Carregar Imagem -> Criar Grafo -> Calcular MST/MSA -> Cortar Arestas -> Salvar
     */
    static void runSegmentation(const std::string& inputPath, 
                                const std::string& outputPath, 
                                Strategy strategy,
                                double threshold);

private:
    // Métodos auxiliares internos
    static double getDissimilarity(const unsigned char* img, int idx1, int idx2, int channels);
    static void saveSegmentedImage(const std::string& path, int width, int height, const std::vector<int>& componentMap);
};

#endif