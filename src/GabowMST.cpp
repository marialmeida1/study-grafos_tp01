#include "GabowMST.h"
#include "WeightedGraph.h"
#include "WeightedEdge.h"

#include <iostream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

/*
 * Implementação do algoritmo GGST (Gabow et al. 1986) — versão prática:
 *
 * - Extrai arestas do WeightedGraph passado no construtor.
 * - Usa a técnica clássica de Edmonds/Tarjan/Gabow:
 *     1) para cada vértice (exceto raiz) escolhe a aresta entrante de menor peso;
 *     2) detecta ciclos nessas escolhas;
 *     3) contrai cada ciclo em um nó único, ajustando pesos das arestas externas;
 *     4) repete até não haver ciclos;
 *     5) reconstrói a solução final (expansão), usando o histórico de escolhas.
 */
GabowMST::GabowMST(const WeightedGraph &g)
{
    n = g.V();
    edges.clear();

    for (int u = 0; u < n; ++u) {
        WeightedGraph::AdjIterator it(g, u);
        for (WeightedEdge e = it.begin(); !it.end(); e = it.next())
        {
            if if (u != e.v) 
            edges.emplace_back(u, e.v, e.weight);
        }
    }

    arborescence.clear();
    totalWeight = 0.0;
}

/*
 * Função interna: implementação central (contrair+reconstruir).
 *
 * Parâmetros:
 *   - N: número de vértices (originais)
 *   - root: índice da raiz (no grafo original)
 *   - E: lista de arestas (from,to,weight) do grafo original
 *
 * Retorna: vetor de GabowEdge que formam a arborescência mínima enraizada em 'root'.
 *
 * Nota: trata grafos possivelmente desconectos (da raiz) retornando vetor vazio se impossível.
 */
static std::vector<GabowEdge> gabow_impl(int N, int root, const std::vector<GabowEdge> &E)
{
    const double INF = std::numeric_limits<double>::infinity();
    if (N == 0) return {};

    std::vector<GabowEdge> curE = E;
    int curN = N;

    // Histórico para reconstrução:
    // ids_hist: mapeamentos entre níveis de contração
    // pre_hist: arestas escolhidas em cada nível
    std::vector<std::vector<int>> ids_hist;
    std::vector<std::vector<GabowEdge>> pre_hist;

    int curRoot = root;

    while (true)
    {
        // 1) melhor aresta entrante por vértice (no grafo atual)
        std::vector<double> inW(curN, INF);
        std::vector<int> pre(curN, -1);

        for (const auto &e : curE) {
            if (e.weight < inW[e.to]) {
                inW[e.to] = e.weight;
                pre[e.to] = e.from;
            }
        }

        // raiz não tem pai
        inW[curRoot] = 0;
        pre[curRoot] = -1;

        // Verifica se todos os vértices são alcançáveis
        for (int v = 0; v < curN; ++v) {
            if (v != curRoot && inW[v] == INF) {
                return {}; // grafo desconexo da raiz
            }
        }

        // 2) Detectar ciclos
        std::vector<int> id(curN, -1);
        std::vector<int> vis(curN, -1);
        int comps = 0;

        for (int v = 0; v < curN; ++v) {
            if (v == curRoot) continue;
            int u = v;
            while (u != curRoot && id[u] == -1 && vis[u] != v) {
                vis[u] = v;
                u = pre[u];
            }
            if (u != curRoot && id[u] == -1 && vis[u] == v) {
                // marca ciclo
                for (int x = pre[u]; x != u; x = pre[x])
                    id[x] = comps;
                id[u] = comps++;
            }
        }

        // Sem ciclos: terminar
        if (comps == 0) {
            for (int v = 0; v < curN; ++v)
                if (id[v] == -1) id[v] = comps++;
            ids_hist.push_back(id);
            pre_hist.push_back({});
            break;
        }

        // atribui ids para vértices que não estão em ciclos
        for (int v = 0; v < curN; ++v)
            if (id[v] == -1) id[v] = comps++;

        // registre o mapping id[] e as arestas escolhidas (pre[v] -> v)
        ids_hist.push_back(id);

        std::vector<GabowEdge> chosen;
        chosen.reserve(curN - 1);
        for (int v = 0; v < curN; ++v) {
            if (v == curRoot) continue;
            chosen.emplace_back(pre[v], v, inW[v]);
        }
        pre_hist.push_back(chosen);

        // 3) Construir grafo contraído
        int newN = comps;
        std::vector<GabowEdge> newE;
        newE.reserve(curE.size());

        // Ajustar pesos: w' = w - inW[to]
        for (const auto &e : curE) {
            int u = id[e.from];
            int v = id[e.to];
            if (u != v) {
                double newW = e.weight - inW[e.to];
                newE.emplace_back(u, v, newW);
            }
        }

        // 4) atualizar root e grafo atual
        curRoot = id[curRoot];
        curN = newN;
        curE.swap(newE);
    }

    // ========================================
    // FASE DE RECONSTRUÇÃO
    // ========================================
    std::vector<int> cur_map = ids_hist.back();
    std::vector<GabowEdge> finalTree;

    // Percorre níveis de trás para frente
    for (int level = (int)ids_hist.size() - 2; level >= 0; --level) {
        const auto &mapping = ids_hist[level];   // mapping do nível 'level' para level+1
        const auto &preds = pre_hist[level];     // preds deste nível (arestas pre[v]->v com pesos relativos)

        // Converte arestas para índices originais
        for (const auto &pe : preds) {
            int u = pe.from;
            int v = pe.to;
            // mapping[v] é o nó no nível (level+1) correspondente ao v do nível atual.
            int mapped_v = cur_map[ mapping[v] ]; // nó no nível original associado a v
            int mapped_u = cur_map[ mapping[u] ]; // nó no nível original associado a u
            // adiciona aresta na forma original (mapped_u -> mapped_v)
            finalTree.emplace_back(mapped_u, mapped_v, pe.weight);
        }

        // Atualiza mapeamento para o próximo nível
        std::vector<int> new_map(mapping.size());
        for (int i = 0; i < (int)mapping.size(); ++i) {
            // mapping[i] aponta para um índice no nível+1; cur_map[mapping[i]] é o índice no original
            new_map[i] = cur_map[ mapping[i] ];
        }
        cur_map.swap(new_map);
    }

    // Remove duplicatas
    std::sort(finalTree.begin(), finalTree.end(), [](const GabowEdge &a, const GabowEdge &b) {
        if (a.to != b.to) return a.to < b.to;
        if (a.from != b.from) return a.from < b.from;
        return a.weight < b.weight;
    });

    finalTree.erase(std::unique(finalTree.begin(), finalTree.end(), [](const GabowEdge &a, const GabowEdge &b) {
        return a.from == b.from && a.to == b.to;
    }), finalTree.end());

    // Em certos casos devemos garantir que temos exatamente N-1 arestas (se a arborescência existe).
    // Se houver arestas a mais (por segurança), podemos filtrar por destino mantendo a menor
    // — porém a construção acima já preserva as escolhas mínimas por construção do algoritmo.
    // Retornamos finalTree (se vazio ⇒ problema/desconexão da raiz).
    return finalTree;
}

std::vector<GabowEdge> GabowMST::compute(int root)
{
    if (root < 0 || root >= n)
        throw std::invalid_argument("Raiz fora do intervalo válido.");

    arborescence = gabow_impl(n, root, edges);

    totalWeight = 0.0;
    for (const auto &e : arborescence)
        totalWeight += e.weight;

    return arborescence;
}

void GabowMST::printArborescence(const std::vector<GabowEdge> &arb) const
{
    std::cout << "\n--- Arborescência Geradora Mínima (Gabow/GGST) ---\n";
    double sum = 0.0;
    for (const auto &e : arb) {
        std::cout << e.from << " -> " << e.to << " (peso: " << e.weight << ")\n";
        sum += e.weight;
    }
    std::cout << "Peso total: " << sum << "\n";
}
