#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

lang=$1


# cp $LIMA_SOURCES/SetCompilerFlags.cmake .
# cp $LIMA_SOURCES/SystemSpecificInformations.cmake .
# cp $LIMA_SOURCES/manageQt5.cmake .
# cp -R $LIMA_SOURCES/lima_linguisticdata .
# mkdir -p lima_linguisticdata/build/disambiguisationMatrices/${lang}
# # cp train.tfcv lima_linguisticdata/build/disambiguisationMatrices/${lang}/corpus_${lang}_merge.txt
# ./lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py train.tfcv > lima_linguisticdata/build/disambiguisationMatrices/${lang}/corpus_${lang}_merge.txt
# pushd lima_linguisticdata/
# mkdir -p dist
# cd build/
# cmake -DCMAKE_INSTALL_PREFIX=../dist -DCMAKE_BUILD_TYPE=Release ..
# cd  analysisDictionary/${lang}
# make install
# popd
