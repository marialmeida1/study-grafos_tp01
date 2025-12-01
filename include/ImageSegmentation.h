#ifndef IMAGE_SEGMENTATION_H
#define IMAGE_SEGMENTATION_H

#include <string>

// Enum para selecionar qual algoritmo de grafo usar
enum class Strategy {
    KRUSKAL_MST,
    EDMONDS_MSA,
    TARJAN_MSA,
    GABOW_MSA
};

// Struct para opções de pré-processamento
// (Esta é a struct que estava causando o erro de assinatura)
struct PreprocessingOptions {
    bool enableBlur = true;       // Ativa suavização (box blur)
    double minSuperpixelSize = 15.0; // Limite para unir superpixels iniciais
};

class ImageSegmentation {
public:
    // A assinatura deve bater EXATAMENTE com a do .cpp
    static void runSegmentation(
        const std::string& inputPath, 
        const std::string& outputPath, 
        Strategy strategy, 
        double threshold,
        PreprocessingOptions options // O parâmetro crítico
    );
};

#endif // IMAGE_SEGMENTATION_H