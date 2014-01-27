#!/bin/bash

if (($#==0)); then 
    echo usage: problemesAlignement.sh langue method
fi

lang=$1
method=$2

temp_dir=/tmp/"$(awk -v p=$$ 'BEGIN { srand(); s = rand(); sub(/^0./, "", s); printf("%X_%X", p, s) }')"
mkdir -m 700 "$temp_dir" || { echo '!! unable to create a tempdir' >&2; exit 1; }

FILES="results.$lang.$method/*/aligned.log"

for file in $FILES 
do
    cat $file >> $temp_dir/tmpAligned;
done

gawk -F"\t" '{print " == "$2"\t"$3}' $temp_dir/tmpAligned | sort | uniq -c | sort -n -r > results.$lang.$method/bad-alignment-all.log;

gawk -F"\t" '{print " == "$2"\t"$3}' $temp_dir/tmpAligned | sort | uniq -c | sort -n -r | sed '/.*  == [^\/]*\t.*/d'> results.$lang.$method/bad-alignment-in-corpus.log;


#rm -Rf $temp_dir