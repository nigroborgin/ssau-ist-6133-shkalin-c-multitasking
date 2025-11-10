#!/bin/bash

SRC_DIR="./src/1_processes"
OUT_DIR="./out/1_processes"
FILE_NAME_1="hello"
FILE_NAME_2="main"

mkdir -p "$OUT_DIR"

cc "$SRC_DIR/$FILE_NAME_1.c" -o "$OUT_DIR/$FILE_NAME_1"
cc "$SRC_DIR/$FILE_NAME_2.c" -o "$OUT_DIR/$FILE_NAME_2"

"$OUT_DIR/$FILE_NAME_2"