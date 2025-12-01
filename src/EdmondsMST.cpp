#include "EdmondsMST.h"
#include <algorithm>
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <iostream>

// Constante para tolerância em comparações de ponto flutuante
const double EPSILON = 1e-9;

WeightedGraph EdmondsMST::obterArborescencia(WeightedGraph& grafo, int raiz) {
    int V = grafo.V();
    std::vector<ArestaInterna> arestasIniciais;
    int idCounter = 0;

    // 1. Converter o WeightedGraph para nossa estrutura interna de arestas
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            // Ignora self-loops iniciais, se houver
            if (e.v != e.w) {
                arestasIniciais.push_back({e.v, e.w, e.weight, idCounter++});
            }
            if (it.end()) break;
            e = it.next();
        }
    }

    // 2. Executar o algoritmo recursivo
    std::vector<ArestaInterna> arestasSelecionadas = processarEdmonds(V, raiz, arestasIniciais);

    // 3. Reconstruir o Grafo de Saída
    WeightedGraph arborescencia(V, true);
    
    // Precisamos mapear de volta os IDs das arestas para os valores originais
    // Como modificamos pesos na recursão, usamos o idOriginal para recuperar os dados reais
    // Recriamos um mapa rápido das arestas originais para consulta
    std::map<int, WeightedEdge> mapaOriginal;
    idCounter = 0;
    for (int i = 0; i < V; ++i) {
        WeightedGraph::AdjIterator it(grafo, i);
        WeightedEdge e = it.begin();
        while (e.v != -1) {
            if (e.v != e.w) mapaOriginal[idCounter++] = e;
            if (it.end()) break;
            e = it.next();
        }
    }

    for (const auto& a : arestasSelecionadas) {
        // Busca a aresta original baseada no ID preservado
        if (mapaOriginal.count(a.idOriginal)) {
            WeightedEdge original = mapaOriginal[a.idOriginal];
            arborescencia.insertEdge(original.v, original.w, original.weight);
        }
    }

    return arborescencia;
}

std::vector<EdmondsMST::ArestaInterna> EdmondsMST::processarEdmonds(int numVertices, int raiz, const std::vector<ArestaInterna>& arestas) {
    std::vector<ArestaInterna> resultado;

    // --- Passo 1: Seleção Gulosa (Menor aresta de entrada para cada vértice) ---
    std::vector<int> pai(numVertices, -1);
    std::vector<double> menorPeso(numVertices, std::numeric_limits<double>::max());
    std::vector<int> indiceArestaEscolhida(numVertices, -1);

    for (int i = 0; i < (int)arestas.size(); ++i) {
        const auto& aresta = arestas[i];
        if (aresta.v != aresta.u && aresta.v != raiz) { // Ignora loops e arestas entrando na raiz
            if (aresta.peso < menorPeso[aresta.v]) {
                menorPeso[aresta.v] = aresta.peso;
                pai[aresta.v] = aresta.u;
                indiceArestaEscolhida[aresta.v] = i;
            }
        }
    }

    // --- Passo 2: Detecção de Ciclos ---
    std::vector<int> grupo(numVertices, -1); // ID do novo super-vértice
    std::vector<bool> visitado(numVertices, false);
    std::vector<bool> naPilha(numVertices, false);
    
    int contagemSuperVertices = 0;
    bool cicloEncontrado = false;

    // Busca ciclos formados pelas arestas escolhidas
    for (int i = 0; i < numVertices; ++i) {
        if (i == raiz || grupo[i] != -1 || pai[i] == -1) continue;

        int curr = i;
        std::vector<int> caminho;
        while (curr != -1 && curr != raiz && !visitado[curr]) {
            visitado[curr] = true;
            naPilha[curr] = true;
            caminho.push_back(curr);
            curr = pai[curr];
        }

        // Se encontrou um nó que está na pilha atual, fechou um ciclo
        if (curr != -1 && curr != raiz && naPilha[curr]) {
            cicloEncontrado = true;
            int vCiclo = curr;
            // Marca todos do ciclo com o novo ID
            do {
                grupo[vCiclo] = contagemSuperVertices;
                vCiclo = pai[vCiclo];
            } while (vCiclo != curr);
            contagemSuperVertices++;
        }

        // Limpa a pilha para a próxima iteração
        for (int v : caminho) naPilha[v] = false;
    }

    // CASO BASE: Se não houver ciclos, retornamos as arestas escolhidas
    if (!cicloEncontrado) {
        for (int i = 0; i < numVertices; ++i) {
            if (i != raiz && indiceArestaEscolhida[i] != -1) {
                resultado.push_back(arestas[indiceArestaEscolhida[i]]);
            }
        }
        return resultado;
    }

    // --- Passo 3: Contração (Recursão) ---
    int numVerticesContraidos = contagemSuperVertices;
    std::vector<int> mapeamento(numVertices); // Mapeia old_id -> new_id
    
    // Mapeia vértices que NÃO estão em ciclos
    for (int i = 0; i < numVertices; ++i) {
        if (grupo[i] == -1) {
            grupo[i] = numVerticesContraidos;
            mapeamento[i] = numVerticesContraidos;
            numVerticesContraidos++;
        } else {
            mapeamento[i] = grupo[i]; // Vértices do ciclo assumem ID 0..k-1
        }
    }

    std::vector<ArestaInterna> novasArestas;
    for (const auto& a : arestas) {
        int u_novo = mapeamento[a.u];
        int v_novo = mapeamento[a.v];

        if (u_novo != v_novo) {
            ArestaInterna nova = a;
            nova.u = u_novo;
            nova.v = v_novo;
            
            // Se o destino original estava num ciclo, ajustamos o peso
            // Novo Peso = Peso Original - Peso da aresta do ciclo que entra em v
            if (grupo[a.v] < contagemSuperVertices) { // Significa que 'v' fazia parte de um super-vértice
                nova.peso -= menorPeso[a.v];
            }
            novasArestas.push_back(nova);
        }
    }

    int novaRaiz = mapeamento[raiz];
    std::vector<ArestaInterna> arborescenciaContraida = processarEdmonds(numVerticesContraidos, novaRaiz, novasArestas);

    // --- Passo 4: Expansão ---
    // Recupera as arestas originais baseadas no idOriginal
    std::vector<int> arestaEntradaCiclo(contagemSuperVertices, -1); // Qual aresta quebrou o ciclo K?

    for (const auto& a_contraida : arborescenciaContraida) {
        // Encontra a aresta original correspondente (pelo idOriginal que foi preservado)
        // Precisamos achar os dados originais dessa aresta na lista 'arestas' deste escopo
        // (Isso poderia ser otimizado com um map, mas linear é aceitável aqui)
        ArestaInterna arestaReal;
        bool achou = false;
        
        // Procura na lista local de arestas quem tem esse idOriginal
        for(const auto& original : arestas) {
            if(original.idOriginal == a_contraida.idOriginal) {
                arestaReal = original;
                achou = true;
                break;
            }
        }

        if(achou) {
            resultado.push_back(arestaReal);
            // Se o destino dessa aresta fazia parte de um ciclo (é um super-vértice < contagem)
            // Marcamos qual super-vértice foi "invadido" para removermos a aresta interna dele depois
            if (grupo[arestaReal.v] < contagemSuperVertices) {
                arestaEntradaCiclo[grupo[arestaReal.v]] = arestaReal.v;
            }
        }
    }

    // Adiciona as arestas internas dos ciclos, exceto aquela que foi substituída pela entrada externa
    for (int i = 0; i < numVertices; ++i) {
        // Se i faz parte de um ciclo (grupo[i] < contagem) E NÃO é o ponto de entrada desse ciclo
        if (grupo[i] < contagemSuperVertices && arestaEntradaCiclo[grupo[i]] != i) {
             if (indiceArestaEscolhida[i] != -1) {
                 resultado.push_back(arestas[indiceArestaEscolhida[i]]);
             }
        }
    }

    return resultado;
}