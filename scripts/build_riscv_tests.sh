#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
TESTS_DIR="third_party/riscv-tests"

if [ ! -d "$TESTS_DIR" ]; then
    echo "error: $TESTS_DIR not found."
    echo "Run from project root:"
    echo "  git submodule add https://github.com/riscv-software-src/riscv-tests.git third_party/riscv-tests"
    echo "  git submodule update --init --recursive"
    exit 1
fi

cd "$TESTS_DIR"

if [ ! -f Makefile ]; then
    echo "[autoconf]"
    autoconf
    echo "[configure]"
    ./configure --prefix=/opt/riscv --with-xlen=32
fi

echo "[make isa]"
make isa -j"$(nproc)"

echo ""
echo "Tests built. Listing rv32ui-p-* binaries:"
ls isa/rv32ui-p-* 2>/dev/null | grep -v '\.dump$' | head -50
