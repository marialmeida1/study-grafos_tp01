#include "EdmondsMST.h"
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <iostream>

using namespace std;

static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity();

long long EdmondsMST::encode_edge_key(int from, int to) {
    return (static_cast<long long>(from) << 32) ^ (static_cast<unsigned long long>(to) & 0xffffffffULL);
}

std::vector<EdmondsMST::DirectedEdgeInternal> EdmondsMST::find_cheapest_incoming_edges(WeightedGraph& graph, int root) {
    int n = graph.V();
    std::vector<DirectedEdgeInternal> cheapest_edges(n);
    std::vector<double> min_costs(n, INFINITE_COST);

    for (int u = 0; u < n; ++u) {
        WeightedGraph::AdjIterator it(graph, u);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            int v = e.w;
            if (v != root && u != v) { 
                if (e.weight < min_costs[v]) {
                    min_costs[v] = e.weight;
                    cheapest_edges[v] = DirectedEdgeInternal(u, v, e.weight);
                }
            }
            if(it.end()) break;
            e = it.next();
        }
    }
    
    for(int v=0; v<n; ++v) {
        if (v != root && min_costs[v] == INFINITE_COST) {
             cheapest_edges[v].u = -1; 
        }
    }

    return cheapest_edges;
}

EdmondsMST::CycleDetectionResult EdmondsMST::detect_cycles(const std::vector<DirectedEdgeInternal>& cheapest_edges, 
                                                         int num_vertices, int root) {
    CycleDetectionResult result;
    result.cycle_id_of_vertex.assign(num_vertices, -1);
    std::vector<int> visit_tag(num_vertices, -1);
    int cycle_index = 0;

    for (int start = 0; start < num_vertices; ++start) {
        if (start == root || result.cycle_id_of_vertex[start] != -1) continue;
        if (cheapest_edges[start].u == -1) continue; 

        int current = start;
        while (current != root && current != -1 && 
               result.cycle_id_of_vertex[current] == -1 && 
               visit_tag[current] != start) {
            
            visit_tag[current] = start;
            current = cheapest_edges[current].u;
            if (current != -1 && cheapest_edges[current].u == -1 && current != root) {
                current = -1; 
            }
        }

        if (current != root && current != -1 && result.cycle_id_of_vertex[current] == -1) {
            std::vector<int> cycle;
            int node = current;
            do {
                cycle.push_back(node);
                result.cycle_id_of_vertex[node] = cycle_index;
                node = cheapest_edges[node].u;
            } while (node != current && node != -1);
            
            result.cycles.push_back(cycle);
            cycle_index++;
        }
    }
    return result;
}

EdmondsMST::InternalResult EdmondsMST::run_chu_liu(WeightedGraph& graph, int root_vertex) {
    int n = graph.V();
    InternalResult result(n);

    if (n == 0 || root_vertex < 0 || root_vertex >= n) {
        result.success = false;
        return result;
    }

    auto cheapest_edges = find_cheapest_incoming_edges(graph, root_vertex);

    for (int v = 0; v < n; ++v) {
        if (v == root_vertex) continue;
        if (cheapest_edges[v].u == -1) {
            result.success = false;
            return result;
        }
    }

    auto cycle_detection = detect_cycles(cheapest_edges, n, root_vertex);

    if (cycle_detection.cycles.empty()) {
        for (int v = 0; v < n; ++v) {
            if (v == root_vertex) continue;
            result.parent[v] = cheapest_edges[v].u;
            result.edge_costs[v] = cheapest_edges[v].cost;
        }
        return result;
    }

    int cycle_count = (int)cycle_detection.cycles.size();
    std::vector<int> component_id(n, -1);
    
    for (int v = 0; v < n; ++v) {
        if (cycle_detection.cycle_id_of_vertex[v] != -1) {
            component_id[v] = cycle_detection.cycle_id_of_vertex[v];
        }
    }

    int next_id = cycle_count;
    for (int v = 0; v < n; ++v) {
        if (component_id[v] == -1) {
            component_id[v] = next_id++;
        }
    }

    int contracted_vertices = next_id;
    int contracted_root = component_id[root_vertex];

    WeightedGraph contracted(contracted_vertices, true);
    std::unordered_map<long long, ContractedEdgeInfo> edge_mapping;

    for (int u = 0; u < n; ++u) {
        WeightedGraph::AdjIterator it(graph, u);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            int from_comp = component_id[u];
            int to_comp = component_id[e.w];

            if (from_comp != to_comp) {
                double adjusted_cost = e.weight;
                if (cycle_detection.cycle_id_of_vertex[e.w] != -1) {
                    adjusted_cost -= cheapest_edges[e.w].cost;
                }

                long long key = encode_edge_key(from_comp, to_comp);
                
                bool exists = contracted.hasEdge(from_comp, to_comp);
                double current_cost = exists ? contracted.getWeight(from_comp, to_comp) : INFINITE_COST;

                if (!exists || adjusted_cost < current_cost) {
                    if (exists) contracted.updateWeight(from_comp, to_comp, adjusted_cost);
                    else contracted.insertEdge(from_comp, to_comp, adjusted_cost);
                    
                    edge_mapping[key] = {from_comp, to_comp, adjusted_cost, u, e.w, e.weight};
                }
            }
            if(it.end()) break;
            e = it.next();
        }
    }

    auto contracted_result = run_chu_liu(contracted, contracted_root);

    if (!contracted_result.success) {
        result.success = false;
        return result;
    }

    for (int v = 0; v < n; ++v) {
        if (v == root_vertex) continue;
        result.parent[v] = cheapest_edges[v].u;
        result.edge_costs[v] = cheapest_edges[v].cost;
    }

    for (int comp = 0; comp < contracted_vertices; ++comp) {
        if (comp == contracted_root) continue;
        
        int parent_comp = contracted_result.parent[comp];
        if (parent_comp == -1) continue;

        long long key = encode_edge_key(parent_comp, comp);
        if (edge_mapping.find(key) != edge_mapping.end()) {
            auto info = edge_mapping[key];
            
            result.parent[info.original_target] = info.original_source;
            result.edge_costs[info.original_target] = info.original_cost;
        }
    }

    return result;
}

WeightedGraph EdmondsMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    auto internal_res = run_chu_liu(grafo, raiz);
    
    WeightedGraph mst(grafo.V(), true);
    if (!internal_res.success) {
        return mst;
    }

    for (int i = 0; i < grafo.V(); ++i) {
        if (i != raiz && internal_res.parent[i] != -1) {
            mst.insertEdge(internal_res.parent[i], i, internal_res.edge_costs[i]);
        }
    }
    return mst;
}