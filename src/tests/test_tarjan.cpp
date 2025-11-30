#include <iostream>
#include <vector>
#include "WeightedGraph.h"
#include "TarjanMST.h"

WeightedGraph buildSampleGraph() {
    WeightedGraph graph(4, /*directed=*/true);

    graph.insertEdge(0, 1, 5.0);
    graph.insertEdge(0, 2, 3.0);
    graph.insertEdge(1, 2, 2.0);
    graph.insertEdge(1, 3, 4.0);
    graph.insertEdge(2, 3, 1.0);
    graph.insertEdge(3, 0, 6.0);

    return graph;
}

void runTarjanTestForRoot(const WeightedGraph& graph, int root) {
    std::cout << "\nCalculando arborescência com raiz " << root << ":\n";

    TarjanMST tarjan(graph);
    std::vector<TarjanEdge> mst = tarjan.findMinimumSpanningArborescence(root);

    tarjan.printArborescence(mst);
}

int main() {
    std::cout << "--- Testing Tarjan MST Algorithm ---\n";

    WeightedGraph graph = buildSampleGraph();
    std::cout << "Grafo de teste criado com 4 vértices.\n";

    runTarjanTestForRoot(graph, 0);
    runTarjanTestForRoot(graph, 1);

    std::cout << "\n--- Tarjan Tests Finished ---\n";
    return 0;
}
