# Makefile - Proyecto 1 Screensaver Paralelo (Computacion Paralela y Distribuida, UVG)
#
# Etapa 2 del proyecto: version secuencial y version paralela (OpenMP),
# ambas comparten src/common/ y solo difieren en src/seq|par/particle_update.c
# y su main.c.
#   bin/screensaver_seq  -> version secuencial
#   bin/screensaver_par  -> version paralela (OpenMP)
#
# Uso:
#   make            compila ambas versiones (seq y par)
#   make seq        compila solo la version secuencial
#   make par        compila solo la version paralela
#   make run        compila y ejecuta la secuencial (ARGS="--n 500")
#   make run-par    compila y ejecuta la paralela (ARGS="--n 500 --threads 4")
#   make clean      elimina binarios y objetos
#   make deps       imprime que instalar segun el sistema operativo

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

# --- OpenMP: en Linux, gcc/clang normalmente aceptan -fopenmp directo.
#     En macOS, el "cc" de Apple (clang) no trae OpenMP integrado y hace
#     falta libomp de Homebrew (brew install libomp); se le pasa el flag
#     de preprocesador y se enlaza contra libomp explicitamente. ---
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  LIBOMP_PREFIX := $(shell brew --prefix libomp 2>/dev/null)
  ifneq ($(strip $(LIBOMP_PREFIX)),)
    OMP_CFLAGS := -Xpreprocessor -fopenmp -I$(LIBOMP_PREFIX)/include
    OMP_LIBS   := -L$(LIBOMP_PREFIX)/lib -lomp
  else
    OMP_CFLAGS := -Xpreprocessor -fopenmp
    OMP_LIBS   := -lomp
  endif
else
  OMP_CFLAGS := -fopenmp
  OMP_LIBS   := -fopenmp
endif

MATH_LIBS := -lm
INCLUDES  := -Iinclude

COMMON_SRC := $(wildcard src/common/*.c)
COMMON_OBJ := $(patsubst src/common/%.c,$(BUILD_DIR)/common/%.o,$(COMMON_SRC))

SEQ_SRC := $(wildcard src/seq/*.c)
SEQ_OBJ := $(patsubst src/seq/%.c,$(BUILD_DIR)/seq/%.o,$(SEQ_SRC))

PAR_SRC := $(wildcard src/par/*.c)
PAR_OBJ := $(patsubst src/par/%.c,$(BUILD_DIR)/par/%.o,$(PAR_SRC))
# El common tambien necesita compilarse con los flags de OpenMP para la
# version paralela (particles_update corre dentro de una region paralela),
# asi que usa su propio set de objetos .o separado del de la version seq.
COMMON_PAR_OBJ := $(patsubst src/common/%.c,$(BUILD_DIR)/common_par/%.o,$(COMMON_SRC))

.PHONY: all seq par run run-par clean dirs deps

all: seq par

seq: dirs $(BIN_DIR)/screensaver_seq

par: dirs $(BIN_DIR)/screensaver_par

dirs:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)/common $(BUILD_DIR)/common_par $(BUILD_DIR)/seq $(BUILD_DIR)/par

$(BIN_DIR)/screensaver_seq: $(COMMON_OBJ) $(SEQ_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(SDL_LIBS) $(MATH_LIBS)

$(BIN_DIR)/screensaver_par: $(COMMON_PAR_OBJ) $(PAR_OBJ)
	$(CC) $(CFLAGS) $(OMP_CFLAGS) $^ -o $@ $(SDL_LIBS) $(MATH_LIBS) $(OMP_LIBS)

$(BUILD_DIR)/common/%.o: src/common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/common_par/%.o: src/common/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OMP_CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/seq/%.o: src/seq/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/par/%.o: src/par/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OMP_CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

run: seq
	./$(BIN_DIR)/screensaver_seq $(ARGS)

run-par: par
	./$(BIN_DIR)/screensaver_par $(ARGS)

deps:
	@echo "macOS (Homebrew):  brew install sdl2 pkg-config libomp"
	@echo "Debian/Ubuntu:     sudo apt install libsdl2-dev pkg-config gcc"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
