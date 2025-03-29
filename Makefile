CC = g++
CFLAGS=-O3 -Iinclude -Wall -pedantic -pedantic-errors -g3 -std=gnu++20 $(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs) -lSDL2 -lSDL2_image

DEPFLAGS = -MMD -MP

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRCS))
DEPS :=	$(OBJS:.o=.d)

TARGET = run

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
