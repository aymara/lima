# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
