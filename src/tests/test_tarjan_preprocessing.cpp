#include <iostream>
#include "ImageSegmentation.h"

using namespace std;

int main() {
    cout << "=== TESTE: Algoritmo de Tarjan com Pré-processamento Melhorado ===" << endl;
    
    string inputImage = "img/input.jpg";
    double threshold = 50.0;
    
    cout << "Imagem: " << inputImage << endl;
    cout << "Threshold: " << threshold << endl << endl;
    
    // Configurações de pré-processamento otimizadas para Tarjan
    PreprocessingOptions options;
    
    // Teste 1: Apenas suavização gaussiana
    cout << "--- TESTE 1: Suavização Gaussiana ---" << endl;
    options = PreprocessingOptions();
    options.enableGaussianBlur = true;
    options.gaussianSigma = 1.5;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_gaussian.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Teste 2: Filtro bilateral (preserva bordas)
    cout << "--- TESTE 2: Filtro Bilateral ---" << endl;
    options = PreprocessingOptions();
    options.enableBilateralFilter = true;
    options.bilateralSigmaColor = 50.0;
    options.bilateralSigmaSpace = 50.0;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_bilateral.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Teste 3: Peso baseado em bordas
    cout << "--- TESTE 3: Peso por Gradientes de Borda ---" << endl;
    options = PreprocessingOptions();
    options.enableEdgeWeighting = true;
    options.edgeWeight = 2.5;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_edges.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Teste 4: Normalização de contraste + análise de textura
    cout << "--- TESTE 4: Contraste + Textura ---" << endl;
    options = PreprocessingOptions();
    options.enableContrastNorm = true;
    options.textureWeight = 0.5;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_contrast_texture.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Teste 5: Combinação completa (configuração premium)
    cout << "--- TESTE 5: Pré-processamento Completo ---" << endl;
    options = PreprocessingOptions();
    options.enableGaussianBlur = true;
    options.gaussianSigma = 1.0;
    options.enableContrastNorm = true;
    options.enableBilateralFilter = true;
    options.bilateralSigmaColor = 75.0;
    options.bilateralSigmaSpace = 75.0;
    options.enableEdgeWeighting = true;
    options.edgeWeight = 2.0;
    options.textureWeight = 0.3;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_premium.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Teste 6: Espaço de cores LAB
    cout << "--- TESTE 6: Espaço de Cores LAB ---" << endl;
    options = PreprocessingOptions();
    options.enableColorSpaceConv = true;
    options.enableContrastNorm = true;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_lab.png", 
        Strategy::TARJAN_MSA, threshold, options);
    cout << endl;
    
    // Para comparação: Tarjan sem pré-processamento
    cout << "--- COMPARAÇÃO: Tarjan Original ---" << endl;
    ImageSegmentation::runSegmentation(
        inputImage, "out_tarjan_original.png", 
        Strategy::TARJAN_MSA, threshold);
    cout << endl;
    
    cout << "=== Testes concluídos! ===" << endl;
    cout << "Arquivos gerados:" << endl;
    cout << "  - out_tarjan_gaussian.png (suavização)" << endl;
    cout << "  - out_tarjan_bilateral.png (preserva bordas)" << endl;
    cout << "  - out_tarjan_edges.png (peso por bordas)" << endl;
    cout << "  - out_tarjan_contrast_texture.png (contraste + textura)" << endl;
    cout << "  - out_tarjan_premium.png (configuração completa)" << endl;
    cout << "  - out_tarjan_lab.png (espaço LAB)" << endl;
    cout << "  - out_tarjan_original.png (sem pré-processamento)" << endl;
    
    return 0;
}
