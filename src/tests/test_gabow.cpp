#include <iostream>
#include <vector>
#include <cassert>

#include "WeightedGraph.h"
#include "GabowMST.h"

using namespace std;

// Verifica se forma uma arborescência válida:
// - (n-1) arestas, sem ciclos, cada vértice tem exatamente 1 pai
bool isValidArborescence(int n, int root, const vector<GabowEdge>& arb) {
    if ((int)arb.size() != n - 1) return false;

    vector<int> parent(n, -1);
    vector<vector<int>> adj(n);

    // Verifica pais e monta a adjacência
    for (auto& e : arb) {
        if (e.to < 0 || e.to >= n) return false;
        if (e.from < 0 || e.from >= n) return false;
        if (e.to == root) return false;        // raiz não pode ter pai
        if (parent[e.to] != -1) return false;  // 2 pais → inválido

        parent[e.to] = e.from;
        adj[e.from].push_back(e.to);
    }

    // DFS para detectar ciclo
    vector<int> vis(n, 0);

    function<bool(int)> dfs = [&](int u) {
        vis[u] = 1;
        for (int v : adj[u]) {
            if (vis[v] == 1) return false; // ciclo
            if (vis[v] == 0 && !dfs(v)) return false;
        }
        vis[u] = 2;
        return true;
    };

    if (!dfs(root)) return false;

    // Todos os vértices devem ser alcançáveis da raiz
    for (int i = 0; i < n; i++)
        if (vis[i] == 0) return false;

    return true;
}

void printARB(const vector<GabowEdge>& arb) {
    cout << "Arborescência retornada:\n";
    for (auto& e : arb)
        cout << e.from << " -> " << e.to << " (peso: " << e.weight << ")\n";
    cout << '\n';
}

/*
 * Caso 1: Grafo simples sem ciclos.
 */
void test_simple_no_cycle() {
    cout << "[TESTE 1] Grafo simples sem ciclos...\n";

    WeightedGraph g(4, true);
    g.insertEdge(0, 1, 1.0);
    g.insertEdge(0, 2, 5.0);
    g.insertEdge(1, 2, 1.0);
    g.insertEdge(1, 3, 2.0);
    g.insertEdge(2, 3, 1.0);

    GabowMST mst(g);
    auto arb = mst.compute(0);

    printARB(arb);

    assert(isValidArborescence(4, 0, arb));

    double peso = 0.0;
    for (auto& e : arb) peso += e.weight;

    assert(fabs(peso - 3.0) < 1e-9); 
    cout << "OK!\n\n";
}

/*
 * Caso 2: Ciclo simples (1 → 2 → 3 → 1)
 */
void test_simple_cycle() {
    cout << "[TESTE 2] Ciclo simples 1->2->3->1...\n";

    WeightedGraph g(4, true);
    g.insertEdge(0, 1, 5.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 3, 2.0);
    g.insertEdge(3, 1, 1.0);

    GabowMST mst(g);
    auto arb = mst.compute(0);

    printARB(arb);

    assert(isValidArborescence(4, 0, arb));

    cout << "OK!\n\n";
}

/*
 * Caso 3: Múltiplos ciclos aninhados (caso clássico do artigo)
 */
void test_multiple_cycles() {
    cout << "[TESTE 3] Vários ciclos aninhados...\n";

    WeightedGraph g(6, true);

    g.insertEdge(0, 1, 3.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 0, 4.0);

    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 4, 1.0);
    g.insertEdge(4, 2, 1.0);

    g.insertEdge(4, 5, 2.0);

    GabowMST mst(g);
    auto arb = mst.compute(0);

    printARB(arb);

    assert(isValidArborescence(6, 0, arb));

    cout << "OK!\n\n";
}

/*
 * Caso 4: Pesos zero (caso crítico em GGST)
 */
void test_zero_weights() {
    cout << "[TESTE 4] Arestas com peso zero...\n";

    WeightedGraph g(5, true);

    g.insertEdge(0, 1, 0.0);
    g.insertEdge(1, 2, 0.0);
    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 4, 0.0);
    g.insertEdge(4, 1, 0.0);

    GabowMST mst(g);
    auto arb = mst.compute(0);

    printARB(arb);

    assert(isValidArborescence(5, 0, arb));

    cout << "OK!\n\n";
}

/*
 * Caso 5: Grafo grande aleatório
 */
void test_large_graph() {
    cout << "[TESTE 5] Grafo grande randomizado...\n";

    int n = 300;
    WeightedGraph g(n, true);

    for (int i = 0; i < n * 5; i++) {
        int u = rand() % n;
        int v = rand() % n;
        if (u != v)
            g.insertEdge(u, v, (double)(rand() % 100));
    }

    GabowMST mst(g);
    auto arb = mst.compute(0);

    assert(isValidArborescence(n, 0, arb));

    cout << "OK!\n\n";
}

int main() {
    test_simple_no_cycle();
    test_simple_cycle();
    test_multiple_cycles();
    test_zero_weights();
    test_large_graph();

    cout << "\nTodos os testes passaram!\n";

    return 0;
}
