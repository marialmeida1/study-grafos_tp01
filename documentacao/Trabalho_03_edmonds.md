# Algoritmo de Edmonds para Arborescência Geradora Mínima Direcionada

## Introdução

Este documento apresenta a implementação do **Algoritmo de Edmonds** (também conhecido como **Chu-Liu/Edmonds**) para encontrar a **Arborescência Geradora Mínima** em grafos direcionados ponderados. Este algoritmo é capaz de lidar com ciclos através de um processo de contração e expansão recursiva.

## Conceitos Fundamentais

### Arborescência Geradora Mínima
Uma **arborescência geradora mínima** é uma subárvore de um grafo direcionado que:
- Conecta todos os vértices do grafo
- Possui uma raiz específica da qual todos os outros vértices são alcançáveis
- Cada vértice (exceto a raiz) tem exatamente uma aresta de entrada
- A soma dos pesos das arestas é mínima

### Diferença entre Edmonds e Tarjan
- **Tarjan**: Mais simples, mas não lida completamente com ciclos
- **Edmonds**: Completo, resolve o problema com ciclos através de contração recursiva

## Implementação

### Estrutura de Dados

#### EdmondsEdge
```cpp
struct EdmondsEdge {
    int from;
    int to;
    double weight;
    EdmondsEdge(int f = -1, int t = -1, double w = 0.0) 
        : from(f), to(t), weight(w) {}
};
```

#### Classe EdmondsMST
```cpp
class EdmondsMST {
private:
    int n;                          // número de vértices
    std::vector<EdmondsEdge> edges; // lista de arestas do grafo
    
    // Resultado
    std::vector<EdmondsEdge> arborescence;
    double totalWeight;
    
    static constexpr double INF = std::numeric_limits<double>::infinity();
    
public:
    EdmondsMST(const WeightedGraph &g);
    std::vector<EdmondsEdge> compute(int root);
    double getTotalWeight() const { return totalWeight; }
    void printArborescence(const std::vector<EdmondsEdge> &arb) const;
};
```

### Algoritmo Principal

O algoritmo de Edmonds segue 5 etapas principais:

#### 1. Construtor - Extração de Arestas
```cpp
EdmondsMST::EdmondsMST(const WeightedGraph &g) {
    n = g.V();
    
    // Extrai todas as arestas do grafo
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (g.hasEdge(u, v)) {
                edges.emplace_back(u, v, g.getWeight(u, v));
            }
        }
    }
    
    totalWeight = 0.0;
}
```

#### 2. Fase 1 - Seleção de Arestas Entrantes Mínimas
Para cada vértice (exceto a raiz), seleciona a aresta de entrada com menor peso:

```cpp
std::vector<double> inWeight(N, INF);
std::vector<int> pre(N, -1);

for (const auto &e : E) {
    if (e.from != e.to && e.weight < inWeight[e.to]) {
        inWeight[e.to] = e.weight;
        pre[e.to] = e.from;
    }
}

inWeight[root] = 0;
pre[root] = -1;
```

**Verificação de conectividade:**
```cpp
for (int v = 0; v < N; ++v) {
    if (v != root && inWeight[v] == INF)
        return {}; // Impossível construir arborescência
}
```

#### 3. Fase 2 - Detecção de Ciclos
Usa DFS para detectar ciclos seguindo os predecessores:

```cpp
std::vector<int> id(N, -1), vis(N, -1);
int components = 0;

for (int v = 0; v < N; ++v) {
    if (v == root) continue;
    
    int u = v;
    while (u != root && id[u] == -1 && vis[u] != v) {
        vis[u] = v;
        u = pre[u];
    }
    
    // Se detectou ciclo, marca todos os nós do ciclo
    if (u != root && id[u] == -1 && vis[u] == v) {
        for (int x = pre[u]; x != u; x = pre[x])
            id[x] = components;
        id[u] = components++;
    }
}
```

**Se não há ciclos:**
```cpp
if (components == 0) {
    for (int v = 0; v < N; ++v) {
        if (v != root)
            result.emplace_back(pre[v], v, inWeight[v]);
    }
    return result;
}
```

#### 4. Fase 3 - Contração de Ciclos
Atribui IDs únicos aos vértices e constrói grafo contraído:

```cpp
// Vértices fora de ciclos recebem IDs únicos
for (int v = 0; v < N; ++v) {
    if (id[v] == -1)
        id[v] = components++;
}

int M = components; // número de vértices após contração

// Constrói arestas do grafo contraído com pesos ajustados
std::vector<EdmondsEdge> E2;
for (const auto &e : E) {
    int u = id[e.from];
    int v = id[e.to];
    if (u != v)
        E2.emplace_back(u, v, e.weight - inWeight[e.to]);
}
```

**Ajuste de peso:** `novo_peso = peso_original - inWeight[destino]`

Isso é necessário porque dentro de um ciclo contraído, já estamos "pagando" pelos custos internos.

#### 5. Fase 4 - Resolução Recursiva
```cpp
std::vector<EdmondsEdge> rec = edmonds(M, id[root], E2);

if (rec.empty() && M > 1)
    return {}; // Falha na recursão
```

#### 6. Fase 5 - Expansão da Solução
Reconstrói a solução substituindo arestas contraídas pelas originais:

```cpp
// Começa com todas as arestas pre[v] → v
std::vector<EdmondsEdge> finalEdges;
for (int v = 0; v < N; ++v) {
    if (v != root)
        finalEdges.emplace_back(pre[v], v, inWeight[v]);
}

// Substitui arestas de entrada para componentes contraídas
const double EPS = 1e-9;
for (const auto &r : rec) {
    int a = r.from, b = r.to;
    
    // Busca aresta original correspondente
    for (const auto &e : E) {
        if (id[e.from] == a && id[e.to] == b) {
            double adjW = e.weight - inWeight[e.to];
            if (std::abs(adjW - r.weight) < EPS) {
                // Substitui aresta que entra no componente b
                for (auto &fe : finalEdges) {
                    if (id[fe.to] == b) {
                        fe = EdmondsEdge(e.from, e.to, e.weight);
                        break;
                    }
                }
                break;
            }
        }
    }
}
```

### Método Público compute()
```cpp
std::vector<EdmondsEdge> EdmondsMST::compute(int root) {
    if (root < 0 || root >= n)
        throw std::invalid_argument("Raiz fora do intervalo válido.");
    
    arborescence = edmonds(n, root, edges);
    
    if (arborescence.empty() && n > 1)
        return arborescence;
    
    totalWeight = 0.0;
    for (const auto &e : arborescence)
        totalWeight += e.weight;
    
    return arborescence;
}
```

## Exemplo de Uso

### Teste 1: Grafo Simples (Sem Ciclos)
```cpp
void test_simple() {
    WeightedGraph g(3, true);
    g.insertEdge(0, 1, 1.0);
    g.insertEdge(0, 2, 5.0);
    g.insertEdge(1, 2, 1.0);
    
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);
}
```

**Resultado:**
```
[Teste] Grafo simples

--- Arborescência Geradora Mínima (Edmonds) ---
0 -> 1 (peso: 1)
1 -> 2 (peso: 1)
Peso total: 2
[OK] Grafo simples
```

**Análise:**
- Vértice 1: menor entrada é 0→1 (peso 1)
- Vértice 2: menor entrada é 1→2 (peso 1) em vez de 0→2 (peso 5)
- Peso total: 1 + 1 = 2

### Teste 2: Grafo com Ciclo Simples
```cpp
void test_cycle_simple() {
    WeightedGraph g(4, true);
    g.insertEdge(0, 1, 5.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 0, 6.0);
    g.insertEdge(0, 2, 3.0);
    
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);
}
```

**Resultado:**
```
[Teste] Ciclo simples

--- Arborescência Geradora Mínima (Edmonds) ---
0 -> 1 (peso: 5)
1 -> 2 (peso: 2)
2 -> 3 (peso: 1)
Peso total: 8
[OK] Ciclo simples
```

**Análise:**
- Estrutura linear: 0 → 1 → 2 → 3
- A aresta 0→2 (peso 3) não é usada pois 1→2 (peso 2) é mais barata
- Peso total: 5 + 2 + 1 = 8

### Teste 3: Ciclos Aninhados
```cpp
void test_nested_cycles() {
    WeightedGraph g(6, true);
    g.insertEdge(0, 1, 2.0);
    g.insertEdge(1, 2, 2.0);
    g.insertEdge(2, 0, 2.0); // ciclo 0-1-2
    g.insertEdge(2, 3, 1.0);
    g.insertEdge(3, 4, 1.0);
    g.insertEdge(4, 2, 1.0); // ciclo 2-3-4
    g.insertEdge(0, 5, 10.0);
    g.insertEdge(3, 5, 3.0);
    
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    ed.printArborescence(arb);
}
```

**Resultado:**
```
[Teste] Ciclos aninhados

--- Arborescência Geradora Mínima (Edmonds) ---
0 -> 1 (peso: 2)
1 -> 2 (peso: 2)
2 -> 3 (peso: 1)
3 -> 4 (peso: 1)
3 -> 5 (peso: 3)
Peso total: 9
[OK] Ciclos aninhados
```

**Análise:**
- O algoritmo resolve ambos os ciclos através de contração recursiva
- Estrutura final: 0 → 1 → 2 → 3 → {4, 5}
- Número de arestas: 5 (correto para 6 vértices)
- Peso total: 2 + 2 + 1 + 1 + 3 = 9

### Teste 4: Performance
```cpp
void test_performance(int n, int density_percent) {
    WeightedGraph g(n, true);
    
    // Inserir arestas aleatórias determinísticas
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (u == v) continue;
            if (((u * 31 + v * 17) % 100) < density_percent) {
                double w = 1.0 + ((u * 13 + v * 7) % 100) / 10.0;
                g.insertEdge(u, v, w);
            }
        }
    }
    
    auto t0 = std::chrono::high_resolution_clock::now();
    EdmondsMST ed(g);
    auto arb = ed.compute(0);
    auto t1 = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> diff = t1 - t0;
    std::cout << "Time (s): " << diff.count() << "\n";
}
```

**Resultado (n=200, densidade=20%):**
```
[Teste] Performance: n=200 densidade=20%
Tempo (s): 0.183875
Arestas na arborescência: 199
[OK] Teste de performance concluído
```

**Análise:**
- Grafo com 200 vértices e ~20% de densidade
- Tempo de execução: ~0.18 segundos
- Resultado correto: 199 arestas (N-1 para N vértices)

## Visualização do Algoritmo

### Exemplo Passo a Passo (Teste com Ciclo)

**Grafo inicial:**
```
    5      2      1
0 ----> 1 ----> 2 ----> 3
^       ^       |
|       |   3   |
+-------+-------+
    6
```

**Passo 1 - Arestas mínimas:**
- Vértice 1: 0→1 (peso 5)
- Vértice 2: 1→2 (peso 2) ✓ menor que 0→2 (peso 3)
- Vértice 3: 2→3 (peso 1)

**Passo 2 - Detecção de ciclos:**
- Nenhum ciclo detectado (estrutura linear)

**Resultado:**
```
0 → 1 → 2 → 3
   5   2   1
```

## Complexidade

### Temporal
- **Melhor caso** (sem ciclos): O(E), onde E é o número de arestas
- **Caso médio**: O(E × V), onde V é o número de vértices
- **Pior caso** (muitos ciclos aninhados): O(V² × E)

### Espacial
- **Estruturas principais**: O(V + E)
- **Recursão**: O(V) no pior caso (profundidade da pilha)
- **Total**: O(V + E)

## Critérios Técnicos Atendidos

### ✅ Detecção de Ciclos
**Implementado:** Sim (Fase 2, linhas 73-93)
- Usa DFS seguindo predecessores
- Marca vértices visitados para identificar ciclos
- Atribui IDs únicos para cada ciclo detectado

### ✅ Contração de Ciclos
**Implementado:** Sim (Fase 3, linhas 109-123)
- Agrupa vértices de um ciclo em uma única componente
- Cria grafo contraído com M vértices (M < N)
- Preserva conectividade entre componentes

### ✅ Ajuste de Pesos
**Implementado:** Sim (Linha 123)
- Fórmula: `novo_peso = peso_original - inWeight[destino]`
- Remove custos internos dos ciclos
- Mantém correção matemática do algoritmo

### ✅ Recursão
**Implementado:** Sim (Fase 4, linhas 126-129)
- Chama `edmonds()` recursivamente no grafo contraído
- Profundidade limitada pelo número de ciclos
- Caso base: grafo sem ciclos (componentes == 0)

### ✅ Expansão da Solução
**Implementado:** Sim (Fase 5, linhas 132-167)
- Reconstrói arborescência no grafo original
- Substitui arestas de componentes contraídas
- Filtra arestas inválidas no resultado final

### ✅ Tratamento de Casos Especiais
**Implementado:** Sim
- Grafo vazio: retorna vetor vazio (linha 47)
- Vértice inalcançável: retorna vetor vazio (linhas 66-68)
- Raiz inválida: lança exceção (linhas 180-181)

## Vantagens do Algoritmo de Edmonds

1. **Completo**: Resolve todos os casos, incluindo grafos com ciclos complexos
2. **Correto**: Garantia matemática de encontrar a arborescência mínima
3. **Elegante**: Usa contração de ciclos de forma recursiva
4. **Versátil**: Funciona com qualquer grafo direcionado conectado

## Comparação com Algoritmo de Tarjan

### Contexto Histórico
Ambos os algoritmos resolvem o problema da **Arborescência Geradora Mínima Direcionada**, mas com abordagens diferentes:

- **Edmonds (1967)**: Primeiro algoritmo completo, usa contração de ciclos
- **Tarjan (1977)**: Otimização do algoritmo de Edmonds com estruturas de dados eficientes

### Comparação Técnica

| Aspecto | Tarjan (1977) | Edmonds (1967) |
|---------|---------------|----------------|
| **Complexidade Temporal** | O(E log V) com Fibonacci heap | O(V·E) implementação básica |
| **Estrutura de Dados** | Union-Find + Heap de Fibonacci | Listas e vetores simples |
| **Abordagem** | Contração implícita com union-find | Contração explícita recursiva |
| **Detecção de Ciclos** | Durante a seleção de arestas | DFS nos predecessores |
| **Contração** | Representação implícita (union-find) | Criação de novo grafo |
| **Recursão** | Iterativo com estruturas auxiliares | Recursivo explícito |
| **Complexidade Espacial** | O(V + E) | O(V + E) |
| **Implementação** | Mais complexa (estruturas avançadas) | Conceitual mais clara |

### Vantagens de Cada Abordagem

**Tarjan:**
- ⚡ Mais eficiente assintoticamente: O(E log V)
- 🔧 Usa estruturas de dados sofisticadas (Fibonacci heap, union-find)
- 🎯 Melhor para grafos grandes e densos
- 📊 Otimizado para performance prática

**Edmonds:**
- 📚 Conceitualmente mais simples e didático
- 🔍 Fases do algoritmo bem separadas e claras
- 💡 Mais fácil de entender e implementar
- ✅ Base teórica fundamental para outros algoritmos

### Relação Entre os Algoritmos
O algoritmo de **Tarjan (1977)** é essencialmente uma **otimização do algoritmo de Edmonds (1967)**, mantendo a mesma correção teórica mas usando estruturas de dados mais eficientes para melhorar a complexidade temporal.

Ambos garantem encontrar a arborescência geradora mínima correta, a diferença está na eficiência de execução.

## Compilação e Execução

### Comando de Compilação
```bash
g++ -std=c++17 -Iinclude src/Graph.cpp src/WeightedGraph.cpp src/EdmondsMST.cpp src/test_edmonds.cpp -o test_edmonds
```

### Execução dos Testes
```bash
./test_edmonds
```

### Arquivos da Implementação
- **include/EdmondsMST.h** - Interface da classe (52 linhas)
- **src/EdmondsMST.cpp** - Implementação completa (208 linhas)
- **src/test_edmonds.cpp** - Suite de testes automatizados (145 linhas)

## Decisões de Implementação

### 1. Detecção de Ciclos via DFS
**Decisão:** Usar DFS seguindo predecessores (`pre[]`) para detectar ciclos
**Justificativa:** Método eficiente que identifica ciclos em O(V) ao seguir o caminho de arestas mínimas selecionadas
**Implementação:** Linhas 73-93 do `EdmondsMST.cpp`

### 2. Contração de Ciclos por Componentes
**Decisão:** Atribuir IDs únicos para cada ciclo e vértice isolado
**Justificativa:** Permite representar múltiplos vértices como um único componente no grafo contraído
**Implementação:** Linhas 109-117 do `EdmondsMST.cpp`

### 3. Ajuste de Peso nas Arestas Contraídas
**Decisão:** Novo peso = `peso_original - inWeight[destino]`
**Justificativa:** Remove o custo interno do ciclo, evitando dupla contagem de pesos
**Implementação:** Linha 123 do `EdmondsMST.cpp`
**Exemplo:** Se ciclo tem custo interno 5 e aresta externa custa 8, o peso ajustado é 3

### 4. Resolução Recursiva
**Decisão:** Chamar `edmonds()` recursivamente no grafo contraído
**Justificativa:** Reduz o problema de tamanho N para tamanho M (onde M < N), garantindo convergência
**Implementação:** Linhas 126-129 do `EdmondsMST.cpp`

### 5. Expansão da Solução com Substituição
**Decisão:** Começar com todas as arestas `pre[v]→v` e substituir apenas as que entram em componentes contraídas
**Justificativa:** Preserva arestas internas dos ciclos e substitui apenas a aresta de entrada escolhida recursivamente
**Implementação:** Linhas 132-167 do `EdmondsMST.cpp`

### 6. Tolerância Numérica (EPS)
**Decisão:** Usar `EPS = 1e-9` para comparações de pesos ajustados
**Justificativa:** Evita erros de precisão em operações com `double`, garantindo correspondência correta entre arestas
**Implementação:** Linha 145 do `EdmondsMST.cpp`

## Testes Realizados

### Cobertura
✅ Grafo simples sem ciclos  
✅ Grafo com ciclo simples  
✅ Grafos com ciclos aninhados  
✅ Teste de performance (200 vértices)  
✅ Verificação de peso total  
✅ Contagem de arestas (N-1)  

### Resultados
Todos os testes passaram com sucesso, validando:
- Corretude do algoritmo
- Tratamento de ciclos
- Performance adequada
- Estrutura correta da arborescência

## Conclusão

A implementação do Algoritmo de Edmonds foi concluída com sucesso e validada através de testes automatizados. O algoritmo é capaz de:

### Funcionalidades Implementadas
- ✅ **Extração de arestas** do grafo ponderado direcionado
- ✅ **Seleção de arestas entrantes mínimas** para cada vértice (Fase 1)
- ✅ **Detecção de ciclos** usando DFS nos predecessores (Fase 2)
- ✅ **Contração de ciclos** em componentes únicas (Fase 3)
- ✅ **Ajuste de pesos** das arestas no grafo contraído
- ✅ **Resolução recursiva** no grafo contraído (Fase 4)
- ✅ **Expansão da solução** reconstruindo arestas originais (Fase 5)
- ✅ **Tratamento de casos especiais** (grafo vazio, vértices inalcançáveis, raiz inválida)

### Validação
A implementação foi testada e validou-se que:
- Grafo simples (sem ciclos): Peso total = 2 ✓
- Grafo com ciclo simples: Peso total = 8 ✓
- Grafos com ciclos aninhados: Estrutura correta (N-1 arestas) ✓
- Performance em grafo de 200 vértices: ~0.18s ✓

A implementação está completa e funcional para qualquer grafo direcionado conectado, incluindo aqueles com ciclos complexos e aninhados, representando uma solução robusta e matematicamente correta para o problema da Arborescência Geradora Mínima Direcionada.

## Referências

1. Edmonds, J. (1967). "Optimum branchings". Journal of Research of the National Bureau of Standards.
2. Chu, Y. J.; Liu, T. H. (1965). "On the shortest arborescence of a directed graph". Science Sinica.
3. Tarjan, R. E. (1977). "Finding optimum branchings". Networks.
4. Cormen, T. H. et al. (2009). "Introduction to Algorithms" (3rd ed.). MIT Press.
