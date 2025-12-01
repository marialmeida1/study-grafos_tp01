#include <iostream>
#include "ImageSegmentation.h"

using namespace std;

int main() {
    cout << "=== COMPARAÇÃO: Tarjan Original vs Tarjan Melhorado ===" << endl;
    
    string inputImage = "img/input.jpg";
    double threshold = 45.0;
    
    cout << "Testando com imagem: " << inputImage << endl;
    cout << "Threshold: " << threshold << endl << endl;
    
    // Tarjan original (sem pré-processamento)
    cout << "🔹 EXECUTANDO TARJAN ORIGINAL..." << endl;
    auto start1 = chrono::high_resolution_clock::now();
    
    ImageSegmentation::runSegmentation(
        inputImage, "resultado_original.png", 
        Strategy::TARJAN_MSA, threshold);
    
    auto end1 = chrono::high_resolution_clock::now();
    auto tempo1 = chrono::duration<double>(end1 - start1).count();
    cout << endl;
    
    // Tarjan com pré-processamento otimizado
    cout << "🔸 EXECUTANDO TARJAN COM PRÉ-PROCESSAMENTO..." << endl;
    auto start2 = chrono::high_resolution_clock::now();
    
    // Configuração otimizada específica para Tarjan
    PreprocessingOptions opts;
    opts.enableGaussianBlur = true;
    opts.gaussianSigma = 1.0;           // Suavização leve
    opts.enableContrastNorm = true;     // Melhora separação de regiões
    opts.enableEdgeWeighting = true;    // Peso baseado em bordas
    opts.edgeWeight = 2.0;              // Multiplicador para bordas
    opts.textureWeight = 0.3;           // Considera textura local
    
    ImageSegmentation::runSegmentationWithPreprocessing(
        inputImage, "resultado_melhorado.png", 
        Strategy::TARJAN_MSA, threshold, opts);
    
    auto end2 = chrono::high_resolution_clock::now();
    auto tempo2 = chrono::duration<double>(end2 - start2).count();
    cout << endl;
    
    // Resumo dos resultados
    cout << "=== RESUMO DOS RESULTADOS ===" << endl;
    cout << "Tempo Tarjan Original:   " << tempo1 << "s" << endl;
    cout << "Tempo Tarjan Melhorado:  " << tempo2 << "s" << endl;
    cout << "Overhead do pré-proc.:   " << (tempo2 - tempo1) << "s" << endl;
    cout << endl;
    cout << "✅ Imagens geradas:" << endl;
    cout << "   📄 resultado_original.png   - Tarjan sem pré-processamento" << endl;
    cout << "   📄 resultado_melhorado.png  - Tarjan com pré-processamento" << endl;
    cout << endl;
    cout << "💡 DICA: Abra as duas imagens para comparar a qualidade!" << endl;
    cout << "   O pré-processamento deve resultar em:" << endl;
    cout << "   • Menos super-segmentação (regiões mais coerentes)" << endl;
    cout << "   • Bordas mais precisas entre objetos" << endl;
    cout << "   • Melhor separação de texturas diferentes" << endl;
    
    return 0;
}
