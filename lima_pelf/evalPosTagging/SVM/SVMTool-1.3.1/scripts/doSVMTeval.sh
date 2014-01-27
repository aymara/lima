#! /usr/local/bin/bash -f
# Author      : Jesús Giménez
# Date        : March 31, 2004
# Description : SVMT evaluation
#
# Usage: doSVMTeval.sh <MODEL> <INDIR/CORPUS> <OUTDIR/CORPUS>
#

if [ $# -eq 6 ]
  then
      TAGGER="/home/usuaris/jgimenez/SVMT/bin/SVMTagger"
      OPTIONS="-V 2"
      #OPTIONS="-K 0 -U 0 -B /usr/usuaris/ia/jgimenez/resources/DICTS/ENG.BLEX"
      let ST=$4
      let LAST=$5+1
      DIR="$6"
      while [  $ST -lt $LAST ]; do
         echo STRATEGY IS $ST :: DIR IS $DIR
         if [ "$DIR" = "LR" ]
         then   
            $TAGGER $OPTIONS -S LR -T $ST $1 < $2 > $3".T$ST.LR"
         elif [ "$DIR" = "RL" ]
         then
            $TAGGER $OPTIONS -S RL -T $ST $1 < $2 > $3".T$ST.RL"
         elif [ "$DIR" = "LRL" ]
         then
            $TAGGER $OPTIONS -S LR -T $ST $1 < $2 > $3".T$ST.LR"
            $TAGGER $OPTIONS -S RL -T $ST $1 < $2 > $3".T$ST.RL"
            $TAGGER $OPTIONS -S LRL -T $ST $1 < $2 > $3".T$ST.LRL"
         elif [ "$DIR" = "GLRL" ]
         then
            $TAGGER $OPTIONS -S LR -T $ST $1 < $2 > $3".T$ST.LR"
            $TAGGER $OPTIONS -S RL -T $ST $1 < $2 > $3".T$ST.RL"
            $TAGGER $OPTIONS -S LRL -T $ST $1 < $2 > $3".T$ST.LRL"
            if [ $ST = 5 ] || [ $ST = 6 ]
            then
               $TAGGER $OPTIONS -S GLRL -T $ST $1 < $2 > $3".T$ST.GLRL"
            fi
         fi
         let ST=$ST+1 
      done
else
  echo "Usage: $0  <MODEL> <INDIR/NAME> <OUTDIR/NAME> <first_strategy> <last_strategy> <LR/RL/LRL/GLRL>"
fi


