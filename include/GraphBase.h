#ifndef GRAPHBASE_H
#define GRAPHBASE_H

class GraphBase {
protected:
    int numV;       // Number of vertices
    int numE;       // Number of edges
    bool directed;  // Is the graph directed?

public:
    GraphBase(int V, bool directed);
    virtual ~GraphBase();

    int V() const;
    int E() const;
    bool isDirected() const;

    virtual void insertEdge(int, int) = 0;
    virtual bool hasEdge(int, int) const = 0;
};

#endif
