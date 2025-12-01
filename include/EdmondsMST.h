#ifndef EDMONDS_MST_H
#define EDMONDS_MST_H

#include "WeightedGraph.h"
#include <vector>
#include <limits>

class EdmondsMST {
public:
    /**
     * Calcula a Arborescência Geradora Mínima usando a implementação
     * de Edmonds-Chu-Liu (adaptada para suportar reconstrução de grafos
     * de forma mais eficiente que a recursão pura).
     */
    static WeightedGraph obterArborescencia(WeightedGraph& grafo, int raiz);

private:
    // Estruturas internas auxiliares
    struct InternalResult {
        bool success;
        std::vector<int> parent;
        std::vector<double> edge_costs;
        InternalResult(int n = 0) : success(true), parent(n, -1), edge_costs(n, 0.0) {}
    };

    struct ContractedEdgeInfo {
        int from_component;
        int to_component;
        double adjusted_cost;
        int original_source;
        int original_target;
        double original_cost;
    };

    struct CycleDetectionResult {
        std::vector<int> cycle_id_of_vertex;
        std::vector<std::vector<int>> cycles;
    };

    // Estrutura simples para aresta interna
    struct DirectedEdgeInternal {
        int u, v;
        double cost;
        DirectedEdgeInternal(int _u=0, int _v=0, double _c=0.0) : u(_u), v(_v), cost(_c) {}
    };

    // Métodos auxiliares
    static std::vector<DirectedEdgeInternal> find_cheapest_incoming_edges(WeightedGraph& graph, int root);
    
    static CycleDetectionResult detect_cycles(const std::vector<DirectedEdgeInternal>& cheapest_edges, 
                                              int num_vertices, int root);
    
    static long long encode_edge_key(int from, int to);

    // O coração do algoritmo (recursivo mas estruturado)
    static InternalResult run_chu_liu(WeightedGraph& graph, int root_vertex);
};

#endif // EDMONDS_MST_H