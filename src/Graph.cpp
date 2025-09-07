#include "Graph.h"
#include <iostream>
using namespace std;

// ---------- GraphBase ----------
GraphBase::GraphBase(int V, bool directed) : numV(V), numE(0), directed(directed) {}
GraphBase::~GraphBase() {}

int GraphBase::V() const { return numV; }
int GraphBase::E() const { return numE; }
bool GraphBase::isDirected() const { return directed; }

// ---------- Graph ----------
Graph::Graph(int V, bool directed) : GraphBase(V, directed), adj(V) {}
Graph::~Graph() {}

// Insere um novo vértice ao grafo
void Graph::insertVertex() {
    adj.push_back(std::vector<Edge>());
    numV++;
}

// Remove o vértice v e todas as arestas associadas
void Graph::removeVertex(int v) {
    if (v < 0 || v >= numV) return;
    // Remove todas as arestas de saída do vértice v
    numE -= adj[v].size();
    adj.erase(adj.begin() + v);
    // Remove todas as arestas de entrada para v
    for (auto& lista : adj) {
        for (auto it = lista.begin(); it != lista.end(); ) {
            if (it->w == v) {
                it = lista.erase(it);
                numE--;
            } else {
                // Ajusta índices dos vértices maiores que v
                if (it->w > v) it->w--;
                if (it->v > v) it->v--;
                ++it;
            }
        }
    }
    numV--;
}

void Graph::insertEdge(int v, int w) {
    if (!hasEdge(v, w)) {
        adj[v].push_back(Edge(v, w));
        numE++;
        if (!directed && v != w) {
            adj[w].push_back(Edge(w, v));
        }
    }
}

bool Graph::hasEdge(int v, int w) const {
    for (const auto& edge : adj[v]) {
        if (edge.w == w) return true;
    }
    return false;
}

bool Graph::edge(int v, int w) { return hasEdge(v, w); }

// Remove a aresta v -> w, caso exista
void Graph::removeEdge(int v, int w) {
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it) {
        if (it->w == w) {
            adj[v].erase(it);
            numE--;
            break;
        }
    }
}

// Imprime o grafo
void Graph::print() const {
    for (int v = 0; v < (int)adj.size(); ++v) {
        cout << v << ": ";
        for (const auto& edge : adj[v]) {
            cout << edge.w << " ";
        }
        cout << endl;
    }
}
// ---------- AdjIterator ----------
Graph::AdjIterator::AdjIterator(const Graph& G, int v) : G(G), v(v), index(0) {}

Edge Graph::AdjIterator::begin() {
    index = 0;
    return next();
}

Edge Graph::AdjIterator::next() {
    if (index < (int)G.adj[v].size())
        return G.adj[v][index++];
    else
        return Edge(-1, -1);
}

bool Graph::AdjIterator::end() {
    return index >= (int)G.adj[v].size();
}
