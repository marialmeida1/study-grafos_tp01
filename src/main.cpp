#include "ImageSegmentation.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // Permite passar a imagem via linha de comando ou usa o padrão
    std::string inputImage = (argc > 1) ? argv[1] : "input.jpg"; // Certifique-se de ter essa imagem
    double threshold = 50.0; // Ajuste este valor (30.0 a 80.0) para controlar a sensibilidade

    std::cout << "=== BENCHMARK DE SEGMENTACAO ===" << std::endl;
    std::cout << "Imagem Alvo: " << inputImage << std::endl;
    std::cout << "Threshold: " << threshold << std::endl << std::endl;

    // 1. Kruskal (Baseline Não Direcionado)
    std::cout << "--- [1] KRUSKAL (MST) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "out_kruskal.png", 
                                       Strategy::KRUSKAL_MST, threshold);
    std::cout << std::endl;

    // 2. Tarjan (MSA Otimizada)
    std::cout << "--- [2] TARJAN (MSA Otimizada) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "out_tarjan.png", 
                                       Strategy::TARJAN_MSA, threshold);
    std::cout << std::endl;

    // 3. Gabow (MSA Path Growing)
    std::cout << "--- [3] GABOW (MSA Path Growing) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "out_gabow.png", 
                                       Strategy::GABOW_MSA, threshold);
    std::cout << std::endl;

    // 4. Edmonds (MSA Recursiva - Pode ser mais lento em imagens grandes)
    std::cout << "--- [4] EDMONDS (MSA Recursiva) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "out_edmonds.png", 
                                       Strategy::EDMONDS_MSA, threshold);
    std::cout << std::endl;

    std::cout << "Benchmark concluido. Verifique os arquivos 'out_*.png'." << std::endl;

    return 0;
}