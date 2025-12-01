#include "GabowMST.h"
#include "WeightedGraph.h"

#include <vector>
#include <limits>
#include <stack>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace {

// ---------------------------
// Nó do Skew Heap
// ---------------------------
struct GabowNode {
    double val;       // peso da aresta (ajustado)
    double lazy;      // valor lazy
    int u, v;         // origem e destino
    int idOriginal;   // índice da aresta original em todasArestas
    GabowNode* left;
    GabowNode* right;

    GabowNode(double w, int _u, int _v, int _id)
        : val(w), lazy(0.0), u(_u), v(_v),
          idOriginal(_id), left(nullptr), right(nullptr) {}
};

// Gerenciador de memória simples (pool)
std::vector<GabowNode*> nodePool;

GabowNode* novoNo(double w, int u, int v, int id) {
    GabowNode* node = new GabowNode(w, u, v, id);
    nodePool.push_back(node);
    return node;
}

// Propaga valor lazy
void gabow_push_lazy(GabowNode* t) {
    if (!t || t->lazy == 0.0) return;
    t->val += t->lazy;
    if (t->left)  t->left->lazy  += t->lazy;
    if (t->right) t->right->lazy += t->lazy;
    t->lazy = 0.0;
}

// Fusão de dois Skew Heaps
GabowNode* gabow_merge(GabowNode* a, GabowNode* b) {
    gabow_push_lazy(a);
    gabow_push_lazy(b);
    if (!a) return b;
    if (!b) return a;
    if (a->val > b->val) std::swap(a, b);
    std::swap(a->left, a->right);
    a->left = gabow_merge(b, a->left);
    return a;
}

GabowNode* gabow_push(GabowNode* root, double w, int u, int v, int id) {
    return gabow_merge(root, novoNo(w, u, v, id));
}

GabowNode* gabow_pop(GabowNode* root) {
    gabow_push_lazy(root);
    return gabow_merge(root->left, root->right);
}

// ---------------------------
// DSU específico (sem union by rank)
// ---------------------------
struct DSU {
    std::vector<int> pai;

    explicit DSU(int n) : pai(n) {
        for (int i = 0; i < n; ++i) pai[i] = i;
    }

    int find(int i) {
        return (pai[i] == i) ? i : (pai[i] = find(pai[i]));
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            pai[root_i] = root_j; // o j (ou quem j aponta) vira o "pai"
        }
    }
};

// ---------------------------
// Estruturas para reconstrução
// ---------------------------
struct ComponenteCiclo {
    int representante; // vértice/componente
    int edgeID;        // índice da aresta de entrada desse componente
};

struct CicloInfo {
    int superNo; // índice do novo super-nó
    std::vector<ComponenteCiclo> componentes;
};

// ---------------------------
// Algoritmo principal (adaptado)
// Todas as arestas em todasArestas: GabowEdge {from, to, weight}
// ---------------------------
std::vector<GabowEdge> gabow_run(
    int n,
    int raiz,
    const std::vector<GabowEdge>& todasArestas
) {
    const double INF = std::numeric_limits<double>::infinity();

    if (n == 0) return {};

    // Limpa memória de execuções anteriores
    for (auto p : nodePool) delete p;
    nodePool.clear();

    // Inicialização das heaps (até 2n possíveis super-nós)
    std::vector<GabowNode*> queues(2 * n, nullptr);

    // Monta heaps de arestas entrantes (por destino)
    for (std::size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.to == raiz || aresta.from == aresta.to) continue;
        queues[aresta.to] = gabow_push(
            queues[aresta.to],
            aresta.weight,
            aresta.from,
            aresta.to,
            static_cast<int>(i)
        );
    }

    DSU dsu(2 * n);
    std::vector<int> estado(2 * n, 0);              // 0: novo, 1: ativo, 2: processado
    std::vector<int> arestaEntradaEscolhida(2 * n, -1);
    std::vector<int> paiNaHierarquia(2 * n, -1);
    std::stack<CicloInfo> pilhaCiclos;

    int numComponentes = n; // novos super-nós virão a partir daqui

    // ----------------------------------
    // Fase de Contração (Path Growing)
    // ----------------------------------
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;

        int u = dsu.find(i);
        if (estado[u] != 0) continue;

        int curr = u;
        while (estado[curr] != 2) {
            estado[curr] = 1; // ativo

            // Remove auto-loops
            GabowNode* minNode = queues[curr];
            while (minNode && dsu.find(minNode->u) == curr) {
                queues[curr] = gabow_pop(queues[curr]);
                minNode = queues[curr];
            }

            if (!minNode) {
                estado[curr] = 2; // não há arestas entrantes válidas -> processado
                break;
            }

            // Seleciona provisoriamente esta aresta como entrante do componente curr
            arestaEntradaEscolhida[curr] = minNode->idOriginal;
            int origem = dsu.find(minNode->u);

            if (estado[origem] == 1) {
                // -----------------------------
                // Ciclo detectado
                // -----------------------------
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;

                GabowNode* heapUniao = nullptr;

                // Funde componentes do ciclo
                int iter = curr;
                while (iter != origem) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;

                    GabowNode* h = queues[iter];
                    if (h) h->lazy -= todasArestas[edgeId].weight;
                    heapUniao = gabow_merge(heapUniao, h);

                    dsu.unite(iter, novoSuperNo);
                    iter = dsu.find(todasArestas[edgeId].from);
                }

                // Trata o nó de fechamento do ciclo
                int edgeIdOrigem = minNode->idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;

                GabowNode* h = queues[origem];
                if (h) h->lazy -= todasArestas[edgeIdOrigem].weight;
                heapUniao = gabow_merge(heapUniao, h);
                dsu.unite(origem, novoSuperNo);

                pilhaCiclos.push(ciclo);
                queues[novoSuperNo] = heapUniao;
                estado[novoSuperNo] = 1; // ativo

                curr = novoSuperNo;

            } else {
                // Caminha pela árvore de predecessores
                curr = origem;
            }
        }

        // Marca caminho como processado
        int temp = u;
        while (temp != -1 && estado[temp] == 1) {
            estado[temp] = 2;
            if (temp == raiz || arestaEntradaEscolhida[temp] == -1) break;

            int parent = dsu.find(todasArestas[arestaEntradaEscolhida[temp]].from);
            if (parent == temp) break;
            temp = parent;
        }
    }

    // ----------------------------------
    // Fase de Expansão
    // ----------------------------------
    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();

        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];

        int subComponenteEntrada = -1;

        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = todasArestas[arestaQueEntraNoSuperNo].to;

            int temp = destinoReal;
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }

        // Resolve arestas internas do ciclo
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // ----------------------------------
    // Monta o resultado como lista de arestas (GabowEdge)
    // ----------------------------------
    std::vector<GabowEdge> resultado;
    resultado.reserve(n - 1);

    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;
        int edgeID = arestaEntradaEscolhida[i];
        if (edgeID != -1) {
            const auto& aresta = todasArestas[edgeID];
            resultado.emplace_back(aresta.from, aresta.to, aresta.weight);
        }
    }

    // Libera memória dos nós do heap
    for (auto p : nodePool) delete p;
    nodePool.clear();

    return resultado;
}

} // namespace

// -----------------------------------------------------------------------------
// Implementação da classe GabowMST
// -----------------------------------------------------------------------------

GabowMST::GabowMST(const WeightedGraph& g)
    : n(g.V())
{
    edges.clear();
    edges.reserve(n * n);

    // Extrai todas as arestas do grafo (similar à EdmondsMST)
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (u == v) continue;
            if (g.hasEdge(u, v)) {
                double w = g.getWeight(u, v);
                edges.emplace_back(u, v, w);
            }
        }
    }

    arborescence.clear();
    totalWeight = 0.0;
}

std::vector<GabowEdge> GabowMST::compute(int root)
{
    if (root < 0 || root >= n) {
        throw std::invalid_argument("Raiz fora do intervalo válido em GabowMST::compute.");
    }

    arborescence = gabow_run(n, root, edges);

    totalWeight = 0.0;
    for (const auto& e : arborescence) {
        totalWeight += e.weight;
    }

    return arborescence;
}

void GabowMST::printArborescence(const std::vector<GabowEdge>& arb) const
{
    std::cout << "\n--- Arborescência Geradora Mínima (Gabow / GGST) ---\n";
    double sum = 0.0;
    for (const auto& e : arb) {
        std::cout << e.from << " -> " << e.to
                  << " (peso: " << e.weight << ")\n";
        sum += e.weight;
    }
    std::cout << "Peso total: " << sum << "\n";
}
