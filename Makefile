# --- USTAWIENIA PODSTAWOWE ---
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

# Foldery projektu
SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin
IMGUI_PATH := lib/imgui
JSON_PATH  := lib/json

# Źródła
SOURCES       := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS       := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
IMGUI_SOURCES := $(wildcard $(IMGUI_PATH)/*.cpp)
IMGUI_OBJECTS := $(patsubst $(IMGUI_PATH)/%.cpp, $(OBJ_DIR)/%.o, $(IMGUI_SOURCES))
ALL_OBJECTS   := $(OBJECTS) $(IMGUI_OBJECTS)

# --- WYKRYWANIE SYSTEMU OPERACYJNEGO ---
ifeq ($(OS),Windows_NT)
    # === KONFIGURACJA WINDOWS ===
    TARGET_NAME := app.exe
    
    # Ścieżki do Twoich bibliotek w folderze lib/
    SFML_PATH := lib/SFML
    INCLUDES  := -Isrc -I$(SFML_PATH)/include -I$(IMGUI_PATH) -I$(JSON_PATH)
    LIBS      := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32

    # Komendy systemowe (Windows)
    MKDIR     := mkdir
    RM        := del /Q /S
    FIX_PATH  = $(subst /,\,$1)

    # Kopiowanie DLL (Tylko na Windowsie)
    SFML_DLLS := $(wildcard $(SFML_PATH)/bin/*.dll)
    COPY_CMD  = xcopy /y /i "$(call FIX_PATH,$(SFML_PATH)/bin/*.dll)" "$(call FIX_PATH,$(BIN_DIR))" > nul 2>&1
else
    # === KONFIGURACJA LINUX / MACOS ===
    TARGET_NAME := app
    
    # Na Linuxie używamy systemowego SFML (zainstalowanego przez apt/pacman)
    # Nie linkujemy do lib/SFML, bo tam są pliki windowsowe!
    INCLUDES  := -Isrc -I$(IMGUI_PATH) -I$(JSON_PATH)
    LIBS      := -lsfml-graphics -lsfml-window -lsfml-system -lGL

    # Komendy systemowe (Unix)
    MKDIR     := mkdir -p
    RM        := rm -rf
    
    # Na Linuxie nie kopiujemy DLL-ek
    COPY_CMD  = echo "Linux detected: Skipping DLL copy."
endif

TARGET := $(BIN_DIR)/$(TARGET_NAME)

# --- REGUŁY ---
.PHONY: all clean docs run

all: $(TARGET)

# Linkowanie
$(TARGET): $(ALL_OBJECTS) | $(BIN_DIR)
	$(CXX) $(ALL_OBJECTS) -o $@ $(LIBS)
	@$(COPY_CMD)
	@echo "Build successful! Run with: ./$(BIN_DIR)/$(TARGET_NAME)"

# Kompilacja źródeł projektu
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Kompilacja ImGui
$(OBJ_DIR)/%.o: $(IMGUI_PATH)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Tworzenie folderów
$(OBJ_DIR):
	@$(MKDIR) $(call FIX_PATH,$(OBJ_DIR)) 2> nul || true

$(BIN_DIR):
	@$(MKDIR) $(call FIX_PATH,$(BIN_DIR)) 2> nul || true

# Generowanie dokumentacji
docs:
	doxygen Doxyfile

# Czyszczenie
clean:
ifeq ($(OS),Windows_NT)
	-@if exist $(OBJ_DIR) $(RM) $(call FIX_PATH,$(OBJ_DIR)\*.o)
	-@if exist $(BIN_DIR) $(RM) $(call FIX_PATH,$(BIN_DIR)\$(TARGET_NAME))
else
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
endif
	@echo "Cleaned up."

# Uruchamianie
run: all
	./$(TARGET)