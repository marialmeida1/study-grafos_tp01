#include "ImageSegmentation.h"
#include <iostream>
#include <string>
#include <cstring>

// Presets de pré-processamento
namespace TarjanPresets
{
    PreprocessingOptions balanced()
    {
        PreprocessingOptions opts;
        opts.enableGaussianBlur = true;
        opts.gaussianSigma = 1.0;
        opts.enableContrastNorm = true;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 2.0;
        opts.textureWeight = 0.3;
        return opts;
    }

    PreprocessingOptions premium()
    {
        PreprocessingOptions opts;
        opts.enableGaussianBlur = true;
        opts.gaussianSigma = 0.8;
        opts.enableContrastNorm = true;
        opts.enableBilateralFilter = true;
        opts.bilateralSigmaColor = 75.0;
        opts.bilateralSigmaSpace = 75.0;
        opts.enableEdgeWeighting = true;
        opts.edgeWeight = 2.2;
        opts.textureWeight = 0.35;
        return opts;
    }
}

int main(int argc, char **argv)
{
    // Permite passar a imagem via linha de comando ou usa o padrão
    std::string inputImage = (argc > 1) ? argv[1] : "img/input.jpg";
    double threshold = 50.0;
    bool usePreprocessing = false;

    // Verifica se foi solicitado pré-processamento
    for (int i = 2; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--preprocess") == 0 || std::strcmp(argv[i], "-p") == 0)
        {
            usePreprocessing = true;
        }
        else if (std::strcmp(argv[i], "--threshold") == 0 || std::strcmp(argv[i], "-t") == 0)
        {
            if (i + 1 < argc)
            {
                threshold = std::stod(argv[++i]);
            }
        }
    }

    std::cout << "=== BENCHMARK DE SEGMENTACAO ";
    if (usePreprocessing)
        std::cout << "COM PRE-PROCESSAMENTO ";
    std::cout << "===" << std::endl;
    std::cout << "Imagem Alvo: " << inputImage << std::endl;
    std::cout << "Threshold: " << threshold << std::endl;
    if (usePreprocessing)
    {
        std::cout << "Pré-processamento: ATIVADO (preset balanceado)" << std::endl;
    }
    std::cout << std::endl;

    // 1. Kruskal (Baseline Não Direcionado)
    std::cout << "--- [1] KRUSKAL (MST) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "/img/out_kruskal.png",
                                       Strategy::KRUSKAL_MST, threshold);
    std::cout << std::endl;

    // 2. Tarjan (MSA Otimizada)
    std::cout << "--- [2] TARJAN (MSA Otimizada) ---" << std::endl;
    if (usePreprocessing)
    {
        ImageSegmentation::runSegmentationWithPreprocessing(inputImage, "/img/out_tarjan.png",
                                                            Strategy::TARJAN_MSA, threshold,
                                                            TarjanPresets::balanced());
    }
    else
    {
        ImageSegmentation::runSegmentation(inputImage, "/img/out_tarjan.png",
                                           Strategy::TARJAN_MSA, threshold);
    }
    std::cout << std::endl;

    // 3. Gabow (MSA Path Growing)
    std::cout << "--- [3] GABOW (MSA Path Growing) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "/img/out_gabow.png",
                                       Strategy::GABOW_MSA, threshold);
    std::cout << std::endl;

    // 4. Edmonds (MSA Recursiva - Pode ser mais lento em imagens grandes)
    std::cout << "--- [4] EDMONDS (MSA Recursiva) ---" << std::endl;
    ImageSegmentation::runSegmentation(inputImage, "/img/out_edmonds.png",
                                       Strategy::EDMONDS_MSA, threshold);
    std::cout << std::endl;

    std::cout << "Benchmark concluido. Verifique os arquivos 'out_*.png'." << std::endl;

    if (usePreprocessing)
    {
        std::cout << std::endl
                  << "DICA: O arquivo 'out_tarjan.png' foi gerado com pré-processamento melhorado!" << std::endl;
        std::cout << "Para comparar, execute sem --preprocess para ver a diferença." << std::endl;
    }
    else
    {
        std::cout << std::endl
                  << "DICA: Execute com --preprocess para usar pré-processamento melhorado no Tarjan:" << std::endl;
        std::cout << "  ./bin/main img/input.jpg --preprocess" << std::endl;
    }

    return 0;
}