#ifndef GRAFO_PONDERADO_H
#define GRAFO_PONDERADO_H

#include <vector>
#include "structure.cpp"  

// Estrutura de aresta ponderada direcionada
struct ArestaP {
    int v, w;
    double peso; 
    ArestaP(int v = -1, int w = -1, double peso = 1.0)
        : v(v), w(w), peso(peso) {}
};

class GrafoPonderado : public BaseGrafo {
private:
    std::vector<std::vector<ArestaP>> adj;

public:
    GrafoPonderado(int V);
    ~GrafoPonderado() {}

    // Inserção
    void inserirAresta(int v, int w, double peso);
    void inserirAresta(int v, int w) override { inserirAresta(v, w, 1.0); }// método permite criar aresta ponderada com valor padrão

    // Remoção
    void removerAresta(int v, int w);

    // Atualização de peso
    bool atualizarPeso(int v, int w, double novoPeso);

    // Verificação
    bool existeAresta(int v, int w) const override;

    // Mostrar peso da aresta
    double getPeso(int v, int w) const;

    // Iterator
    class adjIterator {
        const GrafoPonderado& G;
        int v, index;
    public:
        adjIterator(const GrafoPonderado& G, int v);
        ArestaP begin();
        ArestaP next();
        bool end();
    };
};

#endif
