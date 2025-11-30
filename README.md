# Trabalho de Teoria de Grafos e Computabilidade

Este projeto é um trabalho de grupo da disciplina **Teoria de Grafos e Computabilidade**, que tem como objetivo implementar estruturas básicas de grafos e operações relacionadas.

---

## Explicação Básica

O programa implementa grafos não direcionados e não ponderados, permitindo a criação de vértices e arestas, além de operações básicas de manipulação e consulta da estrutura do grafo.

---

## Organização do Código

O projeto está organizado da seguinte forma:

```
study-grafos_tp01/
├─ include/
│ ├─ Edge.h # Definição da classe Aresta
│ ├─ Graph.h # Definição da classe Grafo
│ ├─ GraphBase.h # Classes base ou utilitárias
│ ├─ WeightedEdge.h
│ ├─ WeightedGraph.h
│ ├─ EdmondsMST.h
│ └─ TarjanMST.h
├─ src/
│ ├─ Graph.cpp # Implementação da classe Grafo
│ ├─ WeightedGraph.cpp
│ ├─ EdmondsMST.cpp
│ ├─ TarjanMST.cpp
│ └─ main.cpp # Função principal do programa
└─ main # Executável gerado após compilação
```


- **include/**: Contém todos os arquivos de cabeçalho (.h) usados no projeto.
- **src/**: Contém os arquivos de implementação (.cpp) e o arquivo `main.cpp`.
- **main**: Executável gerado após a compilação.

---

## Como Executar o Código

Para compilar e executar o projeto, utilize o seguinte comando no terminal:

**Grafo Não-Ponderado**

```bash
g++ -Iinclude src/main.cpp src/Graph.cpp -o main
./main
``` 

**Todos os Grafos**

```bash
g++ -DWEIGHTED_GRAPH -Iinclude src/main.cpp src/Graph.cpp src/WeightedGraph.cpp -o main
./main
``` 

* -Iinclude adiciona a pasta include ao caminho de busca dos arquivos de cabeçalho.
* `src/main.cpp` `src/Graph.cpp` `src/WeightedGraph.cpp` são os arquivos de implementação.
* -o main especifica o nome do executável.

## Integrantes

- [![GitHub](https://img.shields.io/badge/-Alice-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/alicesalim)
- [![GitHub](https://img.shields.io/badge/-Arthur-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/ArthurCRodrigues) 
- [![GitHub](https://img.shields.io/badge/-Bruna-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/cestpassion)
- [![GitHub](https://img.shields.io/badge/-Daniel-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/vitorrdan)
- [![GitHub](https://img.shields.io/badge/-Felipe-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/nkdwon)
- [![GitHub](https://img.shields.io/badge/-Gustavo-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/GhrCastro)
- [![GitHub](https://img.shields.io/badge/-Mariana-181717?style=flat-square&logo=github&logoColor=white)](https://github.com/marialmeida)

---

## Conclusão

Este projeto foi desenvolvido como trabalho de grupo para a disciplina **Teoria de Grafos e Computabilidade**. Ele tem como objetivo aplicar os conceitos aprendidos em sala de aula, explorando estruturas de grafos e algoritmos relacionados, de forma prática e colaborativa.  

Através deste trabalho, os integrantes puderam aprimorar suas habilidades em programação, organização de código, e trabalho em equipe, além de consolidar conceitos teóricos essenciais para a área de computação.  

Esperamos que este repositório sirva como referência para estudos futuros e como um exemplo de aplicação prática dos conceitos abordados na disciplina.  

---

Agradecemos a todos que contribuíram direta ou indiretamente para o desenvolvimento deste projeto!
