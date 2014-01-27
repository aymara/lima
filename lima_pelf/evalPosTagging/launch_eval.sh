#!/bin/bash


if (($#==0)); then 
    echo usage: launch_eval.sh lang1 lang2 ...
fi

nbParts=10

# Emplacement de ce script sur la machine:
# (à paramétrer avant le lancement de l'évaluation)
EVAL_PATH=$PWD 

# svm_light location:
svm_light="$EVAL_PATH/SVM/svm_light";
# svm_learn location (perl version):
svm_learn="$EVAL_PATH/SVM/SVMTool-1.3.1/bin/SVMTlearn"
# paramétrage de svm_learn:
# sed -i "s/use lib \".*\/lib/\";/use lib \"$EVAL_PATH\/SVM\/SVMTool-1.3.1\/lib\/\";/g" SVM/SVMTool-1.3.1/bin/SVMTlearn
# make sure training-sets exists
rm -Rf training-sets
mkdir -p training-sets

function readMethod() {
  dynsvmtool=`head -n 70 $LIMA_CONF/lima-lp-$1.xml | grep '"DynamicSvmToolPosTagger"/>'`
  svmtool=`head -n 70 $LIMA_CONF/lima-lp-$1.xml | grep '"SvmToolPosTagger"/>'`
  viterbi=`head -n 70 $LIMA_CONF/lima-lp-$1.xml | grep '"viterbiPostagger-freq"/>'`

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
        ger) addOption="-s L_GE_PONCTUATION_FORTE"; corpus=corpus-ger-small.txt; conf=config-minimale.SVMT ;;
        spa) addOption="-s L_PONCTU_FORTE_FERMANTE"; corpus=corpus-spa-30000.txt; conf=config-minimale.SVMT ;;
        fre) corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/fre/corpus/corpus_fre.txt; conf=config-minimale.SVMT ;;
        eng) addOption="-s . -n $nbParts"; corpus=$LINGUISTIC_DATA_ROOT/disambiguisationMatrices/eng/corpus/ptb.pos; conf=config-minimale.SVMT;;
        ara) corpus=corpus_ar.txt; conf=config-minimale.SVMT ;;
    esac
    method=$(readMethod $lang)
    echo "treating $lang.$method... (see ${lang}.${method}.log)"

    rm -Rf results.$lang.$method


    ./tfcv.py -c -t -l $lang $addOption $corpus conf/$lang/$conf $svm_light $svm_learn
    if  [[ $? -ne 0 ]]; then
      echo "tfcv error, exiting"
      exit
    fi

    echo results.$lang.$method
    for f in results.$lang.$method/*/aligned; do 
        ./micro2macro.sh $lang $f > $f.macro;
    done
    echo "micro: " `./eval.pl results.$lang.$method/*/aligned 2>&1 | grep "^all.precision"` 
    echo "macro: " `./eval.pl results.$lang.$method/*/aligned.macro 2>&1 | grep "^all.precision"` 

    mkdir -p ExploitResults/data
    ./problemesAlignement.sh $lang $method
    pushd ExploitResults
    ./detailed-res.sh $nbParts $lang >& /dev/null
    popd
done
