#include "WeightedGraph.h"
#include <iostream>

/* Construtor do grafo ponderado direcionado
   Recebe o número de vértices (V) e inicializa:
 - GraphBase(V, true) → grafo com V vértices e marcado como dirigido
 - adj(V) → vetor de listas de adjacência, cada posição representa as arestas de saída de um vértice*/
WeightedGraph::WeightedGraph(int V, bool directed)
    : GraphBase(V, directed), adj(V), directed(directed) {}  

// Destrutor do grafo ponderado
WeightedGraph::~WeightedGraph() {
    // Se não precisar desalocar nada manualmente, pode deixar vazio
}

// Insere uma aresta direcionada de v -> w com peso informado
// Só insere se a aresta ainda não existir, para evitar duplicatas    
void WeightedGraph::insertEdge(int v, int w, double peso) {
    if (!hasEdge(v, w)) {
        adj[v].push_back(WeightedEdge(v, w, peso));
        numE++;
    }
    if (!directed && !hasEdge(w, v)) {
        adj[w].push_back(WeightedEdge(w, v, peso));
        numE++;
    }
}

// Remove a aresta v -> w, caso exista
// Percorre a lista de adjacência de v, encontra o destino w e apaga
void WeightedGraph::removeEdge(int v, int w) {
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it) {
        if (it->w == w) {
            adj[v].erase(it);
            numE--;
            break;
        }
    }
    if (!directed) {
        for (auto it = adj[w].begin(); it != adj[w].end(); ++it) {
            if (it->w == v) {
                adj[w].erase(it);
                numE--;
                break;
            }
        }
    }
}

// Atualiza o peso de uma aresta existente v -> w
// Retorna true se conseguiu atualizar, false se a aresta não existe
bool WeightedGraph::updateWeight(int v, int w, double novoPeso) {
    for (auto& aresta : adj[v]) {
        if (aresta.w == w) {
            aresta.weight = novoPeso;
            return true;
        }
    }
    if(!directed){
        for (auto& aresta : adj[w]) {
            if (aresta.w == v) {
                aresta.weight = novoPeso;
                return true;
            }
        }
    }
    return false; // não encontrou a aresta
}

// Verifica se uma aresta v -> w existe no grafo
// Percorre a lista de adjacência de v procurando destino w
bool WeightedGraph::hasEdge(int v, int w) const {
    for (const auto& aresta : adj[v]) {
        if (aresta.w == w) return true;
    }
    return false;
}

// Retorna o peso de uma aresta v -> w
// Caso a aresta não exista, retorna -1 como valor "sentinela"
double WeightedGraph::getWeight(int v, int w) const {
    for(const auto& aresta : adj[v]){
        if(aresta.w == w){
            return aresta.weight;
        }
    }
    return -1;
}

// --- Iterator para grafos ponderados (funciona pra grafos não direcionais)
WeightedGraph::AdjIterator::AdjIterator(const WeightedGraph& G, int v)
    : G(G), v(v), index(0) {}

WeightedEdge WeightedGraph::AdjIterator::begin() {
    index = 0;
    return next();
}

WeightedEdge WeightedGraph::AdjIterator::next() {
    if (index < (int)G.adj[v].size())
        return G.adj[v][index++];
    else
        return WeightedEdge(-1, -1, -1);
}

bool WeightedGraph::AdjIterator::end() {
    return index >= (int)G.adj[v].size();
}

