# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
