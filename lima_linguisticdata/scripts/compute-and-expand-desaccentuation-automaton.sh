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
export PATH=$PATH:$LINGUISTIC_DATA_ROOT/scripts

extract-automaton-words.pl $2 tmp
sort -u tmp > words
desaccent --language=$1 words > words.desacc
filter_empty.pl words words.desacc words.filtered words.desacc.filtered
composedDict --language=$1 --dicoId=mainDictionary --keyFile=words.filtered | egrep "(linguistic|empty)" > words.dic
composedDict --language=$1 --dicoId=mainDictionary --keyFile=words.desacc.filtered | egrep "(linguistic|empty)" > words.desacc.dic
prepare-desaccentuate-conv.pl words.filtered words.desacc.filtered words.dic words.desacc.dic conv
expand-rules-for-desaccentuation.pl conv $2 $3
