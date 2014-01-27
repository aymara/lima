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

cut -f 1 $1 > $1.words
$LINGUISTIC_DATA_ROOT/scripts/disambcorp2text.pl $1.words > $1.line
analyzeText --language=fre $1.line --dumper=textDumper 
cut -f 2 -d "|" $1.line.out | perl -pe "s/^\s+//"  > $1.line.out2
diff -ubB $1.words $1.line.out2
