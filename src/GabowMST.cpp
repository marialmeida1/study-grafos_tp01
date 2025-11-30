#include "GabowMST.h"
#include "WeightedGraph.h"
#include "WeightedEdge.h"

#include <iostream>
#include <algorithm>
#include <cmath>

/*
 * Algoritmo de Gabow et al. (GGST) para Arborescência Geradora Mínima.
 *
 * Ideia geral:
 *   1) Seleciona a menor aresta entrante para cada vértice.
 *   2) Detecta ciclos nessas escolhas.
 *   3) Contrai ciclos em um único nó e ajusta os pesos.
 *   4) Repete até não existirem ciclos.
 *   5) Reconstrói a solução final a partir das escolhas realizadas.
 *
 * Esta versão segue o método de Gabow de forma prática e clara,
 * mantendo compatibilidade com a estrutura do projeto.
 */

GabowMST::GabowMST(const WeightedGraph &g)
{
    n = g.V();
    edges.clear();

    // Extrai arestas do grafo.
    for (int u = 0; u < n; ++u)
    {
        WeightedGraph::AdjIterator it(g, u);
        for (WeightedEdge e = it.begin(); !it.end(); e = it.next())
        {
            if (u != e.v)
                edges.emplace_back(e.v, e.w, e.weight);
        }
    }

    totalWeight = 0.0;
}

/*
 * Função principal do algoritmo de Gabow.
 * Retorna as arestas da arborescência mínima.
 */
static std::vector<GabowEdge> gabow(int N, int root, const std::vector<GabowEdge> &E)
{
    const double INF = std::numeric_limits<double>::infinity();
    if (N == 0)
        return {};

    std::vector<GabowEdge> curE = E;
    int curN = N;

    // Histórico da reconstrução
    std::vector<std::vector<int>> ids_hist;          // mapeamento dos nós após cada contração
    std::vector<std::vector<GabowEdge>> pre_hist;    // arestas escolhidas em cada estágio

    while (true)
    {
        // 1) Melhor aresta entrante por vértice
        std::vector<double> inW(curN, INF);
        std::vector<int> pre(curN, -1);

        for (const auto &e : curE)
        {
            if (e.weight < inW[e.to])
            {
                inW[e.to] = e.weight;
                pre[e.to] = e.from;
            }
        }

        // raiz não tem pai
        inW[root] = 0;
        pre[root] = -1;

        // Algum vértice sem aresta entrante → impossível
        for (int v = 0; v < curN; ++v)
        {
            if (v != root && inW[v] == INF)
                return {};
        }

        // 2) Detectar ciclos
        std::vector<int> id(curN, -1), vis(curN, -1);
        int comps = 0;

        for (int v = 0; v < curN; ++v)
        {
            if (v == root)
                continue;

            int u = v;
            while (u != root && id[u] == -1 && vis[u] != v)
            {
                vis[u] = v;
                u = pre[u];
            }

            if (u != root && id[u] == -1 && vis[u] == v)
            {
                for (int x = pre[u]; x != u; x = pre[x])
                    id[x] = comps;
                id[u] = comps++;
            }
        }

        // 3) Se não houve ciclos → registrar último mapeamento e parar
        if (comps == 0)
        {
            for (int v = 0; v < curN; ++v)
                if (id[v] == -1)
                    id[v] = comps++;

            ids_hist.push_back(id);
            pre_hist.push_back({}); // último nível não tem preds próprios

            break; // termina a fase de contração
        }

        // Atribui ids aos vértices fora dos ciclos
        for (int v = 0; v < curN; ++v)
            if (id[v] == -1)
                id[v] = comps++;

        // Guardar os preds deste estágio
        std::vector<GabowEdge> chosen;
        chosen.reserve(curN - 1);
        for (int v = 0; v < curN; ++v)
            if (v != root)
                chosen.emplace_back(pre[v], v, inW[v]);

        ids_hist.push_back(id);
        pre_hist.push_back(chosen);

        // 4) Constrói grafo contraído
        int newN = comps;
        std::vector<GabowEdge> newE;
        newE.reserve(curE.size());

        for (const auto &e : curE)
        {
            int u = id[e.from];
            int v = id[e.to];
            if (u != v)
                newE.emplace_back(u, v, e.weight - inW[e.to]);
        }

        // Atualiza para próxima iteração
        root = id[root];
        curN = newN;
        curE.swap(newE);
    }

    // ===========================================
    //     FASE FINAL — RECONSTRUÇÃO DA MSA
    // ===========================================

    std::vector<int> cur_id = ids_hist.back();
    std::vector<GabowEdge> finalTree;

    // processa mapeamentos do último para o primeiro
    for (int step = (int)ids_hist.size() - 2; step >= 0; --step)
    {
        const auto &prev_id = ids_hist[step];
        const auto &preds = pre_hist[step];

        // expande todas as arestas escolhidas deste nível
        for (auto &edge : preds)
        {
            int u = edge.from;
            int v = edge.to;

            // só adiciona se o nó ainda é válido após expansões
            if (cur_id[v] == v)
                finalTree.emplace_back(u, v, edge.weight);
        }

        // atualiza cur_id (desfazendo contração)
        std::vector<int> new_map(prev_id.size());
        for (int i = 0; i < (int)prev_id.size(); ++i)
        {
            new_map[i] = cur_id[ prev_id[i] ];
        }
        cur_id = new_map;
    }

    // remove duplicatas
    std::sort(finalTree.begin(), finalTree.end(), [](const GabowEdge &a, const GabowEdge &b)
              { return (a.to < b.to) || (a.to == b.to && a.from < b.from); });

    finalTree.erase(std::unique(finalTree.begin(), finalTree.end(),
                                [](const GabowEdge &a, const GabowEdge &b)
                                { return a.from == b.from && a.to == b.to; }),
                    finalTree.end());

    return finalTree;
}

/*
 * Executa o algoritmo de Gabow e computa a AGM direcionada.
 */
std::vector<GabowEdge> GabowMST::compute(int root)
{
    if (root < 0 || root >= n)
        throw std::invalid_argument("Raiz fora do intervalo válido.");

    arborescence = gabow(n, root, edges);

    totalWeight = 0.0;
    for (const auto &e : arborescence)
        totalWeight += e.weight;

    return arborescence;
}

/*
 * Impressão amigável da árvore geradora mínima.
 */
void GabowMST::printArborescence(const std::vector<GabowEdge> &arb) const
{
    std::cout << "\n--- Arborescência Geradora Mínima (Gabow) ---\n";

    double sum = 0.0;
    for (const auto &e : arb)
    {
        std::cout << e.from << " -> " << e.to
                  << " (peso: " << e.weight << ")\n";
        sum += e.weight;
    }

    std::cout << "Peso total: " << sum << "\n";
}
