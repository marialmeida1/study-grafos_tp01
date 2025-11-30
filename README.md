# Trabalho de Teoria de Grafos e Computabilidade

Este projeto é um trabalho de grupo da disciplina **Teoria de Grafos e Computabilidade**, que tem como objetivo implementar estruturas básicas de grafos e operações relacionadas.

---

## Explicação Básica

O programa implementa estruturas de grafos direcionados e não direcionados, tanto ponderados quanto não ponderados, permitindo a criação de vértices e arestas, além de operações básicas de manipulação e consulta da estrutura do grafo.

### Algoritmos Implementados

- **Algoritmo de Tarjan**: Para encontrar árvores geradoras mínimas em grafos direcionados
- **Algoritmo de Edmonds**: Para encontrar árvores geradoras mínimas em grafos direcionados  
- **Algoritmo de Gabow**: Para encontrar árvores geradoras mínimas em grafos direcionados

---

## Organização do Código

O projeto está organizado da seguinte forma:

```
study-grafos_tp01/
├─ include/
│ ├─ Edge.h              # Definição da classe Aresta básica
│ ├─ WeightedEdge.h      # Definição da classe Aresta ponderada
│ ├─ Graph.h             # Definição da classe Grafo básico
│ ├─ WeightedGraph.h     # Definição da classe Grafo ponderado
│ ├─ GraphBase.h         # Classes base ou utilitárias
│ ├─ TarjanMST.h         # Algoritmo de Tarjan para MST direcionada
│ ├─ EdmondsMST.h        # Algoritmo de Edmonds para MST direcionada
│ └─ GabowMST.h          # Algoritmo de Gabow para MST direcionada
├─ src/
│ ├─ Graph.cpp           # Implementação da classe Grafo básico
│ ├─ WeightedGraph.cpp   # Implementação da classe Grafo ponderado
│ ├─ TarjanMST.cpp       # Implementação do algoritmo de Tarjan
│ ├─ EdmondsMST.cpp      # Implementação do algoritmo de Edmonds
│ ├─ GabowMST.cpp        # Implementação do algoritmo de Gabow
│ ├─ main.cpp            # Função principal do programa
│ ├─ tests/              # Arquivos de teste
│ └─ output/             # Arquivos executáveis gerados
├─ documentacao/         # Documentação do projeto
├─ output/               # Saída de compilação
└─ README.md             # Este arquivo
```

- **include/**: Contém todos os arquivos de cabeçalho (.h) do projeto, incluindo classes de grafos e algoritmos de MST.
- **src/**: Contém os arquivos de implementação (.cpp), testes e executáveis gerados.
- **documentacao/**: Contém a documentação técnica do projeto e especificações dos trabalhos.
- **output/**: Diretório para arquivos de saída da compilação.

---

## Como Executar o Código

Para compilar e executar o projeto, utilize o seguinte comando no terminal:

```
make
```

Escolha o teste que deseja rodar

```
./bin/main
```
```
./bin/test_tarjan
```
....

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
