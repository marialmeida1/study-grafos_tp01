#include "TarjanMST.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <stack>

using namespace std;

// =========================================================
// Estruturas Auxiliares
// =========================================================

struct SkewNode {
    double peso;            
    double propagacaoLazy;  
    int u, v;               
    int idOriginal;         
    SkewNode *esq, *dir;    

    SkewNode(double w, int _u, int _v, int _id) 
        : peso(w), propagacaoLazy(0), u(_u), v(_v), idOriginal(_id), esq(nullptr), dir(nullptr) {}
};

class TarjanSolver {
private:
    vector<SkewNode*> memoriaNodes; 
    vector<SkewNode*> filasPrioridade; 
    vector<int> paiDSU; 

public:
    TarjanSolver(int n) {
        filasPrioridade.resize(2 * n, nullptr); 
        paiDSU.resize(2 * n);
        for(int i = 0; i < 2 * n; ++i) paiDSU[i] = i;
    }

    ~TarjanSolver() {
        for (auto node : memoriaNodes) delete node;
    }

    void propagar(SkewNode* node) {
        if (!node || node->propagacaoLazy == 0) return;
        node->peso += node->propagacaoLazy;
        if (node->esq) node->esq->propagacaoLazy += node->propagacaoLazy;
        if (node->dir) node->dir->propagacaoLazy += node->propagacaoLazy;
        node->propagacaoLazy = 0;
    }

    SkewNode* merge(SkewNode* a, SkewNode* b) {
        propagar(a);
        propagar(b);
        if (!a) return b;
        if (!b) return a;
        if (a->peso > b->peso) swap(a, b);
        swap(a->esq, a->dir);
        a->esq = merge(b, a->esq);
        return a;
    }

    void push(int vertice, double w, int u, int v, int id) {
        SkewNode* novo = new SkewNode(w, u, v, id);
        memoriaNodes.push_back(novo);
        filasPrioridade[vertice] = merge(filasPrioridade[vertice], novo);
    }

    SkewNode* pop(int vertice) {
        SkewNode* root = filasPrioridade[vertice];
        propagar(root);
        filasPrioridade[vertice] = merge(root->esq, root->dir);
        return root;
    }

    SkewNode* top(int vertice) {
        propagar(filasPrioridade[vertice]);
        return filasPrioridade[vertice];
    }

    void addLazy(int vertice, double val) {
        if (filasPrioridade[vertice]) 
            filasPrioridade[vertice]->propagacaoLazy -= val;
    }

    void unirHeaps(int destino, int origem) {
        filasPrioridade[destino] = merge(filasPrioridade[destino], filasPrioridade[origem]);
    }

    int find(int i) {
        if (paiDSU[i] == i) return i;
        return paiDSU[i] = find(paiDSU[i]);
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) paiDSU[root_i] = root_j;
    }
};

struct EstadoCiclo {
    int idSuperNo;
    vector<int> nosNoCiclo;     
    vector<int> arestasEscolhidas; 
};

// =========================================================
// Implementação Principal
// =========================================================

WeightedGraph TarjanMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    int V = grafo.V();
    vector<WeightedEdge> arestasReais;
    int idCounter = 0;
    
    TarjanSolver solver(V);

    // Inicialização
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.w != raiz && e.v != e.w) { 
                arestasReais.push_back(e); 
                solver.push(e.w, e.weight, e.v, e.w, idCounter);
                idCounter++;
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    int numSuperVertices = V;
    vector<int> arestaEntrada(2 * V, -1); 
    vector<int> paiHierarquia(2 * V, -1); 
    stack<EstadoCiclo> historicoCiclos;   
    vector<int> caminhoVisitado(2 * V, -1);

    for (int i = 0; i < V; ++i) {
        if (i == raiz) continue;

        int curr = solver.find(i);
        
        while (arestaEntrada[curr] == -1 && solver.find(raiz) != curr) {
            SkewNode* minEdge = solver.top(curr);
            
            // Remove arestas internas
            while (minEdge && solver.find(minEdge->u) == curr) {
                solver.pop(curr);
                minEdge = solver.top(curr);
            }

            if (!minEdge) break; 

            arestaEntrada[curr] = minEdge->idOriginal;
            int u_origem = solver.find(minEdge->u);

            if (u_origem == curr) {
                // Auto-loop (já tratado pelo while acima, mas por segurança)
                continue;
            } 
            else if (caminhoVisitado[u_origem] == i) {
                // === CICLO DETECTADO ===
                int novoSuperNo = numSuperVertices++;
                EstadoCiclo infoCiclo;
                infoCiclo.idSuperNo = novoSuperNo;

                // PASSO 1: Identificar os nós do ciclo sem alterar o DSU ainda
                // Percorre para trás usando arestaEntrada até voltar ao 'curr'
                int iter = u_origem;
                while (iter != curr) {
                    infoCiclo.nosNoCiclo.push_back(iter);
                    int edgeId = arestaEntrada[iter];
                    iter = solver.find(arestasReais[edgeId].v); // Retrocede
                }
                // Adiciona o próprio curr (que fecha o ciclo)
                infoCiclo.nosNoCiclo.push_back(curr);
                
                // PASSO 2: Realizar fusão (Merge e Unite)
                for (int nodeDoCiclo : infoCiclo.nosNoCiclo) {
                    // Salva a aresta escolhida para este nó antes de fundir
                    infoCiclo.arestasEscolhidas.push_back(arestaEntrada[nodeDoCiclo]);
                    
                    // Atualiza Hierarquia
                    paiHierarquia[nodeDoCiclo] = novoSuperNo;

                    // Lazy Update e Merge de Heaps
                    double pesoArestaCiclo = arestasReais[arestaEntrada[nodeDoCiclo]].weight;
                    solver.addLazy(nodeDoCiclo, pesoArestaCiclo);
                    solver.unirHeaps(novoSuperNo, nodeDoCiclo);
                    
                    // Unir conjuntos no DSU
                    solver.unite(nodeDoCiclo, novoSuperNo);
                }

                historicoCiclos.push(infoCiclo);
                curr = novoSuperNo; 
                caminhoVisitado[curr] = i; 

            } else {
                caminhoVisitado[u_origem] = i; 
                curr = u_origem;
            }
        }
    }

    // Fase de Expansão
    while (!historicoCiclos.empty()) {
        EstadoCiclo ciclo = historicoCiclos.top();
        historicoCiclos.pop();

        int superNo = ciclo.idSuperNo;
        int arestaQueEntraNoSuperNo = arestaEntrada[superNo];

        int noEntradaReal = -1;
        if (arestaQueEntraNoSuperNo != -1) {
            int destino = arestasReais[arestaQueEntraNoSuperNo].w; 
            
            // Busca qual componente do ciclo é ancestral do destino
            // Isso lida corretamente com ciclos aninhados
            int ancestral = destino;
            bool achou = false;
            
            // Como paiHierarquia sobe na árvore de super-nós,
            // verificamos se algum ancestral do 'destino' pertence à lista deste ciclo.
            while (ancestral != -1) {
                // Verifica se 'ancestral' está na lista de nós deste ciclo
                for (int comp : ciclo.nosNoCiclo) {
                    if (comp == ancestral) {
                        noEntradaReal = comp;
                        achou = true;
                        break;
                    }
                }
                if (achou) break;
                if (paiHierarquia[ancestral] == -1) break;
                ancestral = paiHierarquia[ancestral];
            }
        }

        // Restaura arestas
        for (size_t k = 0; k < ciclo.nosNoCiclo.size(); ++k) {
            int componente = ciclo.nosNoCiclo[k];
            if (componente == noEntradaReal) {
                arestaEntrada[componente] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntrada[componente] = ciclo.arestasEscolhidas[k];
            }
        }
    }

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