#ifndef GABOW_MST_H
#define GABOW_MST_H

#include <vector>

// Forward declaration para não forçar include de WeightedGraph aqui
class WeightedGraph;

// Estrutura de aresta: from -> to com peso weight
struct GabowEdge {
    int from;
    int to;
    double weight;

    GabowEdge() : from(-1), to(-1), weight(0.0) {}
    GabowEdge(int u, int v, double w) : from(u), to(v), weight(w) {}
};

/**
 * GabowMST
 * Implementação inspirada em Gabow et al. (GGST) para
 * Arborescência Geradora Mínima em grafos direcionados.
 *
 * Uso típico:
 *   GabowMST solver(grafo);
 *   auto arb = solver.findMinimumSpanningArborescence(root);
 *   double total = solver.getTotalWeight();
 */
class GabowMST {
public:
    explicit GabowMST(const WeightedGraph& g);

    // API semântica principal (mesmo estilo de Edmonds/Tarjan)
    std::vector<GabowEdge> findMinimumSpanningArborescence(int root) {
        return compute(root);
    }

    // Método principal de cálculo
    std::vector<GabowEdge> compute(int root);

    // Retorna o peso total obtido após compute
    double getTotalWeight() const { return totalWeight; }

    // Imprime a arborescência em formato amigável
    void printArborescence(const std::vector<GabowEdge>& arb) const;

private:
    int n {0};                             // número de vértices no grafo original
    std::vector<GabowEdge> edges;         // lista de arestas do grafo original
    std::vector<GabowEdge> arborescence;  // resultado após compute
    double totalWeight {0.0};
};

#endif // GABOW_MST_H
