#include "TarjanMST.h"
#include <iostream>
#include <algorithm>
#include <stack>
#include <set>

const double INF = std::numeric_limits<double>::infinity();

TarjanMST::TarjanMST(const WeightedGraph& graph) : numVertices(graph.V()) {
    
    // Extrair todas as arestas do grafo
    for (int v = 0; v < numVertices; v++) {
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
    inCycle.resize(numVertices, -1);
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
    
    std::cout << "DEBUG: Verificando ciclos nas arestas mínimas:" << std::endl;
    for (int i = 0; i < numVertices; i++) {
        if (i != root) {
            std::cout << "Vértice " << i << " <- " << parent[i] << " (peso: " << minCost[i] << ")" << std::endl;
        }
    }
    
    for (int v = 0; v < numVertices; v++) {
        if (v == root) continue;
        
        if (visit[v] == -1) {
            // DFS para detectar ciclos
            std::vector<int> path;
            int u = v;
            std::cout << "DEBUG: Iniciando DFS a partir do vértice " << v << std::endl;
            
            while (visit[u] != v && id[u] == -1 && u != root) {
                visit[u] = v;
                path.push_back(u);
                std::cout << "DEBUG: Visitando " << u << " -> " << parent[u] << std::endl;
                u = parent[u];
            }
            
            // Se encontrou um ciclo
            if (u != root && id[u] == -1) {
                std::cout << "DEBUG: CICLO DETECTADO! Vértice " << u << " já foi visitado." << std::endl;
                std::cout << "DEBUG: Ciclo: ";
                
                // Marcar todos os vértices do ciclo
                int cycleStart = u;
                do {
                    std::cout << u << " -> ";
                    id[u] = cycleCount;
                    u = parent[u];
                } while (u != cycleStart);
                std::cout << cycleStart << std::endl;
                
                cycleCount++;
            } else if (u == root) {
                std::cout << "DEBUG: Caminho de " << v << " chegou à raiz " << root << std::endl;
            }
        }
    }
    
    std::cout << "DEBUG: Total de ciclos detectados: " << cycleCount << std::endl;
    
    // Se não há ciclos, construir resultado
    if (cycleCount == 0) {
        for (int v = 0; v < numVertices; v++) {
            if (v != root) {
                result.push_back(TarjanEdge(parent[v], v, minCost[v]));
            }
        }
        return result;
    }
    
    // Contração de ciclos e resolução recursiva
    std::cout << "DEBUG: Iniciando contração de " << cycleCount << " ciclo(s)." << std::endl;
    return contractCyclesAndSolve(root, cycleCount);
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

std::vector<TarjanEdge> TarjanMST::contractCyclesAndSolve(int root, int cycleCount) {
    std::vector<TarjanEdge> result;
    
    // Criar mapeamento de nós: vértices originais -> vértices contraídos
    std::vector<int> nodeMapping(numVertices);
    int newNodeCount = 0;
    
    // Mapear vértices não-ciclo e raiz
    for (int v = 0; v < numVertices; v++) {
        if (v == root || id[v] == -1) {
            nodeMapping[v] = newNodeCount++;
        }
    }
    
    // Mapear cada ciclo para um único super-nó
    for (int c = 0; c < cycleCount; c++) {
        int superNode = newNodeCount++;
        for (int v = 0; v < numVertices; v++) {
            if (id[v] == c) {
                nodeMapping[v] = superNode;
            }
        }
    }
    
    std::cout << "DEBUG: Criando grafo contraído com " << newNodeCount << " vértices." << std::endl;
    
    // Criar grafo contraído
    WeightedGraph contractedGraph = createContractedGraph(root, cycleCount, nodeMapping);
    
    // Resolver recursivamente no grafo contraído
    TarjanMST contractedTarjan(contractedGraph);
    std::vector<TarjanEdge> contractedSolution = contractedTarjan.findMinimumSpanningArborescence(nodeMapping[root]);
    
    // Expandir solução para o grafo original
    result = expandSolution(contractedSolution, nodeMapping);
    
    return result;
}

WeightedGraph TarjanMST::createContractedGraph(int root, int cycleCount, std::vector<int>& nodeMapping) {
    int newNodeCount = 1 + (numVertices - 1) + cycleCount; // estimativa conservadora
    
    // Contar vértices reais
    std::set<int> uniqueNodes;
    for (int v = 0; v < numVertices; v++) {
        uniqueNodes.insert(nodeMapping[v]);
    }
    newNodeCount = uniqueNodes.size();
    
    WeightedGraph contractedGraph(newNodeCount, true);
    
    std::cout << "DEBUG: Mapeamento de vértices:" << std::endl;
    for (int v = 0; v < numVertices; v++) {
        std::cout << "Vértice " << v << " -> " << nodeMapping[v];
        if (id[v] != -1) {
            std::cout << " (ciclo " << id[v] << ")";
        }
        std::cout << std::endl;
    }
    
    // Adicionar arestas contraídas
    for (const auto& edge : edges) {
        int fromContracted = nodeMapping[edge.from];
        int toContracted = nodeMapping[edge.to];
        
        // Ignorar arestas dentro do mesmo super-nó (ciclo)
        if (fromContracted == toContracted) {
            continue;
        }
        
        // Ajustar peso da aresta se ela entra em um super-nó (ciclo)
        double adjustedWeight = edge.weight;
        if (id[edge.to] != -1) {
            // Se a aresta vai para um vértice em ciclo, subtrair o custo da aresta mínima do ciclo
            adjustedWeight = edge.weight - minCost[edge.to];
        }
        
        // Adicionar ou atualizar aresta no grafo contraído
        if (!contractedGraph.hasEdge(fromContracted, toContracted) || 
            contractedGraph.getWeight(fromContracted, toContracted) > adjustedWeight) {
            if (contractedGraph.hasEdge(fromContracted, toContracted)) {
                contractedGraph.updateWeight(fromContracted, toContracted, adjustedWeight);
            } else {
                contractedGraph.insertEdge(fromContracted, toContracted, adjustedWeight);
            }
        }
    }
    
    return contractedGraph;
}

std::vector<TarjanEdge> TarjanMST::expandSolution(const std::vector<TarjanEdge>& contractedSolution, 
                                                 const std::vector<int>& nodeMapping) {
    std::vector<TarjanEdge> expandedSolution;
    
    // Adicionar arestas da solução contraída
    for (const auto& edge : contractedSolution) {
        // Encontrar vértices originais correspondentes
        int originalFrom = -1, originalTo = -1;
        
        for (int v = 0; v < numVertices; v++) {
            if (nodeMapping[v] == edge.from) {
                originalFrom = v;
            }
            if (nodeMapping[v] == edge.to) {
                originalTo = v;
            }
        }
        
        // Se a aresta vai para um super-nó, precisamos encontrar o ponto de entrada
        if (originalTo == -1) {
            // Encontrar o vértice no ciclo que tem a aresta de entrada
            for (int v = 0; v < numVertices; v++) {
                if (nodeMapping[v] == edge.to && parent[v] != -1 && nodeMapping[parent[v]] == edge.from) {
                    originalTo = v;
                    break;
                }
            }
        }
        
        if (originalFrom != -1 && originalTo != -1) {
            expandedSolution.push_back(TarjanEdge(originalFrom, originalTo, edge.weight));
        }
    }
    
    // Adicionar arestas internas de cada ciclo (exceto uma para quebrar o ciclo)
    for (int v = 0; v < numVertices; v++) {
        if (id[v] != -1 && parent[v] != -1) {
            // Verificar se esta aresta não foi substituída por uma entrada externa
            bool isExternalEntry = false;
            for (const auto& contractedEdge : contractedSolution) {
                for (int u = 0; u < numVertices; u++) {
                    if (nodeMapping[u] == contractedEdge.from && 
                        nodeMapping[v] == contractedEdge.to && 
                        nodeMapping[u] != nodeMapping[v]) {
                        isExternalEntry = true;
                        break;
                    }
                }
                if (isExternalEntry) break;
            }
            
            if (!isExternalEntry) {
                expandedSolution.push_back(TarjanEdge(parent[v], v, minCost[v]));
            }
        }
    }
    
    return expandedSolution;
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