#ifndef KRUSKAL_MST_H
#define KRUSKAL_MST_H

#include <vector>
#include "WeightedGraph.h"

// Aresta simples usada na MST
struct KEdge {
    int u, v;
    double w;
    KEdge(int u, int v, double w) : u(u), v(v), w(w) {}
};

class KruskalMST {
private:
    int n;                          // número de vértices
    std::vector<KEdge> edges;       // arestas coletadas do grafo
    std::vector<KEdge> mst;         // resultado
    double totalWeight = 0.0;

public:
    KruskalMST(const WeightedGraph &g);
    const std::vector<KEdge>& compute();   // executa o algoritmo
    double getTotalWeight() const { return totalWeight; }
};

#endif
