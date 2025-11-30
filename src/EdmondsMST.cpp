#include "EdmondsMST.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>

/*
 * Edmonds/Chu–Liu para Arborescência Geradora Mínima (direcionada).
 * Estrutura geral:
 *   1) Escolher para cada vértice a aresta entrante de menor peso.
 *   2) Detectar ciclos nessas escolhas.
 *   3) Contrair ciclos e ajustar pesos.
 *   4) Resolver recursivamente no grafo contraído.
 *   5) Reexpandir e reconstruir a solução final.
 */

EdmondsMST::EdmondsMST(const WeightedGraph &g)
{
    n = g.V();

    // Extrai todas as arestas do grafo.
    /* for (int u = 0; u < n; ++u)
    {
        for (int v = 0; v < n; ++v)
        {
            if (g.hasEdge(u, v))
            {
                edges.emplace_back(u, v, g.getWeight(u, v));
            }
        }
    } */

    for (int u = 0; u < n; ++u) {
        WeightedGraph::AdjIterator it(g, u); // Supondo que AdjIterator seja acessível
        for (auto edge = it.begin();!it.end(); edge = it.next()) {
            edges.emplace_back(u, edge.w, edge.weight);
        }
    }

    totalWeight = 0.0;
}

/*
 * Função principal do Edmonds:
 * - Recebe N vértices, raiz, e lista de arestas.
 * - Retorna uma lista com as arestas da arborescência mínima.
 */
static std::vector<EdmondsEdge> edmonds(int N, int root, const std::vector<EdmondsEdge> &E)
{
    const double INF = std::numeric_limits<double>::infinity();
    std::vector<EdmondsEdge> result;

    if (N == 0)
        return result;

    // 1) Seleciona aresta entrante mínima para cada vértice.
    std::vector<double> inWeight(N, INF);
    std::vector<int> pre(N, -1);

    for (const auto &e : E)
    {
        if (e.from != e.to && e.weight < inWeight[e.to])
        {
            inWeight[e.to] = e.weight;
            pre[e.to] = e.from;
        }
    }

    inWeight[root] = 0;
    pre[root] = -1;

    // Se algum vértice exceto raiz não tem aresta entrante → impossível.
    for (int v = 0; v < N; ++v)
    {
        if (v != root && inWeight[v] == INF)
            return {};
    }

    // 2) Detecta ciclos seguindo pre[].
    std::vector<int> id(N, -1), vis(N, -1);
    int components = 0;

    for (int v = 0; v < N; ++v)
    {
        if (v == root)
            continue;

        int u = v;
        while (u != root && id[u] == -1 && vis[u] != v)
        {
            vis[u] = v;
            u = pre[u];
        }

        // Detectou ciclo: marca todos os nós do ciclo com mesmo id.
        if (u != root && id[u] == -1 && vis[u] == v)
        {
            for (int x = pre[u]; x != u; x = pre[x])
                id[x] = components;
            id[u] = components++;
        }
    }

    // Se nenhum ciclo, resultado é pre[v] → v.
    if (components == 0)
    {
        for (int v = 0; v < N; ++v)
        {
            if (v != root)
                result.emplace_back(pre[v], v, inWeight[v]);
        }
        return result;
    }

    // 3) Atribui ids únicos aos vértices fora de ciclos.
    for (int v = 0; v < N; ++v)
    {
        if (id[v] == -1)
            id[v] = components++;
    }

    int M = components; // número de vértices após contração

    // Constrói arestas do grafo contraído com pesos ajustados.
    std::vector<EdmondsEdge> E2;
    for (const auto &e : E)
    {
        int u = id[e.from];
        int v = id[e.to];
        if (u != v)
            E2.emplace_back(u, v, e.weight - inWeight[e.to]);
    }

    // 4) Resolve recursivamente no grafo contraído.
    std::vector<EdmondsEdge> rec = edmonds(M, id[root], E2);

    if (rec.empty() && M > 1)
        return {};

    // 5) Reconstrução: começa com todas pre[v] → v.
    std::vector<EdmondsEdge> finalEdges;
    for (int v = 0; v < N; ++v)
    {
        if (v != root)
            finalEdges.emplace_back(pre[v], v, inWeight[v]);
    }

    // Substitui arestas de entrada para componentes contraídas.
    const double EPS = 1e-9;
    for (const auto &r : rec)
    {
        int a = r.from, b = r.to;

        // Busca aresta original correspondente ao componente a→b.
        for (const auto &e : E)
        {
            if (id[e.from] == a && id[e.to] == b)
            {
                double adjW = e.weight - inWeight[e.to];
                if (std::abs(adjW - r.weight) < EPS)
                {
                    // Substitui aresta que entra no componente b.
                    for (auto &fe : finalEdges)
                    {
                        if (id[fe.to] == b)
                        {
                            fe = EdmondsEdge(e.from, e.to, e.weight);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    // Limpa arestas inválidas.
    result.clear();
    for (const auto &e : finalEdges)
        if (e.from >= 0 && e.to >= 0)
            result.push_back(e);

    return result;
}

std::vector<EdmondsEdge> EdmondsMST::compute(int root)
{
    if (root < 0 || root >= n)
        throw std::invalid_argument("Raiz inválida.");

    arborescence = edmonds(n, root, edges);

    if (arborescence.empty() && n > 1)
        return arborescence;

    totalWeight = 0.0;
    for (const auto &e : arborescence)
        totalWeight += e.weight;

    return arborescence;
}

void EdmondsMST::printArborescence(const std::vector<EdmondsEdge> &arb) const
{
    std::cout << "\n--- Arborescência Mínima (Edmonds) ---\n";
    double sum = 0.0;

    for (const auto &e : arb)
    {
        std::cout << e.from << " -> " << e.to << " (peso: " << e.weight << ")\n";
        sum += e.weight;
    }

    std::cout << "Peso total: " << sum << "\n";
}