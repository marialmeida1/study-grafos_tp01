#include "KruskalMST.h"
#include <algorithm>

// ---------- DSU SIMPLES (Union-Find) ----------
struct DSU {
    std::vector<int> parent, rankv;

    DSU(int n = 0) { init(n); }

    void init(int n) {
        parent.resize(n);
        rankv.assign(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);  // compressão de caminho
        return parent[x];
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;        // já conectados

        if (rankv[a] < rankv[b]) std::swap(a, b);
        parent[b] = a;
        if (rankv[a] == rankv[b]) rankv[a]++;
        return true;
    }
};


// ---------- CRIA ESTRUTURA COM TODAS AS ARESTAS ----------
KruskalMST::KruskalMST(const WeightedGraph &g) {
    n = g.V();

    edges.clear();

    // Percorre cada vértice e coleta arestas
    for (int u = 0; u < n; u++) {
        WeightedGraph::AdjIterator it(g, u);
        for (WeightedEdge e = it.begin(); !it.end(); e = it.next()) {
            if (e.v != e.w) {
                edges.emplace_back(e.v, e.w, e.weight);
            }
        }
    }
}


// ---------- EXECUTA KRUSKAL ----------
const std::vector<KEdge>& KruskalMST::compute() {
    mst.clear();
    totalWeight = 0.0;

    // ordena por peso
    std::sort(edges.begin(), edges.end(),
        [](const KEdge &a, const KEdge &b) {
            return a.w < b.w;
        }
    );

    DSU dsu(n);

    // seleciona arestas válidas
    for (auto &e : edges) {
        if (dsu.unite(e.u, e.v)) {
            mst.push_back(e);
            totalWeight += e.w;
            if ((int)mst.size() == n - 1) break;
        }
    }

    // caso grafo desconexo, mst ficará menor que n-1
    return mst;
}
