#!/bin/bash

SRC_DIR="./src/2_threads"
OUT_DIR="./out/2_threads"
FILE_NAME_1="system_v_threads"

mkdir -p "$OUT_DIR"

cc "$SRC_DIR/$FILE_NAME_1.c" -o "$OUT_DIR/$FILE_NAME_1"

"$OUT_DIR/$FILE_NAME_1"