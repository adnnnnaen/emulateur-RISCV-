#!/usr/bin/env bash
set -uo pipefail

cd "$(dirname "$0")/.."

EMU="./build/src/riscv_emu"
TESTS_DIR="third_party/riscv-tests/isa"

if [ ! -x "$EMU" ]; then
    echo "error: $EMU not built. Run:"
    echo "  cmake -B build -G Ninja && cmake --build build"
    exit 1
fi

# Trouver tous les binaires rv32ui-p-* (sans extension .dump)
TESTS=$(ls "$TESTS_DIR"/rv32ui-p-* 2>/dev/null | grep -v '\.dump$' | sort)

if [ -z "$TESTS" ]; then
    echo "error: no rv32ui-p-* tests found. Run: ./scripts/build_riscv_tests.sh"
    exit 1
fi

PASS=0
FAIL=0
FAILED_TESTS=""

for elf in $TESTS; do
    name=$(basename "$elf")
    bin=$(mktemp /tmp/rv32test.XXXXXX.bin)
    riscv64-unknown-elf-objcopy -O binary "$elf" "$bin"

    if "$EMU" "$bin" >/dev/null 2>&1; then
        echo "[OK]   $name"
        PASS=$((PASS + 1))
    else
        echo "[FAIL] $name"
        FAIL=$((FAIL + 1))
        FAILED_TESTS="$FAILED_TESTS $name"
    fi

    rm -f "$bin"
done

echo ""
echo "Results: $PASS passed, $FAIL failed (out of $((PASS + FAIL)))"

if [ $FAIL -gt 0 ]; then
    echo "Failed tests:$FAILED_TESTS"
    exit 1
fi
