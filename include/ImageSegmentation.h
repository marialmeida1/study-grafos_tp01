#ifndef IMAGE_SEGMENTATION_H
#define IMAGE_SEGMENTATION_H

#include "WeightedGraph.h"
#include <string>
#include <vector>

class ImageSegmentation {
public:
    // Estratégias de segmentação
    enum Strategy {
        MST_UNDIRECTED, // Felzenszwalb-like (Grafo associado)
        MSA_DIRECTED    // Arborescência (Cousty et al.)
    };

    // Executa o pipeline completo
    static void runSegmentation(const std::string& inputPath, 
                                const std::string& outputPath, 
                                Strategy strategy,
                                double threshold);

private:
    // Calcula peso entre pixels (Dissimilaridade)
    static double getDissimilarity(const unsigned char* img, int w, int i, int j, int channels, bool directed);
    
    // Converte grafo de volta para imagem colorida por segmentos
    static void saveSegmentedImage(const std::string& path, int width, int height, 
                                   const std::vector<int>& parentMap);
};

#endif