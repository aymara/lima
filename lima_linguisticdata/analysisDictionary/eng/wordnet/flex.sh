#!/bin/bash
# 
# Shell script created by Romaric Besançon on Mon Apr 11 2016 
# Version : $Id$ 
#

# call the flex program to create inflected forms of the wordnet entries
# wn_*.txt contain the wordned entries
# final created file is formes-eng.dic

# flex tables are defined in the following files (taken from previous LIMA version):
# def.txt
# nom-table.txt nom-model.txt
# verbe-table.txt verbe-model.txt
# adjectif-table.txt adjectif-model.txt


#reformat wordnet entries and remove numeric forms
awk '($1 !~ /^[0-9]*$/) {print}' wn_*.txt | awk '{print "^W;;" $1 ";" $3 ";;" }' > mots-simples.txt

../../../scripts/flex.pl def.txt mots-simples.txt . formes-eng.txt exclude.txt

awk -F";" 'BEGIN {OFS="\t"} ($4) {print $1, $5, "" , $4} ($3) {print $1, $5, "", $3}' formes-eng.txt | sort -u --ignore-case > formes-eng.dic.1

# remove entries that are already in freeling dictionary (i.e. that add ambiguities to existing dictionary)

cat ../freeling/*.dic | awk '(ARGIND==1) {dic[tolower($1)]=1} (ARGIND==2 && !(tolower($1) in dic)) {print}' - formes-eng.dic.1 > formes-eng.dic
#cp formes-eng.dic.1 formes-eng.dic
