#include "GabowMST.h"
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

// =========================================================
// Estruturas Internas (Skew Heap & Cycle Info)
// =========================================================

struct GabowNode {
    double val;           // Peso da aresta (ajustado por lazy)
    double lazy;          // Valor para propagação preguiçosa
    int u, v;             // Aresta u -> v
    int idOriginal;       // Para recuperar a aresta real no final
    GabowNode *left, *right;

    GabowNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {}
};

struct ComponenteCiclo {
    int representante; 
    int edgeID;        
};

struct CicloInfo {
    int superNo; 
    vector<ComponenteCiclo> componentes; 
};

// =========================================================
// Classe Solver (Gerencia memória e estado do algoritmo)
// =========================================================

class GabowSolver {
private:
    vector<GabowNode*> nodePool; // Pool para limpar memória no destrutor
    vector<int> paiDSU;          // Union-Find dedicado

public:
    GabowSolver(int n) {
        paiDSU.resize(2 * n);
        for(int i = 0; i < 2 * n; ++i) paiDSU[i] = i;
    }

    ~GabowSolver() {
        // Limpa todos os nós alocados durante a execução
        for(auto ptr : nodePool) delete ptr;
    }

    // --- Métodos de Heap (Skew Heap) ---

    GabowNode* novoNo(double w, int u, int v, int id) {
        GabowNode* node = new GabowNode(w, u, v, id);
        nodePool.push_back(node);
        return node;
    }

    void push_lazy(GabowNode* t) {
        if (!t || t->lazy == 0) return;
        t->val += t->lazy;
        if (t->left) t->left->lazy += t->lazy;
        if (t->right) t->right->lazy += t->lazy;
        t->lazy = 0;
    }

    GabowNode* merge(GabowNode* a, GabowNode* b) {
        push_lazy(a);
        push_lazy(b);
        if (!a) return b;
        if (!b) return a;
        
        // Min-Heap: Menor valor no topo
        if (a->val > b->val) swap(a, b);
        
        swap(a->left, a->right);
        a->left = merge(b, a->left);
        return a;
    }

    GabowNode* push(GabowNode* root, double w, int u, int v, int id) {
        return merge(root, novoNo(w, u, v, id));
    }

    GabowNode* pop(GabowNode* root) {
        push_lazy(root);
        return merge(root->left, root->right);
    }
    
    GabowNode* top(GabowNode* root) {
        push_lazy(root);
        return root;
    }

    // --- Métodos de DSU (Específico para Gabow) ---
    // O novo super-nó DEVE ser o pai do conjunto. Não usamos Union-By-Rank.
    
    int find(int i) {
        if (paiDSU[i] == i) return i;
        return paiDSU[i] = find(paiDSU[i]); // Path compression
    }

    void unite(int filho, int pai) {
        int root_filho = find(filho);
        int root_pai = find(pai);
        if (root_filho != root_pai) {
            paiDSU[root_filho] = root_pai;
        }
    }
};

// =========================================================
// Implementação Principal
// =========================================================

WeightedGraph GabowMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    int V = grafo.V();
    
    // Armazena arestas reais para reconstrução final
    // O índice no vetor 'arestasReais' será o 'idOriginal' nas heaps
    vector<WeightedEdge> arestasReais; 
    
    // Instancia o Solver (gerencia memória e DSU)
    GabowSolver solver(V);

    // Vetor de Heaps (Filas de prioridade de entrada para cada nó)
    vector<GabowNode*> queues(2 * V, nullptr); 

    // 1. Inicialização: Converter Grafo -> Heaps de Arestas de Entrada
    int idCounter = 0;
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            // Ignora arestas que apontam para a raiz ou self-loops iniciais
            if (e.w != raiz && e.v != e.w) {
                // e.v = origem, e.w = destino
                // Adicionamos na heap do DESTINO (e.w)
                queues[e.w] = solver.push(queues[e.w], e.weight, e.v, e.w, idCounter);
                arestasReais.push_back(e);
                idCounter++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    // Estruturas de Estado para o Path Growing
    vector<int> estado(2 * V, 0); // 0: Novo, 1: Ativo (no caminho), 2: Processado
    vector<int> arestaEntradaEscolhida(2 * V, -1); 
    vector<int> paiNaHierarquia(2 * V, -1); // Para descompactar super-nós
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = V; // Contador para IDs de super-nós (começa em V)

    // 2. Loop Principal (Path Growing)
    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;
        
        int u = solver.find(i);
        if (estado[u] != 0) continue; // Já processado ou ativo

        int curr = u;
        while (estado[curr] != 2) {
            estado[curr] = 1; // Marca como Ativo (parte do caminho atual)

            // Limpa self-loops da heap (arestas internas do componente)
            GabowNode* minNode = queues[curr];
            while (minNode && solver.find(minNode->u) == curr) {
                queues[curr] = solver.pop(queues[curr]);
                minNode = queues[curr];
            }

            // Se a heap ficar vazia, o componente é inalcançável
            if (!minNode) {
                estado[curr] = 2; // Marca como processado (mas sem pai)
                break;
            }

            // Seleciona provisoriamente
            arestaEntradaEscolhida[curr] = minNode->idOriginal;
            int origem = solver.find(minNode->u);

            if (estado[origem] == 1) {
                // === CICLO DETECTADO (Aresta aponta para nó Ativo) ===
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;
                
                GabowNode* heapUniao = nullptr;

                // Percorre o ciclo no caminho ativo até encontrar a origem
                int iter = curr;
                while (iter != origem) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;

                    // Merge Heap com Lazy Update
                    // Subtrai o peso da aresta escolhida de todas as outras na heap
                    GabowNode* h = queues[iter];
                    if (h) h->lazy -= arestasReais[edgeId].weight;
                    heapUniao = solver.merge(heapUniao, h);

                    solver.unite(iter, novoSuperNo); // iter vira filho de novoSuperNo
                    
                    // Retrocede no ciclo
                    iter = solver.find(arestasReais[edgeId].v); // .v é origem
                }

                // Trata o nó de fechamento (origem)
                int edgeIdOrigem = minNode->idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;

                GabowNode* h = queues[origem];
                if (h) h->lazy -= arestasReais[edgeIdOrigem].weight;
                heapUniao = solver.merge(heapUniao, h);
                
                solver.unite(origem, novoSuperNo);

                // Finaliza criação do Super-Nó
                queues[novoSuperNo] = heapUniao;
                estado[novoSuperNo] = 1; // O novo super-nó agora é Ativo (topo do caminho)
                
                pilhaCiclos.push(ciclo);
                curr = novoSuperNo; // Continua crescendo a partir do super-nó

            } else {
                // Não é ciclo, apenas estende o caminho
                curr = origem;
            }
        }

        // Caminho terminou (chegou na raiz ou em nó processado).
        // Marca todo o caminho percorrido como Processado (2).
        int temp = u;
        while (temp != -1 && estado[temp] == 1) {
            estado[temp] = 2;
            if (arestaEntradaEscolhida[temp] == -1) break;
            
            // Avança para o pai lógico (origem da aresta de entrada)
            int parent = solver.find(arestasReais[arestaEntradaEscolhida[temp]].v);
            if (parent == temp) break; // Evita loop infinito se algo falhar
            temp = parent;
        }
    }

    // 3. Fase de Expansão (Desfazendo os Super-Nós)
    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();

        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];
        
        int subComponenteEntrada = -1;

        // Se o super-nó tem uma entrada externa, descobrimos quem recebe ela
        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = arestasReais[arestaQueEntraNoSuperNo].w; // Destino
            
            // Sobe na hierarquia a partir do destinoReal até achar o filho imediato deste super-nó
            int temp = destinoReal;
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }

        // Distribui as arestas
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                // Este componente recebe a aresta externa (quebra o ciclo)
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                // Os outros mantêm as arestas internas do ciclo
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // 4. Construção do Grafo Final
    WeightedGraph resultado(V, true);
    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;
        int edgeID = arestaEntradaEscolhida[i];
        if (edgeID != -1) {
            const auto& aresta = arestasReais[edgeID];
            resultado.insertEdge(aresta.v, aresta.w, aresta.weight);
        }
    }

    return resultado;
}