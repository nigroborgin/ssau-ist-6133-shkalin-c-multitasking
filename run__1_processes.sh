#!/bin/bash

SRC_DIR="./src/1_processes"
OUT_DIR="./out/1_processes"

mkdir -p "$OUT_DIR"

cc "$SRC_DIR/hello.c" -o "$OUT_DIR/hello"
cc "$SRC_DIR/main.c" -o "$OUT_DIR/main"

"$OUT_DIR/main"