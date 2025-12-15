CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

SFML_PATH := lib/SFML
INCLUDES  := -Isrc -I$(SFML_PATH)/include
LIBS      := -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system


SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/app.exe


SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)