#!/bin/bash

INPUT_FILE=$1
OUTPUT_DIR=$2
TOK_MODEL=$3
LNG=$4

nice xzcat $INPUT_FILE | nice deeplima-eigen --tok-model=$TOK_MODEL --output-format=horizontal > $OUTPUT_DIR/${LNG}.tok.txt
