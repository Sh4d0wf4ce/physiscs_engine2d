# --- KONFIGURACJA WINDOWS ---
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

# Ścieżki do folderów
SRC_DIR    := src
OBJ_DIR    := obj
BIN_DIR    := bin
SFML_PATH  := lib/SFML
IMGUI_PATH := lib/imgui
JSON_PATH  := lib/json

# Nazwa pliku wynikowego
TARGET_NAME := app.exe
TARGET      := $(BIN_DIR)/$(TARGET_NAME)

# Pliki źródłowe
SOURCES       := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS       := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
IMGUI_SOURCES := $(wildcard $(IMGUI_PATH)/*.cpp)
IMGUI_OBJECTS := $(patsubst $(IMGUI_PATH)/%.cpp, $(OBJ_DIR)/%.o, $(IMGUI_SOURCES))
ALL_OBJECTS   := $(OBJECTS) $(IMGUI_OBJECTS)

# Biblioteki i Nagłówki
INCLUDES := -Isrc -I$(SFML_PATH)/include -I$(IMGUI_PATH) -I$(JSON_PATH)
LIBS     := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32

# Komendy systemowe (Windows)
# fix_path zamienia / na \ (wymagane przez niektóre komendy windows cmd)
FIX_PATH = $(subst /,\,$1)
RM       = del /Q /S

# Kopiowanie DLL
COPY_CMD = xcopy /y /i "$(call FIX_PATH,$(SFML_PATH)/bin/*.dll)" "$(call FIX_PATH,$(BIN_DIR))" > nul 2>&1

.PHONY: all clean docs run

all: $(TARGET)

# Linkowanie
$(TARGET): $(ALL_OBJECTS) | $(BIN_DIR)
	$(CXX) $(ALL_OBJECTS) -o $@ $(LIBS)
	@echo Kopiowanie bibliotek DLL...
	@$(COPY_CMD)
	@echo Build successful! Run: $(TARGET)

# Kompilacja źródeł projektu
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Kompilacja ImGui
$(OBJ_DIR)/%.o: $(IMGUI_PATH)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Tworzenie folderów (bezpieczne dla Windows)
$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# Generowanie dokumentacji
docs:
	doxygen Doxyfile

# Czyszczenie
clean:
	@if exist $(OBJ_DIR) $(RM) $(call FIX_PATH,$(OBJ_DIR)\*.o)
	@if exist $(BIN_DIR) $(RM) $(call FIX_PATH,$(BIN_DIR)\$(TARGET_NAME))
	@echo Cleaned up.

run: all
	./$(TARGET)