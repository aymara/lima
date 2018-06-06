#!/bin/bash

# Usage : ./reBuildRawCorpus.sh tagged_corpus
#

if (($#!=2)); then 
    echo usage: rebuildRawCorpus.sh lang file
fi

lang=$1;
file=$2

case $lang in
    ara)
        cut -f1 $file | tr '\n' ' ' | \
            sed \
            -e "s/ \([\.\,)]\)/\\1/g" -e "s/\(['(]\) /\\1/g" \
            -e 's/ ال / ال/g' -e 's/ ه /ه /g' -e 's/ب /ب/g' \
            -e 's/ل ل /لل/g' -e 's/ ل / ل/g' -e 's/ ف / ف/g' \
            -e 's///g' -e 's/ ي /ي /g' -e 's/ ها /ها /g' -e 's/ ب / ب/g' \
            -e 's/ ني /ني /g' -e 's/ هم /هم /g' -e 's/ هن /هن /g' -e 's/ هما /هما /g' \
            -e 's/ نا /نا /g' -e 's/ كم /كم /g';;
    eng)
        cut -f1 $file | tr '\n' ' ' | \
            sed -e "s/ \([\.\,)]\)/\\1/g" -e "s/\(['(]\) /\\1/g" -e "s/ '\(m\|re\|ll\|d\|ve\) /'\\1 /g";;
    eng.ud)
        cut -f1 $file | tr '\n' ' ' | \
            sed -e "s/ \([\.\,)]\)/\\1/g" -e "s/\(['(]\) /\\1/g" -e "s/ '\(m\|re\|ll\|d\|ve\) /'\\1 /g";;
    fre | spa | ger | por)
        cut -f1 $file | tr '\n' ' ' | \
            sed -e "s/ \([\.\,)]\)/\\1/g" -e "s/\(['(]\) /\\1/g" -e "s/\([a-zA-Z]\-\) /\\1/g";;
esac

