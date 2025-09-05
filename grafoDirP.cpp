#include "grafoDirP.h"
#include <iostream>

/* Construtor do grafo ponderado direcionado
   Recebe o número de vértices (V) e inicializa:
 - BaseGrafo(V, true) → grafo com V vértices e marcado como dirigido
 - adj(V) → vetor de listas de adjacência, cada posição representa as arestas de saída de um vértice*/
GrafoPonderado::GrafoPonderado(int V)
    : BaseGrafo(V, true), adj(V) {}  


// Insere uma aresta direcionada de v -> w com peso informado
// Só insere se a aresta ainda não existir, para evitar duplicatas    
void GrafoPonderado::inserirAresta(int v, int w, double peso) {
    if (!existeAresta(v, w)) {
        adj[v].push_back(ArestaP(v, w, peso));
        numA++;
    }
}

// Remove a aresta v -> w, caso exista
// Percorre a lista de adjacência de v, encontra o destino w e apaga
void GrafoPonderado::removerAresta(int v, int w) {
    for (auto it = adj[v].begin(); it != adj[v].end(); ++it) {
        if (it->w == w) {
            adj[v].erase(it);
            numA--;
            break;
        }
    }
}

// Atualiza o peso de uma aresta existente v -> w
// Retorna true se conseguiu atualizar, false se a aresta não existe
bool GrafoPonderado::atualizarPeso(int v, int w, double novoPeso) {
    for (auto& aresta : adj[v]) {
        if (aresta.w == w) {
            aresta.peso = novoPeso;
            return true;
        }
    }
    return false; // não encontrou a aresta
}

// Verifica se uma aresta v -> w existe no grafo
// Percorre a lista de adjacência de v procurando destino w
bool GrafoPonderado::existeAresta(int v, int w) const {
    for (const auto& aresta : adj[v]) {
        if (aresta.w == w) return true;
    }
    return false;
}

// Retorna o peso de uma aresta v -> w
// Caso a aresta não exista, retorna -1 como valor "sentinela"
double GrafoPonderado::getPeso(int v, int w) const {
    for(const auto& aresta : adj[v]){
        if(aresta.w == w){
            return aresta.peso;
        }
    }
    return -1;
}

// --- Iterator para grafos ponderados (funciona pra grafos não direcionais)
GrafoPonderado::adjIterator::adjIterator(const GrafoPonderado& G, int v)
    : G(G), v(v), index(0) {}

ArestaP GrafoPonderado::adjIterator::begin() {
    index = 0;
    return next();
}

ArestaP GrafoPonderado::adjIterator::next() {
    if (index < (int)G.adj[v].size())
        return G.adj[v][index++];
    else
        return ArestaP(-1, -1, -1);
}

bool GrafoPonderado::adjIterator::end() {
    return index >= (int)G.adj[v].size();
}

// Testes
/*
int main() {
    GrafoPonderado gp(5); // 5 vértices dirigido e ponderados

    gp.inserirAresta(0, 1, 2.5);
    gp.inserirAresta(1, 2, 3.1);
    gp.inserirAresta(2, 4, 1.8);


    std::cout << "Vertices: " << gp.V() << std::endl;
    std::cout << "Arestas: " << gp.A() << std::endl;

    gp.atualizarPeso(1, 2, 10.0);
    gp.removerAresta(2, 4);

    
    std::cout << "Arestas depois da remocao: " << gp.A() << std::endl;
    std::cout << "Existe 1->2? " << (gp.existeAresta(1, 2) ? "Sim" : "Nao") << std::endl;
    std::cout << "Existe 2->4? " << (gp.existeAresta(2, 4) ? "Sim" : "Nao") << std::endl;
    std::cout << "Peso de 1->2 " << (gp.getPeso(1,2)) << std::endl;

    return 0;
}*/

