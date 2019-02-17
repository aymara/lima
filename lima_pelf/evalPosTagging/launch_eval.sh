#!/bin/bash

set -o nounset
set -o errexit
set -o pipefail

command -v SVMTlearn >/dev/null 2>&1 || { echo >&2 "I require SVMTlearn but it's not in the path  Aborting."; exit 1; }
command -v svm_learn >/dev/null 2>&1 || { echo >&2 "I require svm_learn but it's not in the path  Aborting."; exit 1; }

usage()
{
  program=`basename $0`
  cat<<EOF

Usage: $program [OPTION] language1 language2 ...
Evaluate LIMA PoS tagging on several languages.

  Options:
  -f, --fragment <value>     Use only a fragment of the given size from the training corpus
  -n, --notrain              Skip the PoT tagger training step. It must have been done previously
  -h, --help                 Shows this help.

EOF
  exit 1
}


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

# Execute getopt on the arguments passed to this program, identified by the special character $@
PARSED_OPTIONS=$(getopt -n "$0"  -o "f:nh" --long "fragment:,notrain,help"  -- "$@")

#Bad arguments, something has gone wrong with the getopt command.
if [ $? -ne 0 ];
then
  echo "Error parsing arguments"
  exit 1
fi

# A little magic, necessary when using getopt.
eval set -- "$PARSED_OPTIONS"

# Now goes through all the options with a case and using shift to analyse 1 argument at a time.
#$1 identifies the first argument, and when we use shift we discard the first argument, so $2 becomes $1 and goes again through the case.
fragment="infinity"
while true;
do
  case "$1" in

#     -1|--one)
#       echo "One"
#       shift;;

    -n|--notrain)
      notrain=true
      shift;;

    -f|--fragment)
      fragment=$2
      shift 2;;

    -h|--help)
      usage
     shift;;

    --)
      shift
      break;;
  esac
done
echo "Options parsed"
if [ "$#" -lt 1 ]
then
  echo "No language to evaluate"
  exit 1
fi

for lang in $*; do
echo "lang is $lang"
    addOption=""
    case $lang in
        eng)
            addOption="-s . -n $nbParts"
            corpusFile=$(findFileInPaths $LIMA_RESOURCES Disambiguation/corpus_eng_merge.txt  ":")
            corpus=$corpusFile
            conf=config-minimale-eng.SVMT;;
        eng.ud)
            nbParts="1"
            addOption="-s ."
            corpusFile=$(findFileInPaths $LIMA_SOURCES lima_linguisticdata/disambiguisationMatrices/eng.ud/UD_English-EWT/en_ewt-ud-train.conllu  ":")
            corpus=$(echo $corpusFile | sed -e 's/train.conllu//')
            conf=config-minimale-eng.ud.SVMT;;
        fre)
            addOption="-n $nbParts"
            corpus=$(findFileInPaths $LIMA_RESOURCES Disambiguation/corpus_fre_merge.txt  ":")
            conf=config-minimale-fre.SVMT ;;
        por)
            addOption="-s PU+FORTE -n $nbParts"
            corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/por/corpus/macmorpho.conll.txt
            conf=config-minimale-por.SVMT ;;
    esac
    method=$(readMethod $lang)
    echo "treating $lang.$method... (see ${lang}.${method}.log)"


    confFile=$(findFileInPaths $LIMA_CONF $conf ":")

    if [ $notrain = true ]
    then
        $EVAL_PATH/tfcv.py -l $lang $addOption $corpus $confFile $svm_light $svm_learn
    else
        rm -Rf results.$lang.$method
        $EVAL_PATH/tfcv.py -c -t -l $lang -f $fragment $addOption $corpus $confFile $svm_light $svm_learn
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
