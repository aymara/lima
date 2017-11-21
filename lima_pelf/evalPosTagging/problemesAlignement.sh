#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

if (($#==0)); then 
    echo usage: problemesAlignement.sh langue method
fi

lang=$1
method=$2

temp_dir=`mktemp -d`

FILES="results.$lang.$method/*/aligned.log"

for file in $FILES 
do
    cat $file >> $temp_dir/tmpAligned;
done


gawk -F"\t" '{print " == "$2"\t"$3}' $temp_dir/tmpAligned | sort | uniq -c | sort -n -r > results.$lang.$method/bad-alignment-all.log;

gawk -F"\t" '{print " == "$2"\t"$3}' $temp_dir/tmpAligned | sort | uniq -c | sort -n -r | sed '/.*  == [^\/]*\t.*/d'> results.$lang.$method/bad-alignment-in-corpus.log;


#rm -Rf $temp_dir
