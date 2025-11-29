# Algoritmo de Tarjan para Árvore Geradora Mínima Direcionada

## Introdução

Este documento apresenta a implementação do **Algoritmo de Tarjan** para encontrar a **Árvore Geradora Mínima Direcionada** (também conhecida como **Arborescência Geradora Mínima**) em grafos direcionados ponderados.

## Conceitos Fundamentais

### Arborescência Geradora Mínima
Uma **arborescência geradora mínima** é uma subárvore de um grafo direcionado que:
- Conecta todos os vértices do grafo
- Possui uma raiz específica da qual todos os outros vértices são alcançáveis
- Cada vértice (exceto a raiz) tem exatamente uma aresta de entrada
- A soma dos pesos das arestas é mínima

### Diferença entre MST e Arborescência
- **MST (Árvore Geradora Mínima)**: Para grafos não direcionados
- **Arborescência**: Para grafos direcionados, com uma raiz específica

## Implementação

### Estrutura de Dados

#### TarjanEdge
```cpp
struct TarjanEdge {
    int from, to;
    double weight;
    TarjanEdge(int f, int t, double w) : from(f), to(t), weight(w) {}
};
```

#### Classe TarjanMST
```cpp
class TarjanMST {
private:
    std::vector<TarjanEdge> edges;
    int numVertices;
    
    // Estruturas auxiliares para o algoritmo
    std::vector<double> minCost;
    std::vector<int> parent;
    std::vector<int> id;
    std::vector<int> visit;
    std::vector<int> inCycle;
    
    // Métodos auxiliares
    void findMinIncomingEdges();
    
public:
    TarjanMST(const WeightedGraph& graph);
    std::vector<TarjanEdge> findMinimumSpanningArborescence(int root);
    void printArborescence(const std::vector<TarjanEdge>& arborescence) const;
};
```

### Algoritmo Principal

#### 1. Construtor - Extração de Arestas
```cpp
TarjanMST::TarjanMST(const WeightedGraph& graph) : numVertices(graph.V()) {
    // Extrair todas as arestas do grafo
    for (int v = 0; v < numVertices; v++) {
        for (int w = 0; w < numVertices; w++) {
            if (graph.hasEdge(v, w)) {
                double weight = graph.getWeight(v, w);
                edges.push_back(TarjanEdge(v, w, weight));
            }
        }
    }
    
    // Inicializar estruturas auxiliares
    minCost.resize(numVertices, INF);
    parent.resize(numVertices, -1);
    id.resize(numVertices, -1);
    visit.resize(numVertices, -1);
    inCycle.resize(numVertices, -1);
}
```

#### 2. Encontrar Arestas de Entrada Mínima
```cpp
void TarjanMST::findMinIncomingEdges() {
    // Inicializar com infinito
    std::fill(minCost.begin(), minCost.end(), INF);
    std::fill(parent.begin(), parent.end(), -1);
    
    // Para cada aresta, verificar se é a menor entrada para o vértice destino
    for (const auto& edge : edges) {
        if (edge.weight < minCost[edge.to]) {
            minCost[edge.to] = edge.weight;
            parent[edge.to] = edge.from;
        }
    }
}
```

#### 3. Algoritmo Principal
```cpp
std::vector<TarjanEdge> TarjanMST::findMinimumSpanningArborescence(int root) {
    std::vector<TarjanEdge> result;
    
    if (edges.empty()) {
        std::cout << "Grafo vazio!" << std::endl;
        return result;
    }
    
    // Passo 1: Encontrar arestas de entrada mínima para cada vértice
    findMinIncomingEdges();
    minCost[root] = 0; // Raiz não tem aresta de entrada
    
    // Verificar se existe solução
    for (int i = 0; i < numVertices; i++) {
        if (i != root && minCost[i] == INF) {
            std::cout << "Não existe arborescência a partir da raiz " << root << std::endl;
            return result;
        }
    }
    
    // Passo 2: Verificar se há ciclos
    int cycleCount = 0;
    std::fill(id.begin(), id.end(), -1);
    std::fill(visit.begin(), visit.end(), -1);
    
    for (int v = 0; v < numVertices; v++) {
        if (v == root) continue;
        
        if (visit[v] == -1) {
            // DFS para detectar ciclos
            int u = v;
            while (visit[u] != v && id[u] == -1 && u != root) {
                visit[u] = v;
                u = parent[u];
            }
            
            // Se encontrou um ciclo
            if (u != root && id[u] == -1) {
                // Marcar todos os vértices do ciclo
                while (id[u] == -1) {
                    id[u] = cycleCount;
                    u = parent[u];
                }
                cycleCount++;
            }
        }
    }
    
    // Se não há ciclos, construir resultado
    if (cycleCount == 0) {
        for (int v = 0; v < numVertices; v++) {
            if (v != root) {
                result.push_back(TarjanEdge(parent[v], v, minCost[v]));
            }
        }
        return result;
    }
    
    // Caso contrário, contrair ciclos e resolver recursivamente
    std::cout << "Ciclos detectados. Implementação completa requer contração de ciclos." << std::endl;
    
    return result;
}
```

## Exemplo de Uso

### Código de Teste
```cpp
void testTarjanMST() {
    std::cout << "\n--- Testing Tarjan MST Algorithm ---" << std::endl;
    
    // Criar um grafo direcionado ponderado para teste
    WeightedGraph graph(4, true);
    
    // Adicionar arestas
    graph.insertEdge(0, 1, 5.0);
    graph.insertEdge(0, 2, 3.0);
    graph.insertEdge(1, 2, 2.0);
    graph.insertEdge(1, 3, 4.0);
    graph.insertEdge(2, 3, 1.0);
    graph.insertEdge(3, 0, 6.0);
    
    std::cout << "Grafo de teste criado com 4 vértices." << std::endl;
    
    // Executar algoritmo de Tarjan
    TarjanMST tarjan(graph);
    
    // Testar com raiz 0
    std::cout << "\nCalculando arborescência com raiz 0:" << std::endl;
    std::vector<TarjanEdge> mst = tarjan.findMinimumSpanningArborescence(0);
    tarjan.printArborescence(mst);
    
    // Testar com raiz 1
    std::cout << "\nCalculando arborescência com raiz 1:" << std::endl;
    TarjanMST tarjan2(graph);
    std::vector<TarjanEdge> mst2 = tarjan2.findMinimumSpanningArborescence(1);
    tarjan2.printArborescence(mst2);
}
```

### Resultado da Execução
```
--- Testing Tarjan MST Algorithm ---
Grafo de teste criado com 4 vértices.

Calculando arborescência com raiz 0:

--- Arborescência Geradora Mínima (Tarjan) ---
0 -> 1 (peso: 5)
1 -> 2 (peso: 2)
2 -> 3 (peso: 1)
Peso total: 8

Calculando arborescência com raiz 1:

--- Arborescência Geradora Mínima (Tarjan) ---
3 -> 0 (peso: 6)
1 -> 2 (peso: 2)
2 -> 3 (peso: 1)
Peso total: 9
```

## Análise dos Resultados

### Grafo de Entrada
```
Vértices: 0, 1, 2, 3
Arestas:
- 0 → 1 (peso 5.0)
- 0 → 2 (peso 3.0)
- 1 → 2 (peso 2.0)
- 1 → 3 (peso 4.0)
- 2 → 3 (peso 1.0)
- 3 → 0 (peso 6.0)
```

### Arborescência com Raiz 0
- **Estrutura**: 0 → 1 → 2 → 3
- **Arestas selecionadas**:
  - 0 → 1 (peso 5): única entrada para vértice 1
  - 1 → 2 (peso 2): menor entrada para vértice 2 (2 < 3)
  - 2 → 3 (peso 1): menor entrada para vértice 3 (1 < 4)
- **Peso total**: 8

### Arborescência com Raiz 1
- **Estrutura**: Raiz 1, com subárvore 2 → 3 → 0
- **Arestas selecionadas**:
  - 3 → 0 (peso 6): única entrada para vértice 0
  - 1 → 2 (peso 2): menor entrada para vértice 2
  - 2 → 3 (peso 1): menor entrada para vértice 3
- **Peso total**: 9

## Complexidade

- **Temporal**: O(V²) para grafos densos, onde V é o número de vértices
- **Espacial**: O(V + E), onde E é o número de arestas

## Limitações da Implementação Atual

1. **Detecção de Ciclos**: Implementada básica
2. **Contração de Ciclos**: Não implementada (retorna mensagem informativa)
3. **Otimização**: Pode ser otimizada para grafos esparsos

## Compilação e Execução

### Comandos
```bash
# Compilar
g++ -I../include -DWEIGHTED_GRAPH -o test_weighted main.cpp Graph.cpp WeightedGraph.cpp TarjanMST.cpp

# Executar
./test_weighted
```

### Arquivos Necessários
- `TarjanMST.h` - Header da classe
- `TarjanMST.cpp` - Implementação da classe
- `WeightedGraph.h/.cpp` - Grafo ponderado
- `main.cpp` - Função de teste

## Problemas Resolvidos Durante o Desenvolvimento

1. **Bug no Iterator**: O iterator do `WeightedGraph` estava pulando arestas
2. **Solução Aplicada**: Método direto usando `hasEdge()` e `getWeight()`
3. **Resultado**: Extração correta de todas as arestas do grafo

## Conclusão

A implementação do Algoritmo de Tarjan foi concluída com sucesso, sendo capaz de:
- Extrair corretamente todas as arestas de um grafo ponderado direcionado
- Encontrar as arestas de entrada mínima para cada vértice
- Detectar a ausência de ciclos em casos simples
- Construir a arborescência geradora mínima quando não há ciclos
- Produzir resultados matematicamente corretos para diferentes raízes

A implementação está funcional para grafos que não possuem ciclos nas arestas de entrada mínima, cobrindo uma importante classe de problemas práticos.