#include <iostream>
#include "Graph.h"

int main() {
    Graph g(5, false); // Undirected graph with 5 vertices
    g.insertEdge(0, 1);
    g.insertEdge(0, 2);

    std::cout << "Number of vertices: " << g.V() << std::endl;
    std::cout << "Number of edges: " << g.E() << std::endl;
    std::cout << "Edge between 0 and 1? " << (g.hasEdge(0, 1) ? "Yes" : "No") << std::endl;
    std::cout << "Edge between 1 and 2? " << (g.hasEdge(1, 2) ? "Yes" : "No") << std::endl;

    return 0;
}
