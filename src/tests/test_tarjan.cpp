#include <iostream>
#include "WeightedGraph.h"
#include "TarjanMST.h"

using namespace std;

void imprimirResultado(WeightedGraph& g) {
    double pesoTotal = 0;
    cout << "--- Arborescencia Gerada (Tarjan) ---" << endl;
    for (int i = 0; i < g.V(); ++i) {
        WeightedGraph::AdjIterator it(g, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            cout << e.v << " -> " << e.w << " [Peso: " << e.weight << "]" << endl;
            pesoTotal += e.weight;
            if (it.end()) break;
            e = it.next();
        }
    }
    cout << "Peso Total: " << pesoTotal << endl;
    cout << "-------------------------------------" << endl;
}

int main() {
    cout << "=== Teste: Algoritmo de Tarjan ===" << endl;

    // Caso de teste: Grafo com ciclo complexo
    // 4 vértices, Raiz 0
    int V = 4;
    WeightedGraph grafo(V, true);

    /// Grafo com ciclo e múltiplas entradas
    grafo.insertEdge(0, 1, 10.0);
    grafo.insertEdge(0, 2, 2.0);  // Caminho ótimo para entrar no ciclo
    grafo.insertEdge(0, 3, 20.0);
    grafo.insertEdge(1, 2, 5.0);
    grafo.insertEdge(2, 3, 5.0);
    grafo.insertEdge(3, 1, 5.0);

    cout << "Calculando Arborescencia para raiz 0..." << endl;
    WeightedGraph mst = TarjanMST::obterArborescencia(grafo, 0);
    
    imprimirResultado(mst);

    return 0;
}