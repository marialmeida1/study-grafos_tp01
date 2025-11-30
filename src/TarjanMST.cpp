#include "TarjanMST.h"
#include <iostream>
#include <algorithm>
#include <stack>

const double INF = std::numeric_limits<double>::infinity();

TarjanMST::TarjanMST(const WeightedGraph& graph) : numVertices(graph.V()) {
    
    // Extrair todas as arestas do grafo
/*     for (int v = 0; v < numVertices; v++) {
        for (int w = 0; w < numVertices; w++) {
            if (graph.hasEdge(v, w)) {
                double weight = graph.getWeight(v, w);
                edges.push_back(TarjanEdge(v, w, weight));
            }
        }
    }
    
    // Inicializar estruturas auxiliares
    minCost.resize(numVertices, INF);
    parent.resize(numVertices, -1);
    id.resize(numVertices, -1);
    visit.resize(numVertices, -1);
    inCycle.resize(numVertices, -1); */

    // Inicializar estruturas auxiliares
    minCost.resize(numVertices, std::numeric_limits<double>::infinity());
    parent.resize(numVertices, -1);
    id.resize(numVertices, -1);
    visit.resize(numVertices, -1);
    inCycle.resize(numVertices, -1);

    // CORREÇÃO DE PERFORMANCE
    for (int u = 0; u < numVertices; ++u) {
        WeightedGraph::AdjIterator it(graph, u);
        for (auto edge = it.begin();!it.end(); edge = it.next()) {
            edges.push_back(TarjanEdge(u, edge.w, edge.weight));
        }
    }
}

std::vector<TarjanEdge> TarjanMST::findMinimumSpanningArborescence(int root) {
    std::vector<TarjanEdge> result;
    
    if (edges.empty()) {
        std::cout << "Grafo vazio!" << std::endl;
        return result;
    }
    
    // Passo 1: Encontrar arestas de entrada mínima para cada vértice
    findMinIncomingEdges();
    minCost[root] = 0; // Raiz não tem aresta de entrada
    
    // Verificar se existe solução
    for (int i = 0; i < numVertices; i++) {
        if (i != root && minCost[i] == INF) {
            std::cout << "Não existe arborescência a partir da raiz " << root << std::endl;
            return result;
        }
    }
    
    // Passo 2: Verificar se há ciclos
    int cycleCount = 0;
    std::fill(id.begin(), id.end(), -1);
    std::fill(visit.begin(), visit.end(), -1);
    
    for (int v = 0; v < numVertices; v++) {
        if (v == root) continue;
        
        if (visit[v] == -1) {
            // DFS para detectar ciclos
            int u = v;
            while (visit[u] != v && id[u] == -1 && u != root) {
                visit[u] = v;
                u = parent[u];
            }
            
            // Se encontrou um ciclo
            if (u != root && id[u] == -1) {
                // Marcar todos os vértices do ciclo
                while (id[u] == -1) {
                    id[u] = cycleCount;
                    u = parent[u];
                }
                cycleCount++;
            }
        }
    }
    
    // Se não há ciclos, construir resultado
    if (cycleCount == 0) {
        for (int v = 0; v < numVertices; v++) {
            if (v != root) {
                result.push_back(TarjanEdge(parent[v], v, minCost[v]));
            }
        }
        return result;
    }
    
    // Caso contrário, contrair ciclos e resolver recursivamente
    // (Implementação simplificada - em caso real seria necessário contrair os ciclos)
    std::cout << "Ciclos detectados. Implementação completa requer contração de ciclos." << std::endl;
    
    return result;
}

void TarjanMST::findMinIncomingEdges() {
    // Inicializar com infinito
    std::fill(minCost.begin(), minCost.end(), INF);
    std::fill(parent.begin(), parent.end(), -1);
    
    // Para cada aresta, verificar se é a menor entrada para o vértice destino
    for (const auto& edge : edges) {
        if (edge.weight < minCost[edge.to]) {
            minCost[edge.to] = edge.weight;
            parent[edge.to] = edge.from;
        }
    }
}

double TarjanMST::getTotalWeight() const {
    double total = 0;
    for (int i = 0; i < numVertices; i++) {
        if (minCost[i] != INF) {
            total += minCost[i];
        }
    }
    return total;
}

void TarjanMST::printArborescence(const std::vector<TarjanEdge>& arborescence) const {
    std::cout << "\n--- Arborescência Geradora Mínima (Tarjan) ---" << std::endl;
    double totalWeight = 0;
    
    for (const auto& edge : arborescence) {
        std::cout << edge.from << " -> " << edge.to << " (peso: " << edge.weight << ")" << std::endl;
        totalWeight += edge.weight;
    }
    
    std::cout << "Peso total: " << totalWeight << std::endl;
}