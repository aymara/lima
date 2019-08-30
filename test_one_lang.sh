#!/bin/bash

#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

COMPARATOR=conll18_ud_eval.py
UD_PATH=""
UD_CORPUS=""
LIMA_LANG=""
TMP_DIR=""
DIR_TO_DELETE=""
SAVE_DIR=""
UD_TOOLS=""

while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{print $2}'`
  case $PARAM in
    --comparator)
      COMPARATOR=$VALUE
      ;;
    --ud_path)
      UD_PATH=$VALUE
      ;;
    --ud_corpus)
      UD_CORPUS=$VALUE
      ;;
    --lima_lang)
      LIMA_LANG=$VALUE
      ;;
    --tmp_dir)
      TMP_DIR=$VALUE
      ;;
    --save_dir)
      SAVE_DIR=$VALUE
      ;;
    --ud_tools)
      UD_TOOLS=$VALUE
      ;;
  esac
  shift
done

UD_BASE_NAME=`ls $UD_PATH/UD_$UD_CORPUS/*.conllu | head -n 1 | grep -Eo '[^\/]+\.conllu' | grep -Eo '^[a-z\_]+'`
#echo Guessed corpus base name is \"$UD_BASE_NAME\"

if [ "$TMP_DIR" == "" ]; then
  #echo TMP_DIR isn\'t specified
  TMP_DIR=$(mktemp -d -t test-lima-ud-XXXXXXXXXX)
  DIR_TO_DELETE=$TMP_DIR
fi
mkdir -p $TMP_DIR/$UD_CORPUS

for t in train dev test
do
  SRC_FILE=`ls $UD_PATH/UD_$UD_CORPUS/*.conllu | grep $t`
  if [ "$SRC_FILE" == "" ]; then
    continue
  fi

  echo
  echo $UD_CORPUS "-" $t

  perl $UD_TOOLS/conllu_to_text.pl < $SRC_FILE > $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.txt

  time analyzeText -l $LIMA_LANG $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.txt \
                      >  $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t-pred.conllu \
                      2> $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.lima-errors
  ERROR_CODE=$?
  if [ $ERROR_CODE -ne 0 ]; then
    echo analyzeText failed with code $ERROR_CODE TMP_DIR is $TMP_DIR
    exit 1
  fi

  python $COMPARATOR -v $UD_PATH/UD_$UD_CORPUS/$UD_BASE_NAME-ud-$t.conllu \
                        $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t-pred.conllu \
                        >  $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.scores \
                        2> $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.comp-errors
  ERROR_CODE=$?
  if [ $ERROR_CODE -ne 0 ]; then
    echo Comparator failed with code $ERROR_CODE TMP_DIR is $TMP_DIR
    exit 1
  fi

  if [ "$t" == "train" ]; then
    cat $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.scores | grep -E 'Metric|-----'
  fi
  cat $TMP_DIR/$UD_CORPUS/$UD_BASE_NAME-ud-$t.scores | grep -E 'Tokens|Sentences'

done

if [ "$SAVE_DIR" != "" ]; then
  cp $TMP_DIR/$UD_CORPUS/*.conllu $SAVE_DIR/
  cp $TMP_DIR/$UD_CORPUS/*.scores $SAVE_DIR/
fi

if [ "$DIR_TO_DELETE" != "" ]; then
  #echo Deleting temp dir $DIR_TO_DELETE
  rm -rf $DIR_TO_DELETE
fi
