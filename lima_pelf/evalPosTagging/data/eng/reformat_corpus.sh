#!/bin/bash
# 
# Shell script created by Romaric Besancon on Wed Mar 17 2010 
# Version : $Id$ 
#


awk 'BEGIN {OFS="\t"} (NF==3) {print $1,$3} (NF==2 && $1==".") { print $1,$2; } (NF==2 && $1=="YS") {print ";",$2}' orig_corpus_tabbed > orig_corpus_tabbed_2
