CXX ?= g++
CXXFLAGS += -Wall -Wextra -Werror -std=c++17
LDFLAGS += -lncurses -lmenu

SRC_DIR ?= src
BUILD_DIR ?= build
BIN_DIR ?= bin

SRC := $(SRC_DIR)/main.cc
OBJ := $(BUILD_DIR)/main.o

TARGET := $(BIN_DIR)/greeter

ifeq ($(CXX), clang++)
	LDFLAGS += -stdlib=libstdc++
endif

ifeq ($(DEBUG), yes)
	CXXFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

$(OBJ): $(SRC)
	$(CXX) -c $^ -o $@ $(CXXFLAGS)

clean:
	rm -f $(BUILD_DIR)/*.o $(BIN_DIR)/*
