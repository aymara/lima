#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

export PATH=/home/sid-ahmed/Lima/lima/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/bin:$PATH

if (($#==0)); then 
    echo usage: launch_eval.sh lang1 lang2 ...
fi

nbParts=10

# Position of this script on the computer
EVAL_PATH=`dirname $0`

# svm_light location:
svm_light="/usr/local/bin";
# svm_learn location (perl version):
svm_learn="SVMTlearn"
# svm_learn parameters:
# sed -i "s/use lib \".*\/lib/\";/use lib \"$EVAL_PATH\/SVM\/SVMTool-1.3.1\/lib\/\";/g" SVM/SVMTool-1.3.1/bin/SVMTlearn
# make sure training-sets exists
rm -Rf training-sets
mkdir -p training-sets

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
  echo "lpFile is $lpFile"
  dynsvmtool=`head -n 70 $lpFile | grep '"DynamicSvmToolPosTagger"/>'`
  svmtool=`head -n 70 $lpFile | grep '"SvmToolPosTagger"/>'`
  viterbi=`head -n 70 $lpFile | grep '"viterbiPostagger-freq"/>'`

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

for lang in $*; do
    addOption=""
    case $lang in
        eng) addOption="-s . -n $nbParts"; corpus=$LIMA_RESOURCES/Disambiguation/corpus_eng_merge.txt conf=config-minimale-eng.SVMT;;
        fre) corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/fre/corpus/corpus_fre.txt; conf=config-minimale-fre.SVMT ;;
        por) addOption="-s PU+FORTE -n $nbParts"; corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/por/corpus/macmorpho.conll.txt; conf=config-minimale-por.SVMT ;;
    esac
    method=$(readMethod $lang)
    echo "treating $lang.$method... (see ${lang}.${method}.log)"

    rm -Rf results.$lang.$method


    $EVAL_PATH/tfcv.py -c -t -l $lang $addOption $corpus $LIMA_CONF/$conf $svm_light $svm_learn
    if  [[ $? -ne 0 ]]; then
      echo "tfcv error, exiting"
      exit 1
    fi

    echo results.$lang.$method
    for f in results.$lang.$method/*/aligned; do 
        $EVAL_PATH/micro2macro.sh $lang $f > $f.macro;
    done
    echo "micro: " `$EVAL_PATH/eval.pl results.$lang.$method/*/aligned 2>&1 | grep "^all.precision"` 
    echo "macro: " `$EVAL_PATH/eval.pl results.$lang.$method/*/aligned.macro 2>&1 | grep "^all.precision"` 

    mkdir -p results.$lang.$method/data
#     echo "$EVAL_PATH/problemesAlignement.sh $lang $method"
    $EVAL_PATH/problemesAlignement.sh $lang $method
#     echo "$EVAL_PATH/detailed-res.sh $nbParts $lang"
    $EVAL_PATH/detailed-res.sh $nbParts $lang 
done
