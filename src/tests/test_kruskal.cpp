#include <iostream>
#include <cassert>
#include "WeightedGraph.h"
#include "KruskalMST.h"
#include <functional> // <-- para std::function
#include <cmath>    
#include <numeric> 

using namespace std;

// verifica se n-1 arestas e sem ciclos: válido para MST
bool isValidMST(int n, const vector<KEdge>& mst) {
    if ((int)mst.size() != n - 1) return false;

    vector<int> parent(n);
    iota(parent.begin(), parent.end(), 0);

    function<int(int)> find = [&](int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    };

    for (auto &e : mst) {
        int a = find(e.u);
        int b = find(e.v);
        if (a == b) return false;  // ciclo
        parent[b] = a;
    }
    return true;
}

void test_basic() {
    cout << "[TESTE 1] Grafo simples...\n";

    WeightedGraph g(5, false);
    g.insertEdge(0,1,1.0);
    g.insertEdge(0,2,3.0);
    g.insertEdge(1,2,1.0);
    g.insertEdge(1,3,4.0);
    g.insertEdge(2,3,1.0);
    g.insertEdge(3,4,2.0);

    KruskalMST mst(g);
    auto res = mst.compute();

    // DEBUG: mostrar resultado do MST
   double expected = 1.0 + 1.0 + 1.0 + 2.0; // menor árvore (movido para debug)
   std::cerr << "[DEBUG] res.size()=" << res.size() << "\n";
    for (const auto &e : res)
       std::cerr << "[DEBUG] MST edge: " << e.u << " - " << e.v << " (w=" << e.w << ")\n";
    std::cerr << "[DEBUG] mst.getTotalWeight()=" << mst.getTotalWeight() << " expected=" << expected << "\n";



    assert(isValidMST(5, res));

    double expected = 1.0 + 1.0 + 1.0 + 2.0; // menor árvore
    assert(fabs(mst.getTotalWeight() - expected) < 1e-9);

    cout << "OK!\n\n";
}

void test_disconnected() {
    cout << "[TESTE 2] Grafo desconexo...\n";

    WeightedGraph g(4, false);
    g.insertEdge(0,1,1.0);
    g.insertEdge(2,3,1.0);

    KruskalMST mst(g);
    auto res = mst.compute();

    // MST incompleta
    assert(res.size() != 3);

    cout << "OK!\n\n";
}

int main() {
    test_basic();
    test_disconnected();

    cout << "Todos os testes passaram!\n";
    return 0;
}
