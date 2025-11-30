#ifndef EDMONDS_MST_H
#define EDMONDS_MST_H

#include "WeightedGraph.h"
#include <vector>
#include <limits>
#include <stdexcept>

/*
 * EdmondsMST.h
 * Implementação do algoritmo de Edmonds (Chu-Liu/Edmonds) para
 * Arborescência Geradora Mínima Direcionada.
 */

struct EdmondsEdge
{
    int from;
    int to;
    double weight;
    EdmondsEdge(int f = -1, int t = -1, double w = 0.0) : from(f), to(t), weight(w) {}
};

class EdmondsMST
{
private:
    int n;                          // número de vértices
    std::vector<EdmondsEdge> edges; // lista de arestas do grafo

    // Resultado
    std::vector<EdmondsEdge> arborescence;
    double totalWeight;

    // Constantes
    static constexpr double INF = std::numeric_limits<double>::infinity();

public:
    // Construtor: extrai arestas de WeightedGraph (usa interface observada no projeto)
    EdmondsMST(const WeightedGraph &g);

    // Computa a arborescência mínima com raiz `root`.
    // Retorna vetor de arestas (size n-1) e define totalWeight.
    std::vector<EdmondsEdge> compute(int root);

    // Acessores
    double getTotalWeight() const { return totalWeight; }

    // Imprime a arborescência no formato "u -> v (peso)"
    void printArborescence(const std::vector<EdmondsEdge> &arb) const;
};

#endif // EDMONDS_MST_H
