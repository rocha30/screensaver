#!/bin/sh
# scripts/run_benchmarks.sh
#
# Corre el modo --benchmark de ambos binarios (seq y par) para varias
# combinaciones de N (particulas) y numero de hilos, con REPS repeticiones
# por combinacion, y guarda todo en un CSV crudo para el Anexo 3
# (bitacora de pruebas). No calcula speedup/eficiencia aqui -- eso se
# deriva del CSV despues, para no mezclar datos medidos con calculos.
#
# Uso:
#   ./scripts/run_benchmarks.sh
#
# Requiere que bin/screensaver_seq y bin/screensaver_par ya esten
# compilados (make all).

set -eu

BIN_SEQ="./bin/screensaver_seq"
BIN_PAR="./bin/screensaver_par"
OUT_DIR="results"
OUT_FILE="$OUT_DIR/benchmark_results.csv"

N_VALUES="500 2000 8000 20000 50000"
THREAD_VALUES="1 2 4 8"
FRAMES=300
REPS=10

if [ ! -x "$BIN_SEQ" ] || [ ! -x "$BIN_PAR" ]; then
    echo "Error: falta compilar. Corre 'make all' antes de este script." >&2
    exit 1
fi

mkdir -p "$OUT_DIR"
echo "version,n,threads,frames,seconds,respawned,rep" > "$OUT_FILE"

total_runs=0

for n in $N_VALUES; do
    # --- version secuencial: una sola medicion por N (threads no aplica) ---
    for rep in $(seq 1 "$REPS"); do
        line=$("$BIN_SEQ" --n "$n" --frames "$FRAMES" --benchmark)
        # linea del binario: n,threads,frames,seconds,respawned (threads=0 en seq)
        echo "seq,$line,$rep" >> "$OUT_FILE"
        total_runs=$((total_runs + 1))
    done
    echo "seq  n=$n: $REPS mediciones listas"

    # --- version paralela: una medicion por N x threads ---
    for t in $THREAD_VALUES; do
        for rep in $(seq 1 "$REPS"); do
            line=$("$BIN_PAR" --n "$n" --frames "$FRAMES" --threads "$t" --benchmark)
            echo "par,$line,$rep" >> "$OUT_FILE"
            total_runs=$((total_runs + 1))
        done
        echo "par  n=$n threads=$t: $REPS mediciones listas"
    done
done

echo ""
echo "Listo: $total_runs corridas guardadas en $OUT_FILE"
