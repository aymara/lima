#!/bin/bash

txtfile=$1
lang=fre

analyzeText -l $lang  -p easy -d text -d bow -d fullxml $txtfile
syanot $txtfile.out >& /dev/null &

if [ -f $txtfile.bp.dot ]
then
  kgraphviewer $txtfile.bp.dot >& /dev/null &
else
  echo "No .bp.dot file. Check $MM_CONF/MM-lp-$lang.xml" > /dev/stderr
fi

if [ -f $txtfile.dot ]
then
  kgraphviewer $txtfile.dot >& /dev/null &
else
  echo "No .dot file. Check $MM_CONF/MM-lp-$lang.xml" > /dev/stderr
fi

for f in $txtfile.sent*.sa.dot ; do
  kgraphviewer $f  & >& /dev/null &
done

if [ -f $txtfile.anal.xml ]
then
  kate -u $txtfile.anal.xml >& /dev/null &
else
  echo "No .anal.xml file. Check $MM_CONF/MM-lp-$lang.xml" > /dev/stderr
fi

