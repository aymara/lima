##!bin/sh
# The script needs variables to be defined : LEFFE_RESOURCES, LEFFE_ALIGNER, LEFFE_EVALUATOR

# command line parameters
if [ $1 ] && [ $2 ] && [ $3 ]; then
  TEXT_FILE=`readlink -f $1`
  REFERENCE_FILE=`readlink -f $2`
  START_TIME=$3
fi
if [ ! $1 ] || [ ! -f $ANALYSIS_FILE ] || [ ! $2 ] || [ ! -f $REFERENCE_FILE ] \
  || [ ! $3 ]; then
  echo "Usage: evaluator.sh analysis-file reference-file start-time"
  echo "  analysis-file : analyzed text file path, output should be analysis-file.easy.xml"
  echo "  reference-file : reference file to evaluate against"
  echo "  start-time : string storing the start time of this evaluation"
  exit
fi

# creates output dir if not exists
TEXT_FILE_DIR=`dirname $TEXT_FILE`
TEXT_FILE_BASENAME=`basename $TEXT_FILE`
OUTPUT_DIR=`echo $TEXT_FILE_DIR/../output | xargs readlink -f`
ANALYSIS_FILE=$OUTPUT_DIR/$TEXT_FILE_BASENAME.easy.xml
ANALYSIS_FILE_ALIGNED=$OUTPUT_DIR/$TEXT_FILE_BASENAME.aligned.easy.xml
STORED_ANALYSIS_FILE_ALIGNED=$OUTPUT_DIR/$START_TIME/$TEXT_FILE_BASENAME.aligned.easy.xml
EVAL_FILE=$OUTPUT_DIR/$TEXT_FILE_BASENAME.eval.out
echo "Creating output path: "$OUTPUT_DIR
install -d $OUTPUT_DIR
echo "Creating results path: $OUTPUT_DIR/$START_TIME"
install -d $OUTPUT_DIR/$START_TIME

# temporary folder location
PATH_TMP=`dirname $ANALYSIS_FILE`/tmp-evaluator-`date +%s`-`uuidgen`
echo "Creating temporary path: "$PATH_TMP
mkdir -p $PATH_TMP
cd $PATH_TMP

echo "---------"
echo "ALIGNMENT"
echo "---------"

# command configuration
EASY_DTD=$LEFFE_RESOURCES/easy.dtd
ALIGNTOOL_BIN=$LEFFE_ALIGNER

echo "Preparing folders"
ALIGNER_PATH_TMP=$PATH_TMP/aligner
ALIGNED_FILE=$ALIGNER_PATH_TMP/out.easy.xml
ALIGNED_FILE_TMP=$ALIGNER_PATH_TMP/out.easy.xml.tmp
mkdir -p $ALIGNER_PATH_TMP/alignedReference
mkdir -p $ALIGNER_PATH_TMP/alignedSubmissions/PX
mkdir -p $ALIGNER_PATH_TMP/lastAlignedSubmissions/PX
mkdir -p $ALIGNER_PATH_TMP/lastAlignedSubmissionsWithRelations/PX
mkdir -p $ALIGNER_PATH_TMP/wellformedSubmissionsUTF8/PX
cp $EASY_DTD $ALIGNER_PATH_TMP/wellformedSubmissionsUTF8/PX
cp $EASY_DTD $ALIGNER_PATH_TMP/alignedReference

echo "Copying files"
ANALYSIS_FILE_BASENAME=`basename $ANALYSIS_FILE .easy.xml`
cp $ANALYSIS_FILE $ALIGNER_PATH_TMP/wellformedSubmissionsUTF8/PX/$ANALYSIS_FILE_BASENAME.easy.xml
cp $REFERENCE_FILE $ALIGNER_PATH_TMP/alignedReference/$ANALYSIS_FILE_BASENAME.easy.xml.txt

echo "Executing aligner tool"
pushd $ALIGNTOOL_BIN #>& /dev/null
./cea_aligner.sh $ALIGNER_PATH_TMP #>& /dev/null
popd #>& /dev/null 
if [ ! -f $ALIGNER_PATH_TMP/lastAlignedSubmissionsWithRelations/PX/$ANALYSIS_FILE_BASENAME.easy.xml ]; then
  echo "Aligner tool failed"
  exit
fi

echo "Creating aligned output file $ALIGNED_FILE"
cp $ALIGNER_PATH_TMP/lastAlignedSubmissionsWithRelations/PX/$ANALYSIS_FILE_BASENAME.easy.xml $ALIGNED_FILE_TMP
#recode latin1..utf8 $ALIGNED_FILE_TMP
tr "'" "\"" < $ALIGNED_FILE_TMP > $ALIGNED_FILE
sed -i "s/&apos;/'/g" $ALIGNED_FILE
sed -i "s/<EMPTY>/empty/g" $ALIGNED_FILE
sed -i "s/&lt;EMPTY&gt;/empty/g" $ALIGNED_FILE
cp $ALIGNED_FILE $ANALYSIS_FILE_ALIGNED
echo "Copied output file into $TEXT_FILE.easy.xml.aligned"
cp $ALIGNED_FILE $STORED_ANALYSIS_FILE_ALIGNED
echo "Copied output file into $STORED_ANALYSIS_FILE_ALIGNED"

echo "----------"
echo "EVALUATION"
echo "----------"

# command configuration
EVALUATE_PATH_TMP=$PATH_TMP/evaluator
EVALUATE_ENV=$EVALUATE_PATH_TMP/cea_easy_env.sh
EVALUATORTOOL_DIR=$LEFFE_EVALUATOR
EVALUATORTOOL_BIN=$EVALUATORTOOL_DIR/bin
EVALUATE_TEMPLATE_ENV=$EVALUATORTOOL_DIR/cea_easy_env.sh

echo "Preparing folders"
mkdir -p $EVALUATE_PATH_TMP
EVALUATE_PATH_TMP_DATA=$EVALUATE_PATH_TMP/data
mkdir -p $EVALUATE_PATH_TMP_DATA
mkdir -p $EVALUATE_PATH_TMP_DATA/lima
mkdir -p $EVALUATE_PATH_TMP_DATA/ref
EVALUATE_PATH_TMP_RESULTS=$EVALUATE_PATH_TMP/results
mkdir -p $EVALUATE_PATH_TMP_RESULTS

echo "Preparing evaluator environment"
cp $EVALUATE_TEMPLATE_ENV $EVALUATE_ENV
EVALUATOR_REFERENCE_BASE=$EVALUATE_PATH_TMP_DATA/ref/$ANALYSIS_FILE_BASENAME
cp $REFERENCE_FILE $EVALUATOR_REFERENCE_BASE.xml
grep "<E id=\"[^\"]*\">" $EVALUATOR_REFERENCE_BASE.xml | sed "s/<E id=\"//g" | sed "s/\">//g" > $EVALUATOR_REFERENCE_BASE.add
sed -i "s|LEFFE_VAR_REFERENCE_DATA_DIR|$EVALUATE_PATH_TMP_DATA/ref|g" $EVALUATE_ENV
cp $ANALYSIS_FILE_ALIGNED $EVALUATE_PATH_TMP_DATA/lima/$ANALYSIS_FILE_BASENAME.xml
sed -i "s|LEFFE_VAR_P_DIR|$EVALUATE_PATH_TMP_DATA|g" $EVALUATE_ENV
sed -i "s|LEFFE_VAR_P_LIST|\"lima\"|g" $EVALUATE_ENV
sed -i "s|LEFFE_VAR_RES_DIR|$EVALUATE_PATH_TMP_RESULTS|g" $EVALUATE_ENV
echo "LEFFE $ANALYSIS_FILE_BASENAME" > $EVALUATE_PATH_TMP_DATA/lima-corpus-def.txt
sed -i "s|LEFFE_VAR_CORPUS_DEFINITION_FILE|$EVALUATE_PATH_TMP_DATA/lima-corpus-def.txt|g" $EVALUATE_ENV

echo "Executing evaluator tool (log in $EVALUATE_PATH_TMP/evaluator.out)"
pushd $EVALUATORTOOL_BIN > /dev/null 2>&1
$EVALUATORTOOL_BIN/do_Reval_ALL.sh -e $EVALUATE_ENV > $EVALUATE_PATH_TMP/evaluator.out 2>&1
cp $EVALUATE_PATH_TMP/evaluator.out $EVAL_FILE
popd > /dev/null 2>&1

echo "Retrieving evaluation results"
grep "EVAL SUBCORPUS_LEFFE" $EVALUATE_PATH_TMP/evaluator.out;

echo "Retrieving errors list"
$EVALUATORTOOL_BIN/cea_extractErrors_ALL.pl $EVALUATE_PATH_TMP/results/lima/hypgroup_equal/*_result_relations >> $EVAL_FILE
grep "^[FA][|]" $EVAL_FILE;

# temporary folder delete
echo "Deleting temporary files"
rm -Rf $PATH_TMP
