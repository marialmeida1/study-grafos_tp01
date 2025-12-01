#include <iostream>
#include "WeightedGraph.h"
#include "EdmondsMST.h"
#include <cmath>

using namespace std;

// Função auxiliar para imprimir o grafo
void imprimirResultado(WeightedGraph& g) {
    double pesoTotal = 0;
    cout << "--- Arborescencia Gerada ---" << endl;
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
    cout << "Peso Total da Arborescencia: " << pesoTotal << endl;
    cout << "----------------------------" << endl;
}

int main() {
    cout << "=== Teste: Algoritmo de Edmonds (Chu-Liu) ===" << endl;

    // Teste 1: Grafo Simples com Ciclo
    // Raiz: 0
    // 0 -> 1 (10)
    // 0 -> 2 (10)
    // 1 -> 2 (5)
    // 2 -> 3 (5)
    // 3 -> 1 (5)  <-- Ciclo 1-2-3
    // O algoritmo deve escolher entrar no ciclo via 0->1 (custo 10) e usar as arestas internas baratas,
    // quebrando o ciclo na entrada de 1.
    
    int V = 4;
    WeightedGraph grafo(V, true); // true = Direcionado

    grafo.insertEdge(0, 1, 10.0);
    grafo.insertEdge(0, 2, 2.0);  
    grafo.insertEdge(0, 3, 20.0);
    grafo.insertEdge(1, 2, 5.0);
    grafo.insertEdge(2, 3, 5.0);
    grafo.insertEdge(3, 1, 5.0);

    cout << "\nGrafo Original Configurado (Com Ciclo 1-2-3)." << endl;

    WeightedGraph arborescencia = EdmondsMST::obterArborescencia(grafo, 0);
    imprimirResultado(arborescencia);

    // Teste 2: Verificar integridade
    // Se mudarmos o peso de 0->2 para ser muito barato (ex: 2.0),
    // ele deve preferir entrar no ciclo pelo nó 2.
    cout << "\n--- Teste 2: Mudando peso de entrada ---" << endl;
    WeightedGraph grafo2(V, true);
    grafo2.insertEdge(0, 1, 10.0);
    grafo2.insertEdge(0, 2, 2.0); // Muito barato!
    grafo2.insertEdge(1, 2, 5.0);
    grafo2.insertEdge(2, 3, 5.0);
    grafo2.insertEdge(3, 1, 5.0);

    WeightedGraph arborescencia2 = EdmondsMST::obterArborescencia(grafo2, 0);
    imprimirResultado(arborescencia2);

    return 0;
}