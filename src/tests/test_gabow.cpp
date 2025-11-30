#include <iostream>
#include <cassert>
#include <chrono>
#include "WeightedGraph.h"
#include "GabowMST.h"

/*
 * Testes automáticos mínimos para Gabow:
 * - caso 1: grafo simples (sem ciclos)
 * - caso 2: grafo com ciclo simples
 * - caso 3: grafo com ciclos aninhados
 * - caso 4: grafo grande (gerador aleatório simples) apenas para medir tempo
 */

// Testa Gabow em um grafo simples
void test_simple() {
    std::cout << "\n[Teste] Grafo simples\n";

    WeightedGraph g(3, true);
    g.insertEdge(0,1,1.0);
    g.insertEdge(0,2,5.0);
    g.insertEdge(1,2,1.0);

    // Resultado esperado: 0->1 (1.0) e 1->2 (1.0)
    GabowMST gbst(g);
    auto arb = gbst.compute(0);
    gbst.printArborescence(arb);

    assert(std::abs(gbst.getTotalWeight() - 2.0) < 1e-9);
}

// Teste com ciclo simples
void test_cycle_simple() {
    std::cout << "\n[Teste] Ciclo simples\n";

    WeightedGraph g(4, true);
    g.insertEdge(0,1,5.0);
    g.insertEdge(1,2,2.0);
    g.insertEdge(2,3,1.0);
    g.insertEdge(3,0,6.0);
    g.insertEdge(0,2,3.0);

    GabowMST gbst(g);
    auto arb = gbst.compute(0);
    gbst.printArborescence(arb);

    // Esperado: peso total 8.0
    assert(std::abs(gbst.getTotalWeight() - 8.0) < 1e-6);
    std::cout << "[OK] Ciclo simples\n";
}

// Teste com ciclos aninhados
void test_nested_cycles() {
    std::cout << "\n[Teste] Ciclos aninhados\n";

    WeightedGraph g(6, true);
    g.insertEdge(0,1,2.0);
    g.insertEdge(1,2,2.0);
    g.insertEdge(2,0,2.0); // ciclo 0-1-2
    g.insertEdge(2,3,1.0);
    g.insertEdge(3,4,1.0);
    g.insertEdge(4,2,1.0); // ciclo 2-3-4
    g.insertEdge(0,5,10.0);
    g.insertEdge(3,5,3.0);

    GabowMST gbst(g);
    auto arb = gbst.compute(0);
    gbst.printArborescence(arb);

    // Garantir que número de arestas = n-1
    assert((int)arb.size() == g.V() - 1);
    std::cout << "[OK] Ciclos aninhados\n";
}

// Teste de performance com grafo denso/aleatório
void test_performance(int n, int density_percent) {
    std::cout << "\n[Teste] Performance: n=" << n << " densidade=" << density_percent << "%\n";

    WeightedGraph g(n, true);

    // inserir arestas randômicas determinísticas (para teste repetível)
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (u == v) continue;

            // Probabilidade baseada em hashing simples
            if (((u * 31 + v * 17) % 100) < density_percent) {
                double w = 1.0 + ((u * 13 + v * 7) % 100) / 10.0;
                g.insertEdge(u,v,w);
            }
        }
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    GabowMST gbst(g);
    auto arb = gbst.compute(0);
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = t1 - t0;
    std::cout << "Tempo (s): " << diff.count() << "\n";

    if (!arb.empty())
        std::cout << "Arestas na arborescência: " << arb.size() << "\n";
    else
        std::cout << "Nenhuma arborescência encontrada (grafo pode estar desconectado da raiz)\n";

    std::cout << "[OK] Teste de performance concluído\n";
}

int main() {
    try {
        test_simple();
        test_cycle_simple();
        test_nested_cycles();
        test_performance(200, 20);

        std::cout << "\nTodos os testes do Gabow fiUmnalizados.\n";
    } catch (const std::exception& ex) {
        std::cerr << "Exceção: " << ex.what() << "\n";
        return 2;
    } catch (...) {
        std::cerr << "Erro desconhecido\n";
        return 3;
    }
    return 0;
}
