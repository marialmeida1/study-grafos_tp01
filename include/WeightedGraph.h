#ifndef WEIGHTEDGRAPH_H
#define WEIGHTEDGRAPH_H

#include <vector>
#include "WeightedEdge.h"
#include "GraphBase.h"

class WeightedGraph : public GraphBase {
private:
    std::vector<std::vector<WeightedEdge>> adj;

public:
    WeightedGraph(int V);
    ~WeightedGraph();

    // Insertion
    void insertEdge(int v, int w, double weight);
    void insertEdge(int v, int w) override { insertEdge(v, w, 1.0); }

    // Removal
    void removeEdge(int v, int w);

    // Update weight
    bool updateWeight(int v, int w, double newWeight);

    // Verification
    bool hasEdge(int v, int w) const override;

    // Get weight of an edge
    double getWeight(int v, int w) const;

    // Iterator
    class AdjIterator {
        const WeightedGraph& G;
        int v, index;
    public:
        AdjIterator(const WeightedGraph& G, int v);
        WeightedEdge begin();
        WeightedEdge next();
        bool end();
    };
};

#endif
