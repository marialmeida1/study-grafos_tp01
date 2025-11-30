#ifndef GABOW_MST_H
#define GABOW_MST_H

#include <vector>

// Estrutura de aresta: from -> to com peso weight
struct GabowEdge {
    int from;
    int to;
    double weight;
    GabowEdge() : from(-1), to(-1), weight(0.0) {}
    GabowEdge(int u, int v, double w) : from(u), to(v), weight(w) {}
};

/*
 * Classe GabowMST
 * Interface simples:
 *  - construtor a partir de WeightedGraph
 *  - compute(root) -> retorna vetor de GabowEdge (a arborescência)
 *  - getTotalWeight() -> peso total (somado após compute)
 *  - printArborescence(...) -> exibe a solução
 *
 * Esta classe implementa a versão de contração/expansão inspirada
 * em Gabow et al. (GGST), cuidando de registrar as arestas escolhidas
 * em cada nível para permitir reconstrução correta.
 */
class GabowMST {
public:
    GabowMST(const class WeightedGraph &g);

    // calcula a arborescência com raiz 'root' (0..n-1)
    std::vector<GabowEdge> compute(int root);

    // retorna o peso total obtido após compute
    double getTotalWeight() const { return totalWeight; }

    // imprime a arborescência (padrão amigável)
    void printArborescence(const std::vector<GabowEdge> &arb) const;

private:
    int n; // número de vértices no grafo original
    std::vector<GabowEdge> edges; // lista de arestas do grafo original
    std::vector<GabowEdge> arborescence; // resultado após compute
    double totalWeight;
};

#endif // GABOW_MST_H
