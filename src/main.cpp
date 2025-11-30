#include <iostream>
#include "Graph.h"

// This preprocessor directive checks if WEIGHTED_GRAPH has been defined during compilation.
// If so, it includes the necessary header for the WeightedGraph class.
#ifdef WEIGHTED_GRAPH
#include "WeightedGraph.h"
#include "TarjanMST.h"
#endif

// --- Test function for Unweighted Graphs ---
void testUnweightedGraphs() {
    std::cout << "\n--- Testing Unweighted, Directed Graph ---" << std::endl;
    Graph g_dir(5, true);
    g_dir.insertEdge(0, 1);
    g_dir.insertEdge(0, 2);
    std::cout << "Initial Directed Graph:" << std::endl;
    g_dir.print();
    std::cout << "Edge 0->1? " << (g_dir.hasEdge(0, 1) ? "Yes" : "No") << std::endl;
    std::cout << "Edge 1->0? " << (g_dir.hasEdge(1, 0) ? "Yes" : "No") << std::endl;

    std::cout << "\n--- Testing Unweighted, Undirected Graph ---" << std::endl;
    Graph g_undir(5, false);
    g_undir.insertEdge(0, 1);
    g_undir.insertEdge(0, 2);
    std::cout << "Initial Undirected Graph:" << std::endl;
    g_undir.print();
    std::cout << "Edge 0-1? " << (g_undir.hasEdge(0, 1) ? "Yes" : "No") << std::endl;
    std::cout << "Edge 1-0? " << (g_undir.hasEdge(1, 0) ? "Yes" : "No") << std::endl;
}


// --- Test function for Weighted Graphs ---
// This entire function will only be included in the program if WEIGHTED_GRAPH is defined.
#ifdef WEIGHTED_GRAPH
void testWeightedGraphs() {
    std::cout << "\n--- Testing Weighted, Directed Graph ---" << std::endl;
    WeightedGraph wg_dir(5, true);
    wg_dir.insertEdge(0, 1, 2.5);
    wg_dir.insertEdge(1, 2, 3.1);
    std::cout << "Weight of 0->1: " << wg_dir.getWeight(0, 1) << std::endl;
    std::cout << "Weight of 1->0: " << wg_dir.getWeight(1, 0) << " (should be -1 as it doesn't exist)" << std::endl;

    std::cout << "\n--- Testing Weighted, Undirected Graph ---" << std::endl;
    WeightedGraph wg_undir(5, false);
    wg_undir.insertEdge(0, 1, 2.5);
    wg_undir.insertEdge(1, 2, 3.1);
    std::cout << "Weight of 0-1: " << wg_undir.getWeight(0, 1) << std::endl;
    std::cout << "Weight of 1-0: " << wg_undir.getWeight(1, 0) << std::endl;
}

void testTarjanMST() {
    std::cout << "\n--- Testing Tarjan MST Algorithm ---" << std::endl;
    
    // Teste 1: Grafo sem ciclos (caso simples)
    std::cout << "\n=== TESTE 1: Grafo sem ciclos ===" << std::endl;
    WeightedGraph graph1(4, true);
    
    // Adicionar arestas
    graph1.insertEdge(0, 1, 5.0);
    graph1.insertEdge(0, 2, 3.0);
    graph1.insertEdge(1, 2, 2.0);
    graph1.insertEdge(1, 3, 4.0);
    graph1.insertEdge(2, 3, 1.0);
    graph1.insertEdge(3, 0, 6.0);
    
    std::cout << "Grafo de teste criado com 4 vértices." << std::endl;
    
    // Executar algoritmo de Tarjan
    TarjanMST tarjan1(graph1);
    
    // Testar com raiz 0
    std::cout << "\nCalculando arborescência com raiz 0:" << std::endl;
    std::vector<TarjanEdge> mst1 = tarjan1.findMinimumSpanningArborescence(0);
    tarjan1.printArborescence(mst1);
    
    // Teste 2: Grafo com ciclo nas arestas mínimas
    std::cout << "\n=== TESTE 2: Grafo com ciclo ===" << std::endl;
    WeightedGraph graph2(4, true);
    
    // Criar um grafo onde as arestas mínimas formam um ciclo
    // Ciclo: 1 -> 2 -> 3 -> 1
    graph2.insertEdge(0, 1, 10.0);  // Raiz -> 1 (única entrada para 1)
    graph2.insertEdge(1, 2, 2.0);   // 1 -> 2 (menor entrada para 2)
    graph2.insertEdge(2, 3, 1.0);   // 2 -> 3 (menor entrada para 3)
    graph2.insertEdge(3, 1, 1.0);   // 3 -> 1 (menor entrada para 1, criando ciclo!)
    
    // Arestas alternativas (com pesos maiores)
    graph2.insertEdge(0, 2, 15.0);  // Alternativa para 2
    graph2.insertEdge(0, 3, 20.0);  // Alternativa para 3
    
    std::cout << "Grafo com ciclo criado:" << std::endl;
    std::cout << "- 0 -> 1 (peso 10.0)" << std::endl;
    std::cout << "- 1 -> 2 (peso 2.0)" << std::endl;
    std::cout << "- 2 -> 3 (peso 1.0)" << std::endl;
    std::cout << "- 3 -> 1 (peso 1.0) <- Forma ciclo!" << std::endl;
    std::cout << "- 0 -> 2 (peso 15.0)" << std::endl;
    std::cout << "- 0 -> 3 (peso 20.0)" << std::endl;
    
    TarjanMST tarjan2(graph2);
    
    std::cout << "\nCalculando arborescência com raiz 0:" << std::endl;
    std::vector<TarjanEdge> mst2 = tarjan2.findMinimumSpanningArborescence(0);
    tarjan2.printArborescence(mst2);
    
    // Teste 3: Grafo mais complexo com múltiplos ciclos
    std::cout << "\n=== TESTE 3: Grafo com múltiplos componentes ===" << std::endl;
    WeightedGraph graph3(6, true);
    
    // Primeiro componente (raiz 0)
    graph3.insertEdge(0, 1, 5.0);
    graph3.insertEdge(0, 2, 4.0);
    
    // Ciclo entre 3, 4, 5
    graph3.insertEdge(1, 3, 3.0);   // Entrada para 3
    graph3.insertEdge(2, 4, 2.0);   // Entrada para 4  
    graph3.insertEdge(3, 5, 1.0);   // 3 -> 5
    graph3.insertEdge(4, 5, 1.5);   // 4 -> 5 (maior, não será escolhida)
    graph3.insertEdge(5, 3, 1.0);   // 5 -> 3 (cria ciclo!)
    graph3.insertEdge(5, 4, 0.5);   // 5 -> 4 (menor entrada para 4, cria ciclo!)
    
    std::cout << "Grafo complexo com 6 vértices criado." << std::endl;
    
    TarjanMST tarjan3(graph3);
    
    std::cout << "\nCalculando arborescência com raiz 0:" << std::endl;
    std::vector<TarjanEdge> mst3 = tarjan3.findMinimumSpanningArborescence(0);
    tarjan3.printArborescence(mst3);
}
#endif


int main() {
    std::cout << "--- Running Graph Tests ---" << std::endl;
    testUnweightedGraphs();

    // The call to testWeightedGraphs() is also wrapped, so it only runs if compiled for it.
    #ifdef WEIGHTED_GRAPH
    testWeightedGraphs();
    testTarjanMST();
    #endif

    std::cout << "\n--- Tests Finished ---" << std::endl;
    return 0;
}