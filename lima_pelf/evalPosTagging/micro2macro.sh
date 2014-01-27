#!/bin/bash
# 
# Shell script created by Romaric Besancon on Thu Mar 18 2010 
# Version : $Id$ 
#

if (($#!=2)); then 
    echo usage: micro2macro.sh lang file
fi

lang=$1;

case $lang in
    ger) sed -e 's/L_GE_\([^_>]*\)[^>]*>/L_GE_\1>/g' $2 ;;
    fre | eng | spa | ara) sed -e 's/L_\([^_>]*\)[^>]*>/L_\1>/g' $2;;
esac
