#   Copyright 2002-2013 CEA LIST
#    
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
#! /bin/bash

#
# Script de cr�ation de matrices de trigrammes et bigrammes � partir d'un corpus sous forme
# de fichier tabul�.
# Ce script peut optionnellement convertir les cat�gories du corpus d'origine selon une table
# de conversion.
#

if [ ! $1 ];
then
echo USAGE : create_matrices_from_corpus.sh CorpusFile [CategorieConvertionTable]
echo   Results matrices are in directory 'matrice'
fi;

CORPUS=$1

mkdir matrices

echo extract succession categories from $CORPUS

gawk -F"	" '{ print $2 }' $CORPUS >  succession_categs.txt
if [ $2 ];
then
CONVERTION=$2
echo convert categories tag using $CONVERTION
disamb_matrices_retag.pl succession_categs.txt succession_categs_retag.txt $CONVERTION
mv succession_categs_retag.txt succession_categs.txt
fi;

echo extract trigrams and bigrams
disamb_matrices_extract.pl succession_categs.txt
echo normalize matrices
disamb_matrices_normalize.pl trigramsend.txt matrices/trigramMatrix.dat
mv bigramsend.txt matrices/bigramMatrix.dat
rm -f tri trigramsdef.txt trigramsend.txt trigramssort.txt bi bigramsdef.txt bigramssort.txt succession_categs_retag.txt succession_categs.txt
