#include "ImageSegmentation.h"

int main() {
    // Exemplo de uso
    // Threshold alto (ex: 30.0) funde regiões parecidas. Baixo (ex: 10.0) detalha mais.
    
    // Teste MSA (Direcionado - Tarjan)
    ImageSegmentation::runSegmentation("./imgs/input.jpg", "./imgs/output_tarjan_msa.png", 
                                       ImageSegmentation::MSA_DIRECTED, 25.0);
                                       
    // Teste MST (Não Direcionado - Simulado no Edmonds)
    ImageSegmentation::runSegmentation("./imgs/input.jpg", "./imgs/output_edmonds_mst.png", 
                                       ImageSegmentation::MST_UNDIRECTED, 25.0);
    
    return 0;
}