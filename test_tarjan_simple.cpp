#include <iostream>
#include "ImageSegmentation.h"

using namespace std;

int main() {
    cout << "=== TESTE: Tarjan com Suavização Gaussiana ===" << endl;
    
    string inputImage = "img/input.jpg";
    double threshold = 45.0;
    
    cout << "Imagem: " << inputImage << endl;
    cout << "Threshold: " << threshold << endl << endl;
    
    // Configuração apenas com suavização gaussiana
    PreprocessingOptions opts;
    opts.enableGaussianBlur = true;
    opts.gaussianSigma = 1.2;
    
    cout << "Processando com suavização gaussiana..." << endl;
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "tarjan_gaussian_final.png", 
        Strategy::TARJAN_MSA, threshold, opts);
    
    cout << endl << "=== CONCLUÍDO ===" << endl;
    cout << "Imagem gerada: tarjan_gaussian_final.png" << endl;
    
    return 0;
}
