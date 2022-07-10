# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
