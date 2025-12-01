# Compilador e flags
CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS   = 

# Pastas
SRC_DIR   = src
TEST_DIR  = src/tests
BUILD_DIR = build
BIN_DIR   = bin

# Arquivos fonte principais (sem testes)
SRC_FILES = \
  $(SRC_DIR)/Graph.cpp \
  $(SRC_DIR)/WeightedGraph.cpp \
  $(SRC_DIR)/EdmondsMST.cpp \
  $(SRC_DIR)/TarjanMST.cpp \
  $(SRC_DIR)/GabowMST.cpp \
  $(SRC_DIR)/KruskalMST.cpp \
  $(SRC_DIR)/ImageSegmentation.cpp \
  $(SRC_DIR)/main.cpp


# Gera a lista de .o correspondentes em build/
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# =========================
# Alvos principais
# =========================

# Compila tudo
all: main tests

# Programa principal com grafo ponderado (define WEIGHTED_GRAPH)
main: CXXFLAGS += -DWEIGHTED_GRAPH
main: $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(BIN_DIR)/main $(LDFLAGS)

# =========================
# Testes
# =========================

TEST_SOURCES = \
  $(TEST_DIR)/test_edmonds.cpp \
  $(TEST_DIR)/test_gabow.cpp \
  $(TEST_DIR)/test_tarjan.cpp \
  $(TEST_DIR)/test_kruskal.cpp \
  $(TEST_DIR)/test_tarjan_preprocessing.cpp \
  $(TEST_DIR)/test_tarjan_presets.cpp \

TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
TEST_BINS    = $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SOURCES))

tests: $(TEST_BINS)

# Cada teste linka com os objetos comuns dos algoritmos/grafo
$(BIN_DIR)/%: $(BUILD_DIR)/tests/%.o $(filter-out $(BUILD_DIR)/main.o,$(OBJ_FILES)) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# =========================
# Regras genéricas de compilação
# =========================

# Compilar fontes principais em build/*.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar fontes de teste em build/tests/*.o
$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)/tests
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =========================
# Criação de pastas
# =========================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/tests:
	mkdir -p $(BUILD_DIR)/tests

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# =========================
# Limpeza
# =========================

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all main tests clean
