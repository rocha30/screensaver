# Makefile - Proyecto 1 Screensaver Paralelo (Computacion Paralela y Distribuida, UVG)
#
# Etapa 1 del proyecto: solo existe la version secuencial.
#   bin/screensaver_seq  -> version secuencial
#
# (La Etapa 2 agrega bin/screensaver_par con OpenMP, extendiendo este
# mismo Makefile con un target "par" que comparte src/common/.)
#
# Uso:
#   make            compila la version secuencial
#   make run        compila y ejecuta (ARGS="--n 500")
#   make clean      elimina binarios y objetos

CC     := cc
STD    := -std=c11
WARN   := -Wall -Wextra
OPT    := -O2
CFLAGS := $(STD) $(WARN) $(OPT)

BIN_DIR   := bin
BUILD_DIR := build

# --- SDL2: usa pkg-config o sdl2-config; si ninguno esta disponible, hay
#     que exportar SDL_CFLAGS/SDL_LIBS a mano antes de correr make. ---
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL_LIBS   := $(shell pkg-config --libs sdl2 2>/dev/null)
ifeq ($(strip $(SDL_CFLAGS)$(SDL_LIBS)),)
  SDL_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
  SDL_LIBS   := $(shell sdl2-config --libs 2>/dev/null)
endif

MATH_LIBS := -lm
INCLUDES  := -Iinclude

COMMON_SRC := $(wildcard src/common/*.c)
COMMON_OBJ := $(patsubst src/common/%.c,$(BUILD_DIR)/common/%.o,$(COMMON_SRC))

SEQ_SRC := $(wildcard src/seq/*.c)
SEQ_OBJ := $(patsubst src/seq/%.c,$(BUILD_DIR)/seq/%.o,$(SEQ_SRC))

.PHONY: all run clean dirs deps

all: dirs $(BIN_DIR)/screensaver_seq

dirs:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)/common $(BUILD_DIR)/seq

$(BIN_DIR)/screensaver_seq: $(COMMON_OBJ) $(SEQ_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(SDL_LIBS) $(MATH_LIBS)

$(BUILD_DIR)/common/%.o: src/common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/seq/%.o: src/seq/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

run: all
	./$(BIN_DIR)/screensaver_seq $(ARGS)

deps:
	@echo "macOS (Homebrew):  brew install sdl2 pkg-config"
	@echo "Debian/Ubuntu:     sudo apt install libsdl2-dev pkg-config gcc"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
