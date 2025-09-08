#include <iostream>
#include "Graph.h"

// This preprocessor directive checks if WEIGHTED_GRAPH has been defined during compilation.
// If so, it includes the necessary header for the WeightedGraph class.
#ifdef WEIGHTED_GRAPH
#include "WeightedGraph.h"
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
#endif


int main() {
    std::cout << "--- Running Graph Tests ---" << std::endl;
    testUnweightedGraphs();

    // The call to testWeightedGraphs() is also wrapped, so it only runs if compiled for it.
    #ifdef WEIGHTED_GRAPH
    testWeightedGraphs();
    #endif

    std::cout << "\n--- Tests Finished ---" << std::endl;
    return 0;
}