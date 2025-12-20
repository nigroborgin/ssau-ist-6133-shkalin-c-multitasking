#!/bin/bash

SRC_DIR="./src/lab_01"
OUT_DIR="./build/lab_01"
INCLUDE_DIR="./include/lab_01"
EXEC_NAME="lab_01.exe"

mkdir -p "$OUT_DIR"

gcc -I$INCLUDE_DIR \
   "$SRC_DIR/main.c" \
   "$SRC_DIR/arithmetic_processes.c" \
   -o "$OUT_DIR/$EXEC_NAME" \
   -lpthread -lm
   # -l - подключить библиотеку;
   # pthread - POSIX-потоки, m - libm (математика)

"$OUT_DIR/$EXEC_NAME"