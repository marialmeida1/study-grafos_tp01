#include <iostream>
#include <vector>

// Estrutura que representa uma aresta entre dois vértices v e w.
// Pode ser utilizada tanto para grafos dirigidos quanto não-dirigidos.
struct Aresta 
{
    int v, w;
    // Construtor padrão inicializa v e w com -1 (valor inválido).
    Aresta (int v = -1, int w = -1) : v(v) , w(w) { } 
};

// Classe base abstrata para estruturas de grafos.
// Permite reutilização de código e serve como base para diferentes tipos de grafos (dirigidos, não-dirigidos, ponderados, etc).
class BaseGrafo {
protected:
    int numV;      // Número de vértices do grafo.
    int numA;      // Número de arestas do grafo.
    bool dirigido; // Indica se o grafo é dirigido (true) ou não-dirigido (false).

public:
    // Construtor: inicializa o número de vértices, zera as arestas e define se é dirigido.
    BaseGrafo(int V, bool dirigido) : numV(V), numA(0), dirigido(dirigido) {}
    virtual ~BaseGrafo() {}

    // Retorna o número de vértices.
    int V() const { return numV; }
    // Retorna o número de arestas.
    int A() const { return numA; }
    // Retorna se o grafo é dirigido.
    bool direcionado() const { return dirigido; }

    // Métodos virtuais puros para inserção e verificação de arestas.
    // Devem ser implementados pelas classes derivadas.
    virtual void inserirAresta(int, int) = 0;
    virtual bool existeAresta(int, int) const = 0;
};

// Classe Grafo: implementação concreta de um grafo simples usando lista de adjacência.
// Herda de BaseGrafo, podendo ser reutilizada ou estendida para outros tipos de grafos.
class Grafo : public BaseGrafo
{
private:
    // Lista de adjacência: para cada vértice, armazena as arestas incidentes.
    std::vector< std::vector<Aresta> > adj;

public:
    // Construtor: inicializa o grafo com V vértices e define se é dirigido.
    Grafo(int V, bool dirigido) : BaseGrafo(V, dirigido), adj(V) {}
    ~Grafo() {}

    // Insere uma aresta entre os vértices v e w.
    // Para grafos não-dirigidos, insere a aresta nos dois sentidos.
    void inserirAresta(int v, int w) override {
        if (!existeAresta(v, w)) {
            adj[v].push_back(Aresta(v, w));
            numA++;
            // Se o grafo não é dirigido e não é laço, insere também a aresta oposta.
            if (!dirigido && v != w) {
                adj[w].push_back(Aresta(w, v));
            }
        }
    }

    // Verifica se existe uma aresta entre v e w.
    bool existeAresta(int v, int w) const override {
        for (const auto& aresta : adj[v]) {
            if (aresta.w == w) return true;
        }
        return false;
    }

    // Método legado para compatibilidade (faz o mesmo que existeAresta).
    bool aresta(int v, int w) { return existeAresta(v, w); }

    // Classe interna para iterar sobre a lista de adjacência de um vértice.
    class adjIterator 
    {
        const Grafo& G; // Referência ao grafo.
        int v, index;   // v: vértice de interesse, index: posição atual na lista.
    public:
        // Construtor: recebe o grafo e o vértice a ser iterado.
        adjIterator(const Grafo& G, int v) : G(G), v(v), index(0) {}
        // Inicia a iteração.
        Aresta begin() { index = 0; return next(); }
        // Retorna a próxima aresta da lista de adjacência.
        Aresta next() {
            if (index < (int)G.adj[v].size())
                return G.adj[v][index++];
            else
                return Aresta(-1, -1); // Retorna aresta inválida ao final.
        }
        // Verifica se chegou ao final da lista de adjacência.
        bool end() { return index >= (int)G.adj[v].size(); }
    };
};

// Função principal de demonstração.
// Cria um grafo simples, insere arestas e imprime informações básicas.
int main() {
    Grafo g(5, false); // Cria um grafo com 5 vértices, não-dirigido.
    g.inserirAresta(0, 1);
    g.inserirAresta(0, 2);

    std::cout << "Numero de vertices: " << g.V() << std::endl;
    std::cout << "Numero de arestas: " << g.A() << std::endl;
    std::cout << "Aresta entre 0 e 1? " << (g.existeAresta(0, 1) ? "Sim" : "Nao") << std::endl;
    std::cout << "Aresta entre 1 e 2? " << (g.existeAresta(1, 2) ? "Sim" : "Nao") << std::endl;

    return 0;
}