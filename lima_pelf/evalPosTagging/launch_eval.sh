#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

command -v SVMTlearn >/dev/null 2>&1 || { echo >&2 "I require SVMTlearn but it's not in the path  Aborting."; exit 1; }
command -v svm_learn >/dev/null 2>&1 || { echo >&2 "I require svm_learn but it's not in the path  Aborting."; exit 1; }

if (($#==0)); then 
    echo usage: launch_eval.sh lang1 lang2 ...
fi

source $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/lima-pelf-functions.sh

nbParts=10

# Position of this script on the computer
EVAL_PATH=`dirname $0`

# svm_light location:
svm_light=$(dirname $(command -v svm_learn ))
# svm_learn location (perl version):
svm_learn="SVMTlearn"
# svm_learn parameters:
# sed -i "s/use lib \".*\/lib/\";/use lib \"$EVAL_PATH\/SVM\/SVMTool-1.3.1\/lib\/\";/g" SVM/SVMTool-1.3.1/bin/SVMTlearn


# make sure training-sets exists
# rm -Rf training-sets
mkdir -p training-sets

notrain=false
for lang in $*; do
    addOption=""
    case $lang in
        eng) 
            addOption="-s . -n $nbParts"; 
            corpusFile=$(findFileInPaths $LIMA_RESOURCES Disambiguation/corpus_eng_merge.txt  ":")
            corpus=$corpusFile 
            conf=config-minimale-eng.SVMT;;
        fre) corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/fre/corpus/corpus_fre.txt; conf=config-minimale-fre.SVMT ;;
        por) addOption="-s PU+FORTE -n $nbParts"; corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/por/corpus/macmorpho.conll.txt; conf=config-minimale-por.SVMT ;;
        notrain) notrain=true ;;
    esac
    method=$(readMethod $lang)
    echo "treating $lang.$method... (see ${lang}.${method}.log)"


    confFile=$(findFileInPaths $LIMA_CONF $conf ":")
    
    if [ $notrain = true ]
    then
        $EVAL_PATH/tfcv.py -l $lang $addOption $corpus $confFile $svm_light $svm_learn
    else
        rm -Rf results.$lang.$method
        $EVAL_PATH/tfcv.py -c -t -l $lang $addOption $corpus $confFile $svm_light $svm_learn
    fi
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
    echo "$EVAL_PATH/problemesAlignement.sh $lang $method"
    $EVAL_PATH/problemesAlignement.sh $lang $method
    echo "$EVAL_PATH/detailed-res.sh $nbParts $lang"
    $EVAL_PATH/detailed-res.sh $nbParts $lang 
done
