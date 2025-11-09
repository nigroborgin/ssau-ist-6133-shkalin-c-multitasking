#!/bin/bash

SRC_DIR="./src/2_threads"
OUT_DIR="./out/2_threads"

mkdir -p "$OUT_DIR"

cc "$SRC_DIR/main.c" -o "$OUT_DIR/main"

"$OUT_DIR/main"