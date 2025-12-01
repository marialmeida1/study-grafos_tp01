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

// Opções de pré-processamento
struct PreprocessingOptions {
    bool enableGaussianBlur = false;      // Suavização gaussiana
    double gaussianSigma = 1.0;           // Desvio padrão do filtro gaussiano
    bool enableContrastNorm = false;      // Normalização de contraste
    bool enableBilateralFilter = false;   // Filtro bilateral
    double bilateralSigmaColor = 50.0;    // Parâmetro de cor do filtro bilateral
    double bilateralSigmaSpace = 50.0;    // Parâmetro espacial do filtro bilateral
    bool enableEdgeWeighting = false;     // Peso baseado em gradientes de borda
    double edgeWeight = 2.0;              // Multiplicador para regiões de borda
    bool enableColorSpaceConv = false;    // Conversão para espaço LAB
    double textureWeight = 0.3;           // Peso da análise de textura local
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

    /**
     * Versão com pré-processamento melhorado para algoritmo de Tarjan
     */
    static void runSegmentationWithPreprocessing(const std::string& inputPath, 
                                                  const std::string& outputPath, 
                                                  Strategy strategy,
                                                  double threshold,
                                                  const PreprocessingOptions& options);

private:
    // Métodos auxiliares internos
    static double getDissimilarity(const unsigned char* img, int idx1, int idx2, int channels);
    static double getDissimilarityEnhanced(const unsigned char* img, const std::vector<double>& gradients,
                                           int idx1, int idx2, int channels, int width, int height,
                                           const PreprocessingOptions& options);
    static void saveSegmentedImage(const std::string& path, int width, int height, const std::vector<int>& componentMap);
    
    // Métodos de pré-processamento
    static std::vector<unsigned char> applyGaussianBlur(const unsigned char* img, int width, int height, 
                                                        int channels, double sigma);
    static std::vector<unsigned char> applyContrastNormalization(const unsigned char* img, int width, int height, int channels);
    static std::vector<unsigned char> applyBilateralFilter(const unsigned char* img, int width, int height, 
                                                          int channels, double sigmaColor, double sigmaSpace);
    static std::vector<double> computeGradients(const unsigned char* img, int width, int height, int channels);
    static std::vector<unsigned char> convertToLAB(const unsigned char* img, int width, int height);
    static double computeTextureVariance(const unsigned char* img, int x, int y, int width, int height, int channels);
    
    // Utilidades
    static double gaussian(double x, double sigma);
    static void rgbToLab(unsigned char r, unsigned char g, unsigned char b, double& l, double& a, double& bVal);
};

#endif