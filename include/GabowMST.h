#ifndef GABOW_MST_H
#define GABOW_MST_H

#include "WeightedGraph.h"
#include <vector>

class GabowMST {
public:
    /**
     * Implementação do algoritmo de Gabow, Galil, Spencer e Tarjan (1986).
     * Utiliza a estratégia de "Path Growing" com Skew Heaps.
     * * @param grafo O grafo direcionado de entrada.
     * @param raiz O vértice raiz da arborescência.
     * @return Um novo WeightedGraph contendo a arborescência mínima.
     */
    static WeightedGraph obterArborescencia(WeightedGraph& grafo, int raiz);
};

#endif // GABOW_MST_H