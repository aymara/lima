#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

INPUT_FILE=$1
OUTPUT_DIR=$2
TOK_MODEL=$3
LNG=$4

xzcat $INPUT_FILE | deeplima-eigen --tok-model=$TOK_MODEL --thread 4 --output-format=horizontal > $OUTPUT_DIR/${LNG}.tok.txt
