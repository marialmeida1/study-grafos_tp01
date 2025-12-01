#include "ImageSegmentation.h"
#include <iostream>

using namespace std;

/**
 * Configurações otimizadas para diferentes tipos de imagem
 */
namespace TarjanPresets {
    
    // Para imagens naturais com muito ruído
    PreprocessingOptions naturalImages() {
        PreprocessingOptions opts;
        opts.enableGaussianBlur = true;
        opts.gaussianSigma = 1.2;
        opts.enableBilateralFilter = true;
        opts.bilateralSigmaColor = 60.0;
        opts.bilateralSigmaSpace = 60.0;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 1.8;
        opts.textureWeight = 0.4;
        return opts;
    }
    
    // Para imagens médicas ou científicas (alta precisão)
    PreprocessingOptions medicalImages() {
        PreprocessingOptions opts;
        opts.enableContrastNorm = true;
        opts.enableBilateralFilter = true;
        opts.bilateralSigmaColor = 40.0;
        opts.bilateralSigmaSpace = 40.0;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 3.0;
        opts.textureWeight = 0.6;
        return opts;
    }
    
    // Para imagens sintéticas ou com cores uniformes
    PreprocessingOptions syntheticImages() {
        PreprocessingOptions opts;
        opts.enableColorSpaceConv = true;
        opts.enableContrastNorm = true;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 2.5;
        opts.textureWeight = 0.2;
        return opts;
    }
    
    // Configuração balanceada para uso geral
    PreprocessingOptions balanced() {
        PreprocessingOptions opts;
        opts.enableGaussianBlur = true;
        opts.gaussianSigma = 1.0;
        opts.enableContrastNorm = true;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 2.0;
        opts.textureWeight = 0.3;
        return opts;
    }
    
    // Configuração rápida (menos processamento)
    PreprocessingOptions fast() {
        PreprocessingOptions opts;
        opts.enableContrastNorm = true;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 1.5;
        opts.textureWeight = 0.1;
        return opts;
    }
    
    // Configuração premium (máxima qualidade)
    PreprocessingOptions premium() {
        PreprocessingOptions opts;
        opts.enableGaussianBlur = true;
        opts.gaussianSigma = 0.8;
        opts.enableContrastNorm = true;
        opts.enableBilateralFilter = true;
        opts.bilateralSigmaColor = 75.0;
        opts.bilateralSigmaSpace = 75.0;
        opts.enableColorSpaceConv = true;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 2.2;
        opts.textureWeight = 0.35;
        return opts;
    }
}

int main() {
    cout << "=== BENCHMARK: Tarjan com Diferentes Presets de Pré-processamento ===" << endl;
    
    string inputImage = "img/input.jpg";
    double threshold = 50.0;
    
    cout << "Imagem: " << inputImage << endl;
    cout << "Threshold: " << threshold << endl << endl;
    
    // Teste todos os presets
    cout << "1. Processando com preset NATURAL..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_natural.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::naturalImages());
    cout << endl;
    
    cout << "2. Processando com preset MEDICAL..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_medical.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::medicalImages());
    cout << endl;
    
    cout << "3. Processando com preset SYNTHETIC..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_synthetic.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::syntheticImages());
    cout << endl;
    
    cout << "4. Processando com preset BALANCED..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_balanced.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::balanced());
    cout << endl;
    
    cout << "5. Processando com preset FAST..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_fast.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::fast());
    cout << endl;
    
    cout << "6. Processando com preset PREMIUM..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "out_tarjan_premium.png", 
        Strategy::TARJAN_MSA, threshold, TarjanPresets::premium());
    cout << endl;
    
    // Comparação com método original
    cout << "7. Processando com método ORIGINAL (sem pré-processamento)..." << endl;
    ImageSegmentation::runSegmentation(
        inputImage, "out_tarjan_original.png", 
        Strategy::TARJAN_MSA, threshold);
    cout << endl;
    
    cout << "=== BENCHMARK CONCLUÍDO ===" << endl;
    cout << "Resultados salvos:" << endl;
    cout << "  - out_tarjan_natural.png    (imagens naturais)" << endl;
    cout << "  - out_tarjan_medical.png    (imagens médicas)" << endl;
    cout << "  - out_tarjan_synthetic.png  (imagens sintéticas)" << endl;
    cout << "  - out_tarjan_balanced.png   (configuração balanceada)" << endl;
    cout << "  - out_tarjan_fast.png       (processamento rápido)" << endl;
    cout << "  - out_tarjan_premium.png    (máxima qualidade)" << endl;
    cout << "  - out_tarjan_original.png   (sem pré-processamento)" << endl;
    cout << endl;
    cout << "Recomendações:" << endl;
    cout << "  - Para fotos naturais: use 'natural' ou 'balanced'" << endl;
    cout << "  - Para imagens médicas: use 'medical'" << endl;
    cout << "  - Para gráficos/logos: use 'synthetic'" << endl;
    cout << "  - Para melhor qualidade: use 'premium'" << endl;
    cout << "  - Para velocidade: use 'fast'" << endl;
    
    return 0;
}
