#include <iostream>
#include "KruskalMST.h"

using namespace std;

void imprimirGrafo(WeightedGraph& g) {
    double pesoTotal = 0;
    cout << "--- Estrutura do Grafo ---" << endl;
    for (int i = 0; i < g.V(); ++i) {
        WeightedGraph::AdjIterator it(g, i);
        WeightedEdge e = it.begin();
        while (true) {
            if (e.v == -1) break;
            
            // Para não imprimir duplicado na visualização (u-v e v-u)
            if (e.v < e.w) {
                cout << e.v << " - " << e.w << " [Peso: " << e.weight << "]" << endl;
                pesoTotal += e.weight;
            }
            
            if (it.end()) break;
            e = it.next();
        }
    }
    cout << "Peso Total: " << pesoTotal << endl;
    cout << "--------------------------" << endl;
}

int main() {
    cout << "=== Teste Algoritmo de Kruskal ===" << endl;

    // Criando um grafo não direcionado com 6 vértices (0 a 5)
    int V = 6;
    WeightedGraph grafo(V, false);

    // Adicionando arestas (Exemplo clássico)
    grafo.insertEdge(0, 1, 4.0);
    grafo.insertEdge(0, 2, 4.0);
    grafo.insertEdge(1, 2, 2.0);
    grafo.insertEdge(1, 0, 4.0); // Redundante, mas testando robustez
    grafo.insertEdge(2, 3, 3.0);
    grafo.insertEdge(2, 5, 2.0);
    grafo.insertEdge(2, 4, 4.0);
    grafo.insertEdge(3, 4, 3.0);
    grafo.insertEdge(5, 4, 3.0);

    cout << "\nGrafo Original:" << endl;
    imprimirGrafo(grafo);

    // Executando Kruskal
    WeightedGraph mst = KruskalMST::obterArvoreGeradoraMinima(grafo);

    cout << "\nGrafo MST (Kruskal):" << endl;
    imprimirGrafo(mst);

    return 0;
}