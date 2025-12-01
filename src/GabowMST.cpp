#include "GabowMST.h"
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

struct GabowNode {
    double val;          
    double lazy;          
    int u, v;             
    int idOriginal;       
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

class GabowSolver {
private:
    vector<GabowNode*> nodePool; 
    vector<int> paiDSU;          

public:
    GabowSolver(int n) {
        paiDSU.resize(2 * n);
        for(int i = 0; i < 2 * n; ++i) paiDSU[i] = i;
    }

    ~GabowSolver() {
        for(auto ptr : nodePool) delete ptr;
    }

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

    int find(int i) {
        if (paiDSU[i] == i) return i;
        return paiDSU[i] = find(paiDSU[i]); 
    }

    void unite(int filho, int pai) {
        int root_filho = find(filho);
        int root_pai = find(pai);
        if (root_filho != root_pai) {
            paiDSU[root_filho] = root_pai;
        }
    }
};

WeightedGraph GabowMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    int V = grafo.V();
    
    vector<WeightedEdge> arestasReais; 
    
    GabowSolver solver(V);

    vector<GabowNode*> queues(2 * V, nullptr); 

    int idCounter = 0;
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.w != raiz && e.v != e.w) {
                queues[e.w] = solver.push(queues[e.w], e.weight, e.v, e.w, idCounter);
                arestasReais.push_back(e);
                idCounter++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    vector<int> estado(2 * V, 0);
    vector<int> arestaEntradaEscolhida(2 * V, -1); 
    vector<int> paiNaHierarquia(2 * V, -1); 
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = V; 
    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;
        
        int u = solver.find(i);
        if (estado[u] != 0) continue; 

        int curr = u;
        while (estado[curr] != 2) {
            estado[curr] = 1; 

            GabowNode* minNode = queues[curr];
            while (minNode && solver.find(minNode->u) == curr) {
                queues[curr] = solver.pop(queues[curr]);
                minNode = queues[curr];
            }

            if (!minNode) {
                estado[curr] = 2;
                break;
            }

            arestaEntradaEscolhida[curr] = minNode->idOriginal;
            int origem = solver.find(minNode->u);

            if (estado[origem] == 1) {
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;
                
                GabowNode* heapUniao = nullptr;

                int iter = curr;
                while (iter != origem) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;

                    GabowNode* h = queues[iter];
                    if (h) h->lazy -= arestasReais[edgeId].weight;
                    heapUniao = solver.merge(heapUniao, h);

                    solver.unite(iter, novoSuperNo); 
                    
                    // Retrocede no ciclo
                    iter = solver.find(arestasReais[edgeId].v); // .v é origem
                }

                int edgeIdOrigem = minNode->idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;

                GabowNode* h = queues[origem];
                if (h) h->lazy -= arestasReais[edgeIdOrigem].weight;
                heapUniao = solver.merge(heapUniao, h);
                
                solver.unite(origem, novoSuperNo);

                queues[novoSuperNo] = heapUniao;
                estado[novoSuperNo] = 1; 
                
                pilhaCiclos.push(ciclo);
                curr = novoSuperNo; 

            } else {
                curr = origem;
            }
        }

        int temp = u;
        while (temp != -1 && estado[temp] == 1) {
            estado[temp] = 2;
            if (arestaEntradaEscolhida[temp] == -1) break;
            
            int parent = solver.find(arestasReais[arestaEntradaEscolhida[temp]].v);
            if (parent == temp) break;
            temp = parent;
        }
    }

    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();

        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];
        
        int subComponenteEntrada = -1;

        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = arestasReais[arestaQueEntraNoSuperNo].w; 
            
            int temp = destinoReal;
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }

        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

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