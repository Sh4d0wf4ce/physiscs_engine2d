# Kompilator i flagi
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./lib/SFML/include
LDFLAGS = -L./lib/SFML/lib
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Katalogi
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Pliki źródłowe i obiektowe
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Plik wykonywalny
TARGET = $(BIN_DIR)/physics_engine.exe

# Domyślna reguła
all: $(TARGET)

# Linkowanie
$(TARGET): $(OBJS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

# Kompilacja plików .cpp do .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Czyszczenie
clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)

# Uruchomienie
run: $(TARGET)
	@echo Uruchamianie programu...
	$(TARGET)

# Kopiowanie DLL (ważne dla Windows!)
copydll:
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@copy /Y "lib\SFML\bin\*.dll" "$(BIN_DIR)\"

# Build all-in-one
build: copydll $(TARGET)

# Debug build
debug: CXXFLAGS += -g -O0
debug: all

# Release build
release: CXXFLAGS += -O2 -DNDEBUG
release: all

.PHONY: all clean run debug release build copydll