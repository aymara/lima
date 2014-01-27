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
# 
# Shell script created by besancon on Fri Sep 10 2004 
# Version : $Id$ 
#

# add head to a file containing idiomatic expressions (format with ;)
# create a new file named [file_name].new

# This script is not intended to be used in the compilation process
# of the idiomatic expressions (it should be used only once to change 
# source file once and for all)

# heuristic process to add heads in idiomatic expressions: 
# for expressions that have type "nom" or "verbe", 
# choose as head the first term that have [S] or [V]

if (( $#==0 )); then echo "need a file name as argument"; exit; fi

export file=$1

gawk '{print NR "#" $0}' $file > $file.n
grep ";nom" $file.n > $file.n.nom
grep ";verbe" $file.n > $file.n.verbe

sed -e 's/\[V\]/\[V\]\&/g' $file.n.verbe > $file.n.verbe.head
sed -e 's/\[S\]/\[S\]\&/g' $file.n.nom > $file.n.nom.head

gawk ' \
BEGIN { FS="#"; } \
(ARGIND==1) { tab[$1]=$2; } \
(ARGIND==2) { tab[$1]=$2; } \
(ARGIND==3) { if ($1 in tab) { print tab[$1]; } else print $2; } \
' $file.n.nom.head $file.n.verbe.head $file.n > $file.new

# report
echo "created $file.new"
echo `grep -c "&" $file.new` "rules modified"

# check if there are rules with several heads
if [[ -n `grep "&[^;]*&" $file.new` ]]; then \
    echo `grep -c "&[^;]*&" $file.new` "errors:"; \
    grep -n "&[^;]*&" $file.new; \
fi;

/bin/rm $file.n $file.n.*
