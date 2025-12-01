#include <iostream>
#include "ImageSegmentation.h"

using namespace std;

int main() {
    cout << "=== TESTE: Pré-processamento do Algoritmo de Tarjan ===" << endl;
    
    string inputImage = "img/input.jpg";
    double threshold = 45.0;
    
    cout << "Imagem: " << inputImage << endl;
    cout << "Threshold: " << threshold << endl << endl;
    
    // 1. Tarjan SEM pré-processamento (baseline)
    cout << "1. TARJAN SEM PRÉ-PROCESSAMENTO (Baseline)" << endl;
    cout << "-------------------------------------------" << endl;
    ImageSegmentation::runSegmentation(
        inputImage, "tarjan_original.png", 
        Strategy::TARJAN_MSA, threshold);
    cout << endl;
    
    // 2. Tarjan COM suavização gaussiana
    cout << "2. TARJAN COM SUAVIZAÇÃO GAUSSIANA" << endl;
    cout << "-----------------------------------" << endl;
    PreprocessingOptions opts1;
    opts1.enableGaussianBlur = true;
    opts1.gaussianSigma = 1.0;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_gaussian.png", 
        Strategy::TARJAN_MSA, threshold, opts1);
    cout << endl;
    
    // 3. Tarjan COM normalização de contraste
    cout << "3. TARJAN COM NORMALIZAÇÃO DE CONTRASTE" << endl;
    cout << "----------------------------------------" << endl;
    PreprocessingOptions opts2;
    opts2.enableContrastNorm = true;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_contrast.png", 
        Strategy::TARJAN_MSA, threshold, opts2);
    cout << endl;
    
    // 4. Tarjan COM peso baseado em bordas
    cout << "4. TARJAN COM PESO BASEADO EM BORDAS" << endl;
    cout << "------------------------------------" << endl;
    PreprocessingOptions opts3;
    opts3.enableEdgeWeighting = true;
    opts3.edgeWeight = 2.5;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_edges.png", 
        Strategy::TARJAN_MSA, threshold, opts3);
    cout << endl;
    
    // 5. Tarjan COM filtro bilateral
    cout << "5. TARJAN COM FILTRO BILATERAL" << endl;
    cout << "-------------------------------" << endl;
    PreprocessingOptions opts4;
    opts4.enableBilateralFilter = true;
    opts4.bilateralSigmaColor = 50.0;
    opts4.bilateralSigmaSpace = 50.0;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_bilateral.png", 
        Strategy::TARJAN_MSA, threshold, opts4);
    cout << endl;
    
    // 6. Tarjan COM configuração balanceada (múltiplas técnicas)
    cout << "6. TARJAN COM CONFIGURAÇÃO BALANCEADA" << endl;
    cout << "-------------------------------------" << endl;
    PreprocessingOptions opts5;
    opts5.enableGaussianBlur = true;
    opts5.gaussianSigma = 1.0;
    opts5.enableContrastNorm = true;
    opts5.enableEdgeWeighting = true;
    opts5.edgeWeight = 2.0;
    opts5.textureWeight = 0.3;
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_balanced.png", 
        Strategy::TARJAN_MSA, threshold, opts5);
    cout << endl;
    
    cout << "=== TESTE CONCLUÍDO ===" << endl;
    cout << "Resultados salvos:" << endl;
    cout << "  • tarjan_original.png   - Sem pré-processamento" << endl;
    cout << "  • tarjan_gaussian.png   - Com suavização" << endl;
    cout << "  • tarjan_contrast.png   - Com normalização" << endl;
    cout << "  • tarjan_edges.png      - Com peso por bordas" << endl;
    cout << "  • tarjan_bilateral.png  - Com filtro bilateral" << endl;
    cout << "  • tarjan_balanced.png   - Configuração balanceada" << endl;
    cout << endl;
    cout << "Compare as imagens para ver o efeito do pré-processamento!" << endl;
    
    return 0;
}
