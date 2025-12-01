#include "WeightedGraph.h"
#include <iostream>

WeightedGraph::WeightedGraph(int V, bool directed)
    : GraphBase(V, directed), adj(V), directed(directed) {}  

WeightedGraph::~WeightedGraph() {
}

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
    return false; 
}

bool WeightedGraph::hasEdge(int v, int w) const {
    for (const auto& aresta : adj[v]) {
        if (aresta.w == w) return true;
    }
    return false;
}

double WeightedGraph::getWeight(int v, int w) const {
    for(const auto& aresta : adj[v]){
        if(aresta.w == w){
            return aresta.weight;
        }
    }
    return -1;
}

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

