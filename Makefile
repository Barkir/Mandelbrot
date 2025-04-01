CC = g++
TARGET = run

CFLAGS_RELEASE = -mavx -mavx2 -mfma -mavx512vl -O2 -Iinclude -Wall -pedantic -pedantic-errors -g3 -std=gnu++20 -DNDEBUG $(shell sdl2-config --cflags)
CFLAGS_DEBUG = -mavx -mavx2 -mfma -mavx512vl -O2 -Iinclude -Wall -pedantic -pedantic-errors -g3 -std=gnu++20 -fsanitize=address $(shell sdl2-config --cflags)
LDFLAGS=$(shell sdl2-config --libs) -lSDL2 -lSDL2_image
DEPFLAGS = -MMD -MP

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRCS))
DEPS :=	$(OBJS:.o=.d)

all: release

ifeq ($(MAKECMDGOALS), debug)
	CFLAGS = $(CFLAGS_DEBUG)
	LDFLAGS += -fsanitize=address
else ifeq ($(MAKECMDGOALS), release)
	CFLAGS = $(CFLAGS_RELEASE)
endif

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

release: CFLAGS = $(CFLAGS_RELEASE)
release: $(TARGET)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
