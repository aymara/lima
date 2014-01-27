#!/usr/bin/bash
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

CHARCHART=$1
PROPERTY_CODE=$2
SYMBOLIC_CODE=$3
DICTIONARY_INPUT=$4

export LC_ALL="C"

compile-dictionary --charChart=$CHARCHART --extractKeyList=keys $DICTIONARY_INPUT
sort -T . -u keys > keys.sorted
testDict16 --charSize=2 --listOfWords=keys.sorted --output=$DICTIONARY_INPUT.key.dat > output
testDict16 --charSize=2 --input=$DICTIONARY_INPUT.key.dat --spare --output=$DICTIONARY_INPUT.key.dat >> output

compile-dictionary --charChart=$CHARCHART --fsaKey=$DICTIONARY_INPUT.key.dat --propertyFile=$PROPERTY_CODE --symbolicCodes=$SYMBOLIC_CODE --output=$DICTIONARY_INPUT.data.dat $DICTIONARY_INPUT
