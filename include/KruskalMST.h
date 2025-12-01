#ifndef KRUSKAL_MST_H
#define KRUSKAL_MST_H

#include "WeightedGraph.h"
#include <vector>

class KruskalMST {
public:
    /**
     * Executa o algoritmo de Kruskal para encontrar a Árvore Geradora Mínima (MST).
     * @param grafoEntrada O grafo ponderado de origem (deve ser não-direcionado para MST clássica).
     * @return Um novo WeightedGraph contendo apenas as arestas da MST.
     */
    static WeightedGraph obterArvoreGeradoraMinima(WeightedGraph& grafoEntrada);
};

#endif // KRUSKAL_MST_H