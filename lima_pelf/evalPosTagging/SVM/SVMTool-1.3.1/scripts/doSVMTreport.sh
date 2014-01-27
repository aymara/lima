#!/bin/bash -f
# Author      : Jesús Giménez
# Date        : April 26, 2004
# Description : SVMTT evaluation
#
# Usage: doSVMTreport.sh <MODEL> <INDIR/CORPUS> <OUTDIR/CORPUS>
#
# use this to prepare for gnuplot:
#   grep -v '\*' REPORTS/100kargmaxlf.WSJTP.TEST | grep 'last' | gawk '{print $3 " " $5 " " $6 " " $7 " " $8}' > GRAPHS/100kargmax.last.txt ?????????????

if [ $# -eq 6 ]
  then
       printf "* ========================= SVMTeval report ==============================\n"
       printf "* model   = [%s]\n* testset = [%s]\n" $1 $2
       printf "* ========================================================================\n";
       printf "* strategy     \t    \tknown\tambig.\tunknown\toverall\n";
       printf "* ========================================================================\n";
       EVAL="/home/usuaris/jgimenez/SVMT/bin/SVMTeval"
       let ST=$4
       let LAST=$5+1
       DIR="$6"
       while [ $ST -lt $LAST ]; do
          echo STRATEGY IS $ST
          if [ "$DIR" = "LR" ]
          then   
             printf "  T = %d \t LR \t" $ST; $EVAL -1 $1 $2 $3".T$ST.LR"
          elif [ "$DIR" = "RL" ]
          then
             printf "  T = %d \t RL \t" $ST; $EVAL -1 $1 $2 $3".T$ST.RL"
          elif [ "$DIR" = "LRL" ]
          then
             printf "  T = %d \t LR \t" $ST; $EVAL -1 $1 $2 $3".T$ST.LR"
             printf "  T = %d \t RL \t" $ST; $EVAL -1 $1 $2 $3".T$ST.RL"
             printf "  T = %d \t LRL\t" $ST; $EVAL -1 $1 $2 $3".T$ST.LRL"
          elif [ "$DIR" = "GLRL" ]
          then
             printf "  T = %d \t LR \t" $ST; $EVAL -1 $1 $2 $3".T$ST.LR"
             printf "  T = %d \t RL \t" $ST; $EVAL -1 $1 $2 $3".T$ST.RL"
             printf "  T = %d \t LRL\t" $ST; $EVAL -1 $1 $2 $3".T$ST.LRL"
             if [ $ST = 5 ] || [ $ST = 6 ]
             then   
                printf "  T = %d \t GLRL\t" $ST; $EVAL -1 $1 $2 $3".T$ST.GLRL"
             fi
          fi
          let ST=$ST+1 
       done
       printf "* ========================================================================\n";

else
  echo "Usage: $0  <MODEL> <INDIR/NAME> <OUTDIR/NAME> <fist_strategy> <last_strategy> <LR/RL/LRL>"
fi




