#!/bin/bash

SRC_DIR="./src/lab_02"
OUT_DIR="./build/lab_02"
INCLUDE_DIR="./include/lab_02"
EXEC_NAME="lab_02.exe"

mkdir -p "$OUT_DIR"

gcc -I$INCLUDE_DIR \
   "$SRC_DIR/rocket.c" \
   -o "$OUT_DIR/$EXEC_NAME" \
   -lX11
   # -l - подключить библиотеку;
   # X11 - Xlib

"$OUT_DIR/$EXEC_NAME"