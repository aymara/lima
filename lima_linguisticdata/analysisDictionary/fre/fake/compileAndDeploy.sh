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
#!/bin/bash

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
