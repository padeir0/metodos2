#!/bin/bash
# Gera as figuras do relatório a partir do código do repositório.
# Uso (da raiz do repositório):  ./gerar_figuras.sh pasta_do_relatorio
# Precisa de gcc, python3, numpy e matplotlib.
set -e -o pipefail

ROOT=$(cd "$(dirname "$0")" && pwd)
OUT=$(realpath -m "${1:-relatorio}")
TMP=$(mktemp -d)
mkdir -p "$OUT"

# quadros da gota, da fenda dupla e do refletor (o resumo do refletor sai no stderr)
cd "$ROOT/exemplos"
./run wave_02.c | python3 ../dados/frames.py -o "$OUT/gota.png" -t 0 0.3 0.6 1.0
./run wave_04.c | python3 ../dados/frames.py -o "$OUT/fenda.png" -t 0 0.4 0.6 0.8 \
    --scale-x 0.55 1 --gamma 0.5
./run wave_05.c | python3 ../dados/frames.py -o "$OUT/refletor.png" -t 0 0.4 0.8 1.05

# mapa do máximo de |u| (o -I faz o "../lib/..." do wave_max.c achar a pasta lib)
cd "$ROOT"
gcc -std=c99 -O2 -I exemplos docs/14-refletor-maximo/wave_max.c -o "$TMP/wave_max" -lm
"$TMP/wave_max" > "$TMP/max.txt"
python3 docs/14-refletor-maximo/plot_max.py "$TMP/max.txt" "$OUT/maximo.png"

# figura que já existe nos slides
cp docs/04b-stencil/stencil.png "$OUT/stencil.png"

rm -rf "$TMP"
echo "figuras em $OUT:"; ls "$OUT"
