#ifndef EDMONDS_MST_H
#define EDMONDS_MST_H

#include "WeightedGraph.h"
#include <vector>
#include <list>

class EdmondsMST {
public:
    /**
     * Calcula a Arborescência Geradora Mínima (Minimum Spanning Arborescence)
     * para um grafo direcionado, dado um vértice raiz.
     * * @param grafo O grafo de entrada (deve ser direcionado).
     * @param raiz O ID do vértice raiz (0 a V-1).
     * @return Um novo WeightedGraph contendo a arborescência.
     */
    static WeightedGraph obterArborescencia(WeightedGraph& grafo, int raiz);

private:
    // Estrutura interna para facilitar a manipulação de arestas durante a recursão
    struct ArestaInterna {
        int u, v;
        double peso;
        int idOriginal; // Rastreia qual aresta real é essa (para reconstrução)
    };

    // Método auxiliar recursivo que realiza a lógica de Chu-Liu/Edmonds
    static std::vector<ArestaInterna> processarEdmonds(int numVertices, int raiz, const std::vector<ArestaInterna>& arestas);
};

#endif // EDMONDS_MST_H