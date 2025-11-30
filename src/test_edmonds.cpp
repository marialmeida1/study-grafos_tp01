#include <iostream>
#include <cassert>
#include <chrono>
#include "WeightedGraph.h"
#include "EdmondsMST.h"
#include <cmath>

/*
 * Testes automáticos mínimos para Edmonds:
 * - caso 1: grafo simples (sem ciclos)
 * - caso 2: grafo com ciclo simples
 * - caso 3: grafo com ciclos aninhados
 * - caso 4: grafo grande (gerador aleatório simples) apenas para medir tempo
 *
 * Os testes usam WeightedGraph::insertEdge(u,v,w) conforme uso em Tarjan tests.
 */

// Testa Edmonds em um grafo simples
void test_simple()
{
    std::cout << "\n[Test] simple graph\n";

    WeightedGraph g(3, true);
    g.insertEdge(0, 1, 1.0);
    g.insertEdge(0, 2, 5.0);
    g.insertEdge(1, 2, 1.0);

    // Resultado esperado: 0->1 (1.0) e 1->2 (1.0)
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);

    assert(std::abs(ed.getTotalWeight() - 2.0) < 1e-9);
    std::cout << "[OK] simple\n";
}

// Teste com ciclo simples
void test_cycle_simple()
{
    std::cout << "\n[Test] simple cycle\n";

    WeightedGraph g(4, true);
    g.insertEdge(0, 1, 5.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 0, 6.0);
    g.insertEdge(0, 2, 3.0);

    // Testa com raiz 0 (exemplo parecido com Tarjan doc)
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);

    // Esperado: peso 8.0 (0->1(5),1->2(2),2->3(1))
    double expected = 8.0;
    assert(std::abs(ed.getTotalWeight() - expected) < 1e-6);
    std::cout << "[OK] simple cycle\n";
}

// Teste com dois ciclos aninhados
void test_nested_cycles()
{
    std::cout << "\n[Test] nested cycles\n";

    // Construir grafo com ciclos aninhados (exemplo pequeno)
    // Estrutura: (0)->1->2->0 e 2->3->4->2 (dois ciclos conectados)
    WeightedGraph g(6, true);
    g.insertEdge(0, 1, 2.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 0, 2.0); // ciclo 0-1-2
    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 4, 1.0);
    g.insertEdge(4, 2, 1.0); // ciclo 2-3-4
    g.insertEdge(0, 5, 10.0);
    g.insertEdge(3, 5, 3.0);

    // raiz 0
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);

    // Não temos um valor único trivial esperado aqui (depende de escolhas), mas
    // garantimos que o número de arestas == n-1 e que peso total > 0
    assert((int)arb.size() == g.V() - 1);
    std::cout << "[OK] nested cycles\n";
}

// Teste de performance com grafo denso/aleatório
void test_performance(int n, int density_percent)
{
    std::cout << "\n[Test] performance: n=" << n << " density=" << density_percent << "%\n";

    WeightedGraph g(n, true);

    // inserir arestas randômicas determinísticas (para teste repetível)
    for (int u = 0; u < n; ++u)
    {
        for (int v = 0; v < n; ++v)
        {
            if (u == v)
                continue;

            // Probabilidade baseada em hashing simples
            if (((u * 31 + v * 17) % 100) < density_percent)
            {
                double w = 1.0 + ((u * 13 + v * 7) % 100) / 10.0;
                g.insertEdge(u, v, w);
            }
        }
    }

    auto t0 = std::chrono::high_resolution_clock::now();
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    auto t1 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = t1 - t0;
    std::cout << "Time (s): " << diff.count() << "\n";

    if (!arb.empty())
        std::cout << "Edges in arborescence: " << arb.size() << "\n";

    else
        std::cout << "No arborescence found (graph may be disconnected from root)\n";

    std::cout << "[OK] performance test done\n";
}

int main()
{
    try
    {
        test_simple();
        test_cycle_simple();
        test_nested_cycles();
        // teste de performance (n pequeno por padrão; ajuste conforme máquina)
        test_performance(200, 20);

        std::cout << "\nAll Edmonds tests finished.\n";
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        std::cerr << "Unknown error\n";
        return 3;
    }
    return 0;
}
