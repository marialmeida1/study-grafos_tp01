#include "ImageSegmentation.h"
#include <iostream>

int main() {
    double threshold = 80.0; // Ajuste conforme a imagem (80.0 costuma ser bom para fotos naturais)

    std::cout << "--- Iniciando Segmentacao ---" << std::endl;

    // 1. Tarjan (MSA Direcionada)
    ImageSegmentation::runSegmentation("./imgs/input3.jpg", "./imgs/output_tarjan.png", 
                                       ImageSegmentation::MSA_DIRECTED, threshold);

    // 2. Gabow (MSA Direcionada - NOVO)
    ImageSegmentation::runSegmentation("./imgs/input3.jpg", "./imgs/output_gabow.png", 
                                       ImageSegmentation::GABOW_MSA, threshold);

    // 3. Edmonds (Simulando MST Não Direcionada)
    ImageSegmentation::runSegmentation("./imgs/input3.jpg", "./imgs/output_edmonds.png", 
                                       ImageSegmentation::MST_UNDIRECTED, threshold);
    
    return 0;
}