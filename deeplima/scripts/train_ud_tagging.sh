#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

BUILD_TYPE=Release
UD_PATH=$1
UD_VERSION=$2
UD_CORPUS=$3
EMBD_FILE=$4
RNN_HIDDEN_SIZE=$5
OUTPUT_DIR=$6
LANG_CODE=$7

TASKS=upos,xpos,feats

EMBD_BASE_NAME=`echo $EMBD_FILE | grep -Eo "[^\/]+$" | sed -E "s/\.[a-z]+$//"`

MODEL_NAME_PREFIX=${OUTPUT_DIR}/${LANG_CODE}/${LANG_CODE}_${EMBD_BASE_NAME}_${TASKS}_w${RNN_HIDDEN_SIZE}

i=1
while [ -f ${MODEL_NAME_PREFIX}-${i}.pt ]; do i=$(( $i + 1 )) ; done

echo $i
MODEL_NAME_PREFIX=${MODEL_NAME_PREFIX}-${i}
touch ${MODEL_NAME_PREFIX}.pt

build/${BUILD_TYPE}/apps/deeplima-train-tag \
    -u ${UD_PATH}/ud-treebanks-v${UD_VERSION} \
    -c ${UD_CORPUS} \
    -e ${EMBD_FILE} \
    -n ${MODEL_NAME_PREFIX} \
    -w ${RNN_HIDDEN_SIZE} \
    --tasks ${TASKS},-Typo,-Foreign \
    --tag ud_version=${UD_VERSION} \
    --tag ud_corpus=${UD_CORPUS}

