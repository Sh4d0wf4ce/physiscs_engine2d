CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

SFML_PATH := lib/SFML
IMGUI_PATH := lib/imgui

INCLUDES  := -Isrc -I$(SFML_PATH)/include -I$(IMGUI_PATH)
LIBS      := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system -lopengl32

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/app.exe

SFML_DLL_DIR := $(SFML_PATH)/bin
SFML_DLLS    := $(notdir $(wildcard $(SFML_DLL_DIR)/*.dll))
BIN_DLLS     := $(addprefix $(BIN_DIR)/,$(SFML_DLLS))

SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

IMGUI_SOURCES := $(wildcard $(IMGUI_PATH)/*.cpp)
IMGUI_OBJECTS := $(patsubst $(IMGUI_PATH)/%.cpp, $(OBJ_DIR)/%.o, $(IMGUI_SOURCES))

ALL_OBJECTS := $(OBJECTS) $(IMGUI_OBJECTS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(ALL_OBJECTS) $(BIN_DLLS) | $(BIN_DIR)
	$(CXX) $(ALL_OBJECTS) -o $@ $(LIBS)

$(BIN_DIR)/%.dll: $(SFML_DLL_DIR)/%.dll | $(BIN_DIR)
	powershell -Command "if (-not (Test-Path '$(BIN_DIR)/$(@F)')) { Copy-Item '$<' '$(BIN_DIR)' }"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(IMGUI_PATH)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)