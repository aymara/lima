#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

function findFileInPaths()
{
  local paths=$1
  local fileName=$2
  local separator=$3
  pathsList=$(echo $paths | tr "$separator" "\n")
  for path in $pathsList
  do
    if [ -e "$path/$fileName" ]
    then
      echo "$path/$fileName"
      return 0
    fi
  done
  echo ""
}


function readMethod() {
  lpFile=$(findFileInPaths $LIMA_CONF lima-lp-$1.xml ":")
  if [ -z $lpFile ]
  then
    echo "No lima-lp-$1.xml found in $LIMA_CONF: setting method to 'none'." > /dev/stderr
    echo "none"
    return
  fi
  echo "lpFile is $lpFile" > /dev/stderr
  dynsvmtool=`head -n 70 $lpFile | grep '"DynamicSvmToolPosTagger"/>$'`
  svmtool=`head -n 70 $lpFile | grep '"SvmToolPosTagger"/>$'`
  viterbi=`head -n 70 $lpFile | grep '"viterbiPostagger-freq"/>$'`

  if [ -n "$svmtool" ]; then
    echo "svmtool"
  elif [ -n "$viterbi" ]; then 
    echo "viterbi"
  elif [ -n "$dynsvmtool" ]; then
    echo "dynsvmtool"
  else
    echo "none"
  fi
}
