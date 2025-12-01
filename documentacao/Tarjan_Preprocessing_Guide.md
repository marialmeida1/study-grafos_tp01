# Pré-processamento Melhorado para Algoritmo de Tarjan

Este documento descreve as melhorias implementadas no algoritmo de Tarjan para segmentação de imagens, incluindo técnicas avançadas de pré-processamento que resultam em segmentações de maior qualidade.

## Visão Geral das Melhorias

### 1. **Suavização Gaussiana**
- **Propósito**: Reduzir ruído na imagem antes da construção do grafo
- **Parâmetros**: `gaussianSigma` (0.5 - 2.0)
- **Benefícios**: Melhora a coerência de regiões similares, reduz segmentação excessiva

### 2. **Normalização de Contraste**
- **Propósito**: Expandir a faixa dinâmica de cada canal de cor
- **Benefícios**: Melhora a separação entre diferentes regiões da imagem

### 3. **Filtro Bilateral**
- **Propósito**: Suavizar a imagem preservando bordas importantes
- **Parâmetros**: `bilateralSigmaColor`, `bilateralSigmaSpace`
- **Benefícios**: Reduz ruído mantendo detalhes estruturais importantes

### 4. **Peso Baseado em Gradientes**
- **Propósito**: Dar mais importância às arestas que cruzam bordas significativas
- **Parâmetros**: `edgeWeight` (1.0 - 3.0)
- **Benefícios**: Melhora a detecção de fronteiras entre objetos

### 5. **Análise de Textura Local**
- **Propósito**: Considerar variância de textura na função de dissimilaridade
- **Parâmetros**: `textureWeight` (0.1 - 0.6)
- **Benefícios**: Ajuda a separar regiões com texturas diferentes mas cores similares

### 6. **Conversão de Espaço de Cores**
- **Propósito**: Usar espaço LAB que é mais perceptualmente uniforme que RGB
- **Benefícios**: Melhora a qualidade da segmentação baseada na percepção humana

## Presets Disponíveis

### 1. **Natural Images** (`naturalImages()`)
Otimizado para fotografias e imagens naturais com ruído:
```cpp
enableGaussianBlur = true (sigma = 1.2)
enableBilateralFilter = true
enableEdgeWeighting = true (weight = 1.8)
textureWeight = 0.4
```

### 2. **Medical Images** (`medicalImages()`)
Para imagens médicas que requerem alta precisão:
```cpp
enableContrastNorm = true
enableBilateralFilter = true
enableEdgeWeighting = true (weight = 3.0)
textureWeight = 0.6
```

### 3. **Synthetic Images** (`syntheticImages()`)
Para imagens sintéticas, logos, gráficos:
```cpp
enableColorSpaceConv = true
enableContrastNorm = true
enableEdgeWeighting = true (weight = 2.5)
textureWeight = 0.2
```

### 4. **Balanced** (`balanced()`)
Configuração equilibrada para uso geral:
```cpp
enableGaussianBlur = true (sigma = 1.0)
enableContrastNorm = true
enableEdgeWeighting = true (weight = 2.0)
textureWeight = 0.3
```

### 5. **Fast** (`fast()`)
Processamento rápido com qualidade aceitável:
```cpp
enableContrastNorm = true
enableEdgeWeighting = true (weight = 1.5)
textureWeight = 0.1
```

### 6. **Premium** (`premium()`)
Máxima qualidade (processamento mais lento):
```cpp
enableGaussianBlur = true (sigma = 0.8)
enableContrastNorm = true
enableBilateralFilter = true
enableColorSpaceConv = true
enableEdgeWeighting = true (weight = 2.2)
textureWeight = 0.35
```

## Como Usar

### 1. **Através do main.cpp melhorado**
```bash
# Executar com pré-processamento
./bin/main img/input.jpg --preprocess

# Especificar threshold personalizado
./bin/main img/input.jpg --preprocess --threshold 40.0
```

### 2. **Usando diretamente a API**
```cpp
// Configurar opções de pré-processamento
PreprocessingOptions options;
options.enableGaussianBlur = true;
options.gaussianSigma = 1.0;
options.enableEdgeWeighting = true;
options.edgeWeight = 2.0;

// Executar segmentação com pré-processamento
ImageSegmentation::runSegmentationWithPreprocessing(
    "input.jpg", "output.png", 
    Strategy::TARJAN_MSA, 50.0, options);
```

### 3. **Executando testes de comparação**
```bash
# Teste completo com diferentes técnicas
make && ./bin/test_tarjan_preprocessing

# Teste com todos os presets
make && ./bin/test_tarjan_presets
```

## Resultados Esperados

As melhorias implementadas resultam em:

1. **Redução de Super-segmentação**: Menos fragmentação de regiões uniformes
2. **Melhor Detecção de Bordas**: Fronteiras mais precisas entre objetos
3. **Robustez ao Ruído**: Menos sensibilidade a ruído na imagem
4. **Segmentação Perceptual**: Resultados mais próximos à percepção humana
5. **Flexibilidade**: Diferentes presets para diferentes tipos de imagem

## Parâmetros Recomendados

| Tipo de Imagem | Preset Recomendado | Threshold Sugerido |
|----------------|-------------------|-------------------|
| Fotos naturais | `balanced` ou `naturalImages` | 40.0 - 60.0 |
| Imagens médicas | `medicalImages` | 30.0 - 50.0 |
| Logos/Gráficos | `syntheticImages` | 50.0 - 80.0 |
| Máxima qualidade | `premium` | 35.0 - 55.0 |
| Processamento rápido | `fast` | 45.0 - 65.0 |

## Complexidade Computacional

- **Suavização Gaussiana**: O(WHr²) onde r é o raio do filtro
- **Filtro Bilateral**: O(WHr²) - mais caro computacionalmente
- **Gradientes**: O(WH) - processamento linear
- **Textura**: O(WHr²) onde r é o raio da janela local

O pré-processamento adiciona overhead, mas resulta em segmentações significativamente melhores, especialmente para o algoritmo de Tarjan que é sensível à qualidade das arestas do grafo.
