#include <iostream>
#include "Graph.h"
#include "WeightedGraph.h"

// Grafo - Não-Direcionado & Não-Ponderado
/*int main() {
    Graph g(5, false); // Undirected graph with 5 vertices
    g.insertEdge(0, 1);
    g.insertEdge(0, 2);

    std::cout << "Number of vertices: " << g.V() << std::endl;
    std::cout << "Number of edges: " << g.E() << std::endl;
    std::cout << "Edge between 0 and 1? " << (g.hasEdge(0, 1) ? "Yes" : "No") << std::endl;
    std::cout << "Edge between 1 and 2? " << (g.hasEdge(1, 2) ? "Yes" : "No") << std::endl;

    return 0;
}*/


// Grafo - Direcionado & Ponderado
int main() {
    WeightedGraph gp(5); // 5 vértices dirigido e ponderados

    gp.insertEdge(0, 1, 2.5);
    gp.insertEdge(1, 2, 3.1);
    gp.insertEdge(2, 4, 1.8);


    std::cout << "Vertices: " << gp.V() << std::endl;
    std::cout << "Arestas: " << gp.E() << std::endl;

    gp.updateWeight(1, 2, 10.0);
    gp.removeEdge(2, 4);

    
    std::cout << "Arestas depois da remocao: " << gp.E() << std::endl;
    std::cout << "Existe 1->2? " << (gp.hasEdge(1, 2) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2->4? " << (gp.hasEdge(2, 4) ? "Sim" : "Nao") << std::endl;
    std::cout << "Peso de 1->2 " << (gp.getWeight(1,2)) << std::endl;

    return 0;
}
