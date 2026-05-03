#!/usr/bin/env bash
# Lance tous les .bin du dossier sur l'émulateur, compare la valeur de a0
# au résultat attendu, et affiche un résumé.

set -uo pipefail
cd "$(dirname "$0")"

EMU="../../build/src/riscv_emu"

if [ ! -x "$EMU" ]; then
    echo "error: emulator not built at $EMU"
    echo "Run from project root: cmake -B build -G Ninja && cmake --build build"
    exit 1
fi

# (Re)compiler tous les .bin
echo "[build] make"
make --silent clean
make --silent || { echo "build failed"; exit 1; }

# Tableau des valeurs attendues pour chaque test (a0 en décimal)
declare -A EXPECTED=(
    [01_set_a0]=42
    [02_multiply]=35
    [03_fibonacci]=55
    [04_factorial]=120
    [05_max_of_three]=22
    [06_popcount]=16
)

echo ""
echo "[run] tests..."
echo ""

PASS=0
FAIL=0

for bin in *.bin; do
    name="${bin%.bin}"
    expected="${EXPECTED[$name]:-?}"

    # Capturer stderr (où l'émulateur affiche a0)
    output=$("$EMU" "$bin" 2>&1 || true)

    # Extraire la valeur décimale de a0 depuis "a0=0xXXXXXXXX (NNN)"
    a0=$(echo "$output" | grep -oE 'a0=0x[0-9A-Fa-f]+ \([0-9]+\)' | grep -oE '\([0-9]+\)' | tr -d '()')

    if [ -z "$a0" ]; then
        echo "[ERR ] $name → no a0 found in output"
        FAIL=$((FAIL+1))
        continue
    fi

    if [ "$a0" = "$expected" ]; then
        printf "[OK]   %-20s a0 = %-5s (expected %s)\n" "$name" "$a0" "$expected"
        PASS=$((PASS+1))
    else
        printf "[FAIL] %-20s a0 = %-5s (expected %s)\n" "$name" "$a0" "$expected"
        FAIL=$((FAIL+1))
    fi
done

echo ""
echo "Results: $PASS passed, $FAIL failed (out of $((PASS+FAIL)))"

[ $FAIL -eq 0 ]
