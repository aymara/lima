# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

cut -f 1 $1 > $1.words
$LINGUISTIC_DATA_ROOT/scripts/disambcorp2text.pl $1.words > $1.line
analyzeText --language=fre $1.line --dumper=textDumper 
cut -f 2 -d "|" $1.line.out | perl -pe "s/^\s+//"  > $1.line.out2
diff -ubB $1.words $1.line.out2
