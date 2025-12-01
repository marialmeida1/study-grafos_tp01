#ifndef GABOW_MST_H
#define GABOW_MST_H

#include "WeightedGraph.h"
#include <vector>

class GabowMST {
public:
    /**
     * Implementação robusta do algoritmo de Gabow (1986) para Arborescência Geradora Mínima.
     * Utiliza estratégia iterativa (Path Growing) com Skew Heaps e Lazy Propagation.
     * Ideal para grafos grandes (ex: imagens).
     * * @param grafo Grafo direcionado ponderado.
     * @param raiz Vértice raiz da arborescência (geralmente 0 para imagens).
     * @return Arborescência resultante como um novo WeightedGraph.
     */
    static WeightedGraph obterArborescencia(WeightedGraph& grafo, int raiz);
};

#endif // GABOW_MST_H