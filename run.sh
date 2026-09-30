#!/bin/sh

set -e

SRC_DIR="src"
OUT_DIR="build"

mkdir -p "$OUT_DIR"

SRCS=$(find "$SRC_DIR" -type f -name '*.c')

gcc -std=c11 -O0 -Wall -Wextra $SRCS -lm -o "$OUT_DIR/program"

./build/program 20000 50