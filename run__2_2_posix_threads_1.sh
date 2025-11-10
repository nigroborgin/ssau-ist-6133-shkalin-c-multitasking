#!/bin/bash

SRC_DIR="./src/2_threads"
OUT_DIR="./out/2_threads"
FILE_NAME_1="posix_threads_1"

mkdir -p "$OUT_DIR"
cc "$SRC_DIR/$FILE_NAME_1.c" -pthread -o "$OUT_DIR/$FILE_NAME_1"

"$OUT_DIR/$FILE_NAME_1"