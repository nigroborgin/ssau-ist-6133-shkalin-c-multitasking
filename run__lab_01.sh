#!/bin/bash

SRC_DIR="./src/lab_01"
OUT_DIR="./out/lab_01"

mkdir -p "$OUT_DIR"

cc -I"./include" \
   "$SRC_DIR/main.c" \
   "$SRC_DIR/arithmetic_processes.c" \
   -o "$OUT_DIR/lab_01.exe" \
   -lm

"$OUT_DIR/lab_01.exe"