#include <iostream>
#include "WeightedGraph.h"
#include "GabowMST.h"

using namespace std;

void imprimirGabow(WeightedGraph& g) {
    double total = 0;
    cout << "--- Arborescencia (Gabow) ---" << endl;
    for (int i = 0; i < g.V(); ++i) {
        WeightedGraph::AdjIterator it(g, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            cout << e.v << " -> " << e.w << " [Peso: " << e.weight << "]" << endl;
            total += e.weight;
            if (it.end()) break;
            e = it.next();
        }
    }
    cout << "Peso Total: " << total << endl;
    cout << "-----------------------------" << endl;
}

int main() {
    cout << "=== Teste: Algoritmo de Gabow ===" << endl;

    // Mesmo caso de teste para garantir consistência
    int V = 4;
    WeightedGraph g(V, true);

    // Grafo com ciclo e múltiplas entradas
    g.insertEdge(0, 1, 10.0);
    g.insertEdge(0, 2, 2.0);  // Caminho ótimo para entrar no ciclo
    g.insertEdge(0, 3, 20.0);
    
    // Ciclo 1-2-3
    g.insertEdge(1, 2, 5.0);
    g.insertEdge(2, 3, 5.0);
    g.insertEdge(3, 1, 5.0);

    // Resultado esperado: 0->2 (2), 2->3 (5), 3->1 (5) = Total 12.
    
    WeightedGraph mst = GabowMST::obterArborescencia(g, 0);
    imprimirGabow(mst);

    return 0;
}