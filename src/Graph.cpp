#include "Graph.h"

// ---------- GraphBase ----------
GraphBase::GraphBase(int V, bool directed) : numV(V), numE(0), directed(directed) {}
GraphBase::~GraphBase() {}

int GraphBase::V() const { return numV; }
int GraphBase::E() const { return numE; }
bool GraphBase::isDirected() const { return directed; }

// ---------- Graph ----------
Graph::Graph(int V, bool directed) : GraphBase(V, directed), adj(V) {}
Graph::~Graph() {}

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
