#include "KruskalMST.h"
#include <algorithm>
#include <vector>
#include <iostream>

// Estrutura auxiliar interna para gerenciar componentes conexos (Union-Find)
// Otimizada com "Path Compression" e "Union by Rank"
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
            pai[v] = buscar(pai[v]); // Path compression
        return pai[v];
    }

    void unir(int v1, int v2) {
        int raiz1 = buscar(v1);
        int raiz2 = buscar(v2);

        if (raiz1 != raiz2) {
            // Union by rank
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
    
    // Cria o grafo de resultado (Não direcionado, pois é MST)
    WeightedGraph mst(numVertices, false);

    // 1. Extrair todas as arestas do grafo usando o Iterator da sua classe
    std::vector<WeightedEdge> listaArestas;
    
    for (int i = 0; i < numVertices; ++i) {
        WeightedGraph::AdjIterator it(grafoEntrada, i);
        
        // Loop seguro para usar o seu Iterator
        WeightedEdge aresta = it.begin();
        while (true) {
            if (aresta.v == -1) break; // Validação de segurança do iterador

            // TRUQUE: Como o grafo não direcionado tem arestas (u,v) e (v,u),
            // pegamos apenas quando origem < destino para evitar duplicatas na ordenação.
            if (aresta.v < aresta.w) {
                listaArestas.push_back(aresta);
            }
            
            if (it.end()) break;
            aresta = it.next();
        }
    }

    // 2. Ordenar arestas pelo peso (Crescente)
    std::sort(listaArestas.begin(), listaArestas.end(), 
        [](const WeightedEdge& a, const WeightedEdge& b) {
            return a.weight < b.weight;
        });

    // 3. Processar arestas com Union-Find
    ConjuntoDisjunto ds(numVertices);
    int arestasAdicionadas = 0;

    for (const auto& aresta : listaArestas) {
        int u = aresta.v;
        int v = aresta.w;

        // Se u e v não estão no mesmo componente, unir e adicionar à MST
        if (ds.buscar(u) != ds.buscar(v)) {
            ds.unir(u, v);
            mst.insertEdge(u, v, aresta.weight);
            arestasAdicionadas++;
        }
    }

    return mst;
}