#ifndef TARJAN_MST_H
#define TARJAN_MST_H

#include "WeightedGraph.h"
#include <vector>
#include <limits>

struct TarjanEdge {
    int from, to;
    double weight;
    TarjanEdge(int f, int t, double w) : from(f), to(t), weight(w) {}
};

class TarjanMST {
private:
    std::vector<TarjanEdge> edges;
    int numVertices;
    
    // Estruturas auxiliares para o algoritmo
    std::vector<double> minCost;
    std::vector<int> parent;
    std::vector<int> id;
    std::vector<int> visit;
    std::vector<int> inCycle;
    
    // Métodos auxiliares
    void findMinIncomingEdges();
    bool hasCycle(int root);
    void contractCycle(int root);
    void expandSolution();
    
public:
    TarjanMST(const WeightedGraph& graph);
    std::vector<TarjanEdge> findMinimumSpanningArborescence(int root);
    double getTotalWeight() const;
    void printArborescence(const std::vector<TarjanEdge>& arborescence) const;
};

#endif