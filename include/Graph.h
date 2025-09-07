#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "Edge.h"
#include "GraphBase.h"

class Graph : public GraphBase {
private:
    std::vector<std::vector<Edge>> adj;

public:
    Graph(int V, bool directed);
    ~Graph();

    // Inserção e remoção de vértices
    void insertVertex();
    void removeVertex(int v);

    void insertEdge(int v, int w) override;
    void removeEdge(int v, int w);
    bool hasEdge(int v, int w) const override;
    bool edge(int v, int w);

    // Impressão do grafo
    void print() const;

    class AdjIterator {
        const Graph& G;
        int v, index;
    public:
        AdjIterator(const Graph& G, int v);
        Edge begin();
        Edge next();
        bool end();
    };
};

#endif
