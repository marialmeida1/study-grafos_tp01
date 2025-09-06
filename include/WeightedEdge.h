#ifndef WEIGHTEDEDGE_H
#define WEIGHTEDEDGE_H

// Represents a weighted directed edge between two vertices v and w.
struct WeightedEdge {
    int v, w;
    double weight;

    WeightedEdge(int v = -1, int w = -1, double weight = 1.0)
        : v(v), w(w), weight(weight) {}
};

#endif
