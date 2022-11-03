# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

export LC_ALL="C"

CHARCHART=../../../scratch/LinguisticProcessings/fre/tokenizerAutomaton-fre.chars.tok

mkdir $MM_RESOURCES/LinguisticProcessings/fre/fake

for dico in corporate.xml user.xml; do
compile-dictionary --charChart=$CHARCHART --extractKeyList=keys $dico
sort -u keys > $dico.keys
../../../scripts/key2Fsa $dico.keys $dico-key.dat
compile-dictionary --charChart=$CHARCHART --fsaKey=$dico-key.dat --propertyFile=../code/code-fre.xml --symbolicCodes=../code/symbolicCode-fre.xml --output=$dico-dat.dat $dico
cp $dico*.dat $MM_RESOURCES/LinguisticProcessings/fre/fake
done;
