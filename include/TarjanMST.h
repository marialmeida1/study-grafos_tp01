#ifndef TARJAN_MST_H
#define TARJAN_MST_H

#include "WeightedGraph.h"
#include <vector>

class TarjanMST {
public:
    /**
     * Implementação eficiente do algoritmo de Arborescência Geradora Mínima
     * baseada no artigo de Tarjan (1977), utilizando Skew Heaps e Union-Find.
     * Complexidade esperada: O(E log V).
     * * @param grafo Grafo direcionado ponderado.
     * @param raiz Vértice raiz da arborescência.
     * @return Novo grafo contendo apenas as arestas da arborescência.
     */
    static WeightedGraph obterArborescencia(WeightedGraph& grafo, int raiz);
};

#endif // TARJAN_MST_H