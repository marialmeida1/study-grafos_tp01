#include <iostream>
#include "Graph.h"

#ifdef WEIGHTED_GRAPH
#include "WeightedGraph.h"
#endif

// -----------------------------------------------------------------------------
// Testes de grafos não ponderados
// -----------------------------------------------------------------------------
void testUnweightedGraphs() {
    std::cout << "\n--- Testing Unweighted, Directed Graph ---\n";

    Graph g_dir(5, /*directed=*/true);
    g_dir.insertEdge(0, 1);
    g_dir.insertEdge(0, 2);

    std::cout << "Initial Directed Graph:\n";
    g_dir.print();

    std::cout << "Edge 0->1? " << (g_dir.hasEdge(0, 1) ? "Yes" : "No") << '\n';
    std::cout << "Edge 1->0? " << (g_dir.hasEdge(1, 0) ? "Yes" : "No") << '\n';

    std::cout << "\n--- Testing Unweighted, Undirected Graph ---\n";

    Graph g_undir(5, /*directed=*/false);
    g_undir.insertEdge(0, 1);
    g_undir.insertEdge(0, 2);

    std::cout << "Initial Undirected Graph:\n";
    g_undir.print();

    std::cout << "Edge 0-1? " << (g_undir.hasEdge(0, 1) ? "Yes" : "No") << '\n';
    std::cout << "Edge 1-0? " << (g_undir.hasEdge(1, 0) ? "Yes" : "No") << '\n';
}

// -----------------------------------------------------------------------------
// Testes de grafos ponderados
// Só é compilado se WEIGHTED_GRAPH estiver definido
// -----------------------------------------------------------------------------
#ifdef WEIGHTED_GRAPH
void testWeightedGraphs() {
    std::cout << "\n--- Testing Weighted, Directed Graph ---\n";

    WeightedGraph wg_dir(5, /*directed=*/true);
    wg_dir.insertEdge(0, 1, 2.5);
    wg_dir.insertEdge(1, 2, 3.1);

    std::cout << "Weight of 0->1: " << wg_dir.getWeight(0, 1) << '\n';
    std::cout << "Weight of 1->0: " 
              << wg_dir.getWeight(1, 0)
              << " (should be -1 as it doesn't exist)\n";

    std::cout << "\n--- Testing Weighted, Undirected Graph ---\n";

    WeightedGraph wg_undir(5, /*directed=*/false);
    wg_undir.insertEdge(0, 1, 2.5);
    wg_undir.insertEdge(1, 2, 3.1);

    std::cout << "Weight of 0-1: " << wg_undir.getWeight(0, 1) << '\n';
    std::cout << "Weight of 1-0: " << wg_undir.getWeight(1, 0) << '\n';
}
#endif

int main() {
    std::cout << "--- Running Graph Tests ---\n";

    testUnweightedGraphs();

#ifdef WEIGHTED_GRAPH
    testWeightedGraphs();
#endif

    std::cout << "\n--- Tests Finished ---\n";
    return 0;
}
