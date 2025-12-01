#include "KruskalMST.h"
#include <algorithm>
#include <vector>
#include <iostream>

struct ConjuntoDisjunto {
    std::vector<int> pai;
    std::vector<int> rank;

    ConjuntoDisjunto(int n) {
        pai.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) {
            pai[i] = i;
        }
    }

    int buscar(int v) {
        if (v != pai[v])
            pai[v] = buscar(pai[v]); 
        return pai[v];
    }

    void unir(int v1, int v2) {
        int raiz1 = buscar(v1);
        int raiz2 = buscar(v2);

        if (raiz1 != raiz2) {
            if (rank[raiz1] < rank[raiz2]) {
                pai[raiz1] = raiz2;
            } else if (rank[raiz1] > rank[raiz2]) {
                pai[raiz2] = raiz1;
            } else {
                pai[raiz2] = raiz1;
                rank[raiz1]++;
            }
        }
    }
};

WeightedGraph KruskalMST::obterArvoreGeradoraMinima(WeightedGraph& grafoEntrada) {
    int numVertices = grafoEntrada.V();
    
    WeightedGraph mst(numVertices, false);

    std::vector<WeightedEdge> listaArestas;
    
    for (int i = 0; i < numVertices; ++i) {
        WeightedGraph::AdjIterator it(grafoEntrada, i);
        
        WeightedEdge aresta = it.begin();
        while (true) {
            if (aresta.v == -1) break;

            if (aresta.v < aresta.w) {
                listaArestas.push_back(aresta);
            }
            
            if (it.end()) break;
            aresta = it.next();
        }
    }

    std::sort(listaArestas.begin(), listaArestas.end(), 
        [](const WeightedEdge& a, const WeightedEdge& b) {
            return a.weight < b.weight;
        });

    ConjuntoDisjunto ds(numVertices);
    int arestasAdicionadas = 0;

    for (const auto& aresta : listaArestas) {
        int u = aresta.v;
        int v = aresta.w;

        if (ds.buscar(u) != ds.buscar(v)) {
            ds.unir(u, v);
            mst.insertEdge(u, v, aresta.weight);
            arestasAdicionadas++;
        }
    }

    return mst;
}