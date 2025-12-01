#include "ImageSegmentation.h"
#include <iostream>
#include <string>
#include <cstring>

// Função auxiliar para imprimir ajuda
void printUsage(const char* progName) {
    std::cout << "Uso: " << progName << " <caminho_imagem> [opcoes]\n"
              << "Opcoes:\n"
              << "  -t, --threshold <valor>  Define o limiar de corte final (Padrao: 200.0)\n"
              << "  -p, --pre <valor>        Define o limiar dos superpixels (Padrao: 40.0)\n"
              << "  --no-blur                Desativa a suavizacao inicial\n"
              << std::endl;
}

int main(int argc, char** argv) {
    // 1. Caminho da Imagem
    std::string inputImage = (argc > 1 && argv[1][0] != '-') ? argv[1] : "img/input.jpg";
    
    // 2. Configurações Padrão (AJUSTADAS PARA MELHOR QUALIDADE)
    
    // Threshold final: Define quão diferente os segmentos devem ser.
    // Valor 200.0 é um bom começo para fotos reais. 
    // Se a imagem ficar muito "inteira", diminua para 100 ou 150.
    double threshold = 20.0; 
    
    PreprocessingOptions opts;
    opts.enableBlur = true; // Essencial para juntar texturas (pelo, grama)
    
    // minSuperpixelSize (antigo preThreshold): 
    // Define o quão agressivo é o agrupamento inicial.
    // 40.0 ajuda a ignorar variações dentro do mesmo objeto.
    opts.minSuperpixelSize = 10.0; 

    // 3. Parsing de Argumentos
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--threshold") == 0 || std::strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) threshold = std::stod(argv[++i]);
        }
        else if (std::strcmp(argv[i], "--pre") == 0 || std::strcmp(argv[i], "-p") == 0) {
            // Permite ajustar o tamanho do superpixel via terminal
            if (i + 1 < argc) opts.minSuperpixelSize = std::stod(argv[++i]);
        }
        else if (std::strcmp(argv[i], "--no-blur") == 0) {
            opts.enableBlur = false;
        }
        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::cout << "=== CONFIGURACAO DE SEGMENTACAO ===" << std::endl;
    std::cout << "Imagem Entrada: " << inputImage << std::endl;
    std::cout << "Threshold Final (-t): " << threshold << std::endl;
    std::cout << "Threshold Superpixel (-p): " << opts.minSuperpixelSize << std::endl;
    std::cout << "Suavizacao (Blur): " << (opts.enableBlur ? "ATIVADO" : "DESATIVADO") << std::endl;
    std::cout << "===================================" << std::endl << std::endl;

    // Executa os 4 algoritmos em sequência
    
    std::cout << "--- 1. KRUSKAL (MST) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "img/out_kruskal.png", Strategy::KRUSKAL_MST, threshold, opts);

    std::cout << "\n--- 2. EDMONDS (MSA) ---" << std::endl;
    // Graças aos Superpixels, Edmonds agora deve rodar em tempo aceitável
    ImageSegmentation::runSegmentation(inputImage, "img/out_edmonds.png", Strategy::EDMONDS_MSA, threshold, opts);

    std::cout << "\n--- 3. TARJAN (MSA) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "img/out_tarjan.png", Strategy::TARJAN_MSA, threshold, opts);

    std::cout << "\n--- 4. GABOW (MSA) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "img/out_gabow.png", Strategy::GABOW_MSA, threshold, opts);

    std::cout << "\nConcluido! Verifique a pasta img/." << std::endl;
    
    return 0;
}