#include "GabowMST.h"
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

// =========================================================
// Estruturas Internas
// =========================================================

struct HeapNodeGabow {
    double valor;           // Peso (chave)
    double lazy;            // Propagação preguiçosa
    int u, v;               // Aresta
    int idOriginal;         // ID para reconstrução
    HeapNodeGabow *esq, *dir;

    HeapNodeGabow(double w, int _u, int _v, int _id) 
        : valor(w), lazy(0), u(_u), v(_v), idOriginal(_id), esq(nullptr), dir(nullptr) {}
};

struct InfoCiclo {
    int idSuperNo;
    vector<int> componentes;      // Nós (ou super-nós) que formam este ciclo
    vector<int> arestasInternas;  // Arestas que conectam o ciclo
};

// Classe Solver para encapsular o estado e memória do algoritmo
class GabowSolver {
private:
    vector<HeapNodeGabow*> poolMemoria; // Gerenciamento RAII
    vector<HeapNodeGabow*> filas;       // Heaps por vértice
    vector<int> paiDSU;                 // Union-Find simples

public:
    GabowSolver(int n) {
        filas.resize(2 * n, nullptr);
        paiDSU.resize(2 * n);
        for(int i=0; i < 2*n; ++i) paiDSU[i] = i;
    }

    ~GabowSolver() {
        for(auto ptr : poolMemoria) delete ptr;
    }

    // --- Lógica de Skew Heap ---

    void pushLazy(HeapNodeGabow* node) {
        if (!node || node->lazy == 0) return;
        node->valor += node->lazy;
        if (node->esq) node->esq->lazy += node->lazy;
        if (node->dir) node->dir->lazy += node->lazy;
        node->lazy = 0;
    }

    HeapNodeGabow* merge(HeapNodeGabow* a, HeapNodeGabow* b) {
        pushLazy(a);
        pushLazy(b);
        if (!a) return b;
        if (!b) return a;
        
        if (a->valor > b->valor) swap(a, b);
        
        swap(a->esq, a->dir);
        a->esq = merge(b, a->esq);
        return a;
    }

    void addNode(int vertice, double w, int u, int v, int id) {
        HeapNodeGabow* novo = new HeapNodeGabow(w, u, v, id);
        poolMemoria.push_back(novo);
        filas[vertice] = merge(filas[vertice], novo);
    }

    HeapNodeGabow* pop(int vertice) {
        HeapNodeGabow* root = filas[vertice];
        pushLazy(root);
        filas[vertice] = merge(root->esq, root->dir);
        return root;
    }

    HeapNodeGabow* top(int vertice) {
        pushLazy(filas[vertice]);
        return filas[vertice];
    }

    void aplicarDesconto(int vertice, double val) {
        if (filas[vertice]) filas[vertice]->lazy -= val;
    }

    void fundirHeaps(int destino, int origem) {
        filas[destino] = merge(filas[destino], filas[origem]);
    }

    // --- Lógica de DSU ---
    // Nota: Gabow exige que o representante seja explicitamente o novo super-nó
    int find(int i) {
        if (paiDSU[i] == i) return i;
        return paiDSU[i] = find(paiDSU[i]);
    }

    void unite(int i, int alvo) {
        int root_i = find(i);
        int root_alvo = find(alvo);
        if (root_i != root_alvo) {
            paiDSU[root_i] = root_alvo; // Alvo sempre vira pai
        }
    }
};

// =========================================================
// Algoritmo Principal
// =========================================================

WeightedGraph GabowMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    int V = grafo.V();
    vector<WeightedEdge> arestasReais; // Mapeamento ID -> Aresta
    int contadorID = 0;

    GabowSolver solver(V);

    // 1. Inicializa Heaps (apenas arestas válidas)
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.w != raiz && e.v != e.w) {
                // e.w é o destino. Adicionamos na heap do destino.
                solver.addNode(e.w, e.weight, e.v, e.w, contadorID);
                arestasReais.push_back(e);
                contadorID++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    // Estruturas de Estado
    int contagemNos = V;
    vector<int> estado(2 * V, 0); // 0: Novo, 1: Ativo (Path), 2: Concluído
    vector<int> arestaEntrada(2 * V, -1);
    vector<int> hierarquia(2 * V, -1); // Quem contém quem
    stack<InfoCiclo> historico;

    // 2. Loop principal (Path Growing)
    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;

        int u = solver.find(i);
        if (estado[u] != 0) continue; // Já processado ou ativo em outro caminho

        int atual = u;
        while (estado[atual] != 2) {
            estado[atual] = 1; // Marca como parte do caminho atual (Ativo)

            // Obtém melhor aresta de entrada
            HeapNodeGabow* minEdge = solver.top(atual);
            
            // Limpa arestas internas (self-loops do componente)
            while (minEdge && solver.find(minEdge->u) == atual) {
                solver.pop(atual);
                minEdge = solver.top(atual);
            }

            if (!minEdge) {
                // Caminho terminou sem conexão (inalcançável)
                estado[atual] = 2; 
                break;
            }

            // Tentativa de conexão
            arestaEntrada[atual] = minEdge->idOriginal;
            int origem = solver.find(minEdge->u);

            if (estado[origem] == 1) {
                // === CICLO ENCONTRADO (origem já está ativa no caminho) ===
                int novoSuperNo = contagemNos++;
                InfoCiclo info;
                info.idSuperNo = novoSuperNo;

                // Percorre o ciclo no caminho ativo para fundir
                int iter = atual;
                while (iter != origem) {
                    info.componentes.push_back(iter);
                    int idAresta = arestaEntrada[iter];
                    info.arestasInternas.push_back(idAresta);
                    
                    hierarquia[iter] = novoSuperNo;
                    
                    // Merge Heap & Lazy Update
                    double pesoAresta = arestasReais[idAresta].weight;
                    solver.aplicarDesconto(iter, pesoAresta);
                    solver.fundirHeaps(novoSuperNo, iter);
                    solver.unite(iter, novoSuperNo);

                    // Retrocede no caminho
                    iter = solver.find(arestasReais[idAresta].v); // v é origem na struct
                }

                // Processa o nó de fechamento (origem)
                info.componentes.push_back(origem);
                int idArestaOrigem = minEdge->idOriginal; // Aresta que fechou o ciclo
                info.arestasInternas.push_back(idArestaOrigem);
                
                hierarquia[origem] = novoSuperNo;
                
                double pesoFechamento = arestasReais[idArestaOrigem].weight;
                solver.aplicarDesconto(origem, pesoFechamento);
                solver.fundirHeaps(novoSuperNo, origem);
                solver.unite(origem, novoSuperNo);

                historico.push(info);
                
                // O novo super-nó agora é o topo do caminho ativo
                atual = novoSuperNo;
                estado[atual] = 1;

            } else {
                // Não é ciclo, apenas estende o caminho
                atual = origem;
            }
        }

        // Caminho finalizou (chegou na raiz ou em nó processado)
        // Marca todo o caminho percorrido como Processado (2)
        int varredura = u;
        while (estado[varredura] == 1) {
            estado[varredura] = 2;
            if (varredura == raiz || arestaEntrada[varredura] == -1) break;
            
            // Avança para o pai (cuidado com super-nós)
            int pai = solver.find(arestasReais[arestaEntrada[varredura]].v);
            if (pai == varredura) break; // Segurança
            varredura = pai;
        }
    }

    // 3. Expansão (Desfazendo Super-Nós)
    while (!historico.empty()) {
        InfoCiclo ciclo = historico.top();
        historico.pop();

        int superNo = ciclo.idSuperNo;
        int idEntradaExterna = arestaEntrada[superNo];
        
        int componenteEntrada = -1;

        if (idEntradaExterna != -1) {
            int destinoReal = arestasReais[idEntradaExterna].w; // w é destino
            
            // Busca hierárquica para achar qual componente do ciclo contém o destinoReal
            int temp = destinoReal;
            bool achou = false;
            while(temp != -1) {
                 for(int comp : ciclo.componentes) {
                     if (comp == temp) {
                         componenteEntrada = comp;
                         achou = true;
                         break;
                     }
                 }
                 if(achou) break;
                 if(hierarquia[temp] == -1) break;
                 temp = hierarquia[temp];
            }
        }

        // Restaura arestas
        for (size_t k = 0; k < ciclo.componentes.size(); ++k) {
            int comp = ciclo.componentes[k];
            if (comp == componenteEntrada) {
                arestaEntrada[comp] = idEntradaExterna;
            } else {
                arestaEntrada[comp] = ciclo.arestasInternas[k];
            }
        }
    }

    // 4. Construção Final
    WeightedGraph resultado(V, true);
    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;
        int id = arestaEntrada[i];
        if (id != -1) {
            const auto& a = arestasReais[id];
            resultado.insertEdge(a.v, a.w, a.weight);
        }
    }

    return resultado;
}