#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

lang=$1

cp $LIMA_SOURCES/SystemSpecificInformations.cmake .
cp -R $LIMA_SOURCES/lima_linguisticdata .
mkdir -p lima_linguisticdata/build/disambiguisationMatrices/${lang}
cp 90pc.tfcv lima_linguisticdata/build/disambiguisationMatrices/${lang}/corpus_${lang}_merge.txt
pushd lima_linguisticdata/
mkdir -p dist
cd build/
cmake -DCMAKE_INSTALL_PREFIX=../dist -DCMAKE_BUILD_TYPE=Release ..
cd  analysisDictionary/${lang}
make install
popd
