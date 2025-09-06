#ifndef EDGE_H
#define EDGE_H

// Represents an edge between two vertices v and w.
struct Edge {
    int v, w;
    Edge(int v = -1, int w = -1) : v(v), w(w) {}
};

#endif
