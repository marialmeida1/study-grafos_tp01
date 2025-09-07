#include <iostream>
#include "Graph.h"
#include "WeightedGraph.h"
using namespace std;

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

// Grafo - Direcionado & Não-Ponderado
int main()
{
    Graph g(5, true); // Grafo direcionado com 5 vértices

    g.insertEdge(0, 1);
    g.insertEdge(0, 2);
    g.insertEdge(1, 3);
    g.insertEdge(3, 4);

    cout << "Vértices: " << g.V() << endl;
    cout << "Arestas: " << g.E() << endl;

    cout << "Existe 0->1? " << (g.hasEdge(0, 1) ? "Sim" : "Nao") << endl;
    cout << "Existe 1->0? " << (g.hasEdge(1, 0) ? "Sim" : "Nao") << endl;

    g.print();

    g.insertVertex();
    cout << "Após inserir vértice, total: " << g.V() << endl;
    g.print();
    cout << "Existe 0->2? " << (g.hasEdge(0, 2) ? "Sim" : "Nao") << endl;

    g.removeEdge(0, 2);
    cout << "Após remover aresta 0->2, total de arestas: " << g.E() << endl;
    g.print();
    cout << "Existe 0->2? " << (g.hasEdge(0, 2) ? "Sim" : "Nao") << endl;

    g.removeVertex(3);
    cout << "Após remover vértice 3, total de vértices: " << g.V() << endl;
    g.print();

    return 0;
}

// Grafo - Direcionado & Ponderado
/*int main() {
    WeightedGraph gp(5, true); // 5 vértices dirigido e ponderados

    gp.insertEdge(0, 1, 2.5);
    gp.insertEdge(1, 2, 3.1);
    gp.insertEdge(2, 4, 1.8);


    std::cout << "Vertices: " << gp.V() << std::endl;
    std::cout << "Arestas: " << gp.E() << std::endl;

    gp.updateWeight(1, 2, 10.0);
    gp.removeEdge(2, 4);


    std::cout << "Arestas depois da remocao: " << gp.E() << std::endl;
    std::cout << "Existe 1->2? " << (gp.hasEdge(1, 2) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2->1? " << (gp.hasEdge(2, 1) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2->4? " << (gp.hasEdge(2, 4) ? "Sim" : "Nao") << std::endl;
    std::cout << "Peso de 1->2 " << (gp.getWeight(1,2)) << std::endl;

    return 0;
}*/

// Grafo - Não-Direcionado & Ponderado
/* int main() {
    WeightedGraph gp(5, false); // 5 vértices não direcionado e ponderados

    gp.insertEdge(0, 1, 2.5);
    gp.insertEdge(1, 2, 3.1);
    gp.insertEdge(2, 4, 1.8);

    std::cout << "Vertices: " << gp.V() << std::endl;
    std::cout << "Arestas: " << gp.E() << std::endl;

    gp.updateWeight(1, 2, 10.0);
    gp.removeEdge(2, 4);

    std::cout << "Arestas depois da remocao: " << gp.E() << std::endl;
    std::cout << "Existe 1-2? " << (gp.hasEdge(1, 2) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2-1? " << (gp.hasEdge(2, 1) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2-4? " << (gp.hasEdge(2, 4) ? "Sim" : "Nao") << std::endl;
    std::cout << "Peso de 1-2 " << (gp.getWeight(1,2)) << std::endl;

    return 0;
} */