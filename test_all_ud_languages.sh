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

LIMA_SRC=.
CORPUS_PATH=lima_linguisticdata/disambiguisationMatrices/ 
TMP_DIR=$(mktemp -d -t test-lima-ud-XXXXXXXXXX)
UD_TOOLS=""
COMPARATOR=""

while [ "$1" != "" ]; do
  PARAM=`echo $1 | awk -F= '{print $1}'`
  VALUE=`echo $1 | awk -F= '{print $2}'`
  case $PARAM in
    --comparator)
      COMPARATOR=$VALUE
      ;;
    --ud_tools)
      UD_TOOLS=$VALUE
      ;;
  esac
  shift
done



for lang in `ls -d $LIMA_SRC/$CORPUS_PATH/* | grep -Eo '[a-z]+\.ud$'`
do
  echo "Lang == $lang"
  for corpus in `ls -d $LIMA_SRC/$CORPUS_PATH/$lang/* | grep -Eo 'UD_English.*' | sed 's/UD_//'`
  do
    echo "Corpus == $corpus"
    ./test_one_lang.sh --ud_corpus=$corpus \
                       --ud_path=$LIMA_SRC/$CORPUS_PATH/$lang/ \
                       --lima_lang=$lang \
                       --tmp_dir=$TMP_DIR \
                       --ud_tools=$UD_TOOLS \
                       --comparator=$COMPARATOR
    echo
  done
done
