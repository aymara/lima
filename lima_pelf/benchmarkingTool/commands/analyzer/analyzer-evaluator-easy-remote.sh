##!bin/sh

# command line parameters
if [ $1 ] && [ $2 ] && [ $3 ]; then
  TEXT_FILE=`readlink -f $1`
  REFERENCE_FILE=`readlink -f $2`
  START_TIME=$3
fi
if [ ! $1 ] || [ ! -f $TEXT_FILE ] || [ ! $2 ] || [ ! -f $REFERENCE_FILE ] \
  || [ ! $3 ]; then
  echo "Usage: $0 analysis-file reference-file start-time"
  echo "  analysis-file : analyzed text file path, output should be analysis-file.easy.xml"
  echo "  reference-file : reference file to evaluate against"
  echo "  start-time : string storing the start time of this evaluation"
  exit
fi

# Remote output path
OUTPUT_DIR_REMOTE=/home/leffe/Passage/LeffeTmps/$USER/$START_TIME

# ssh and rsync configuration
source $LEFFE_COMMANDS_ROOT/sshcommands.sh

# Preparing
TEXT_FILE_DIR=`dirname $TEXT_FILE`
TEXT_FILE_BASENAME=`basename $TEXT_FILE`
OUTPUT_DIR=`echo $TEXT_FILE_DIR/../output | xargs readlink -f`
echo "Creating output path: "$OUTPUT_DIR
install -d $OUTPUT_DIR
echo "Creating results path: $OUTPUT_DIR/$START_TIME"
install -d $OUTPUT_DIR/$START_TIME
ANALYSIS_FILE=$OUTPUT_DIR/$START_TIME/$TEXT_FILE_BASENAME.easy.xml
rm -f $ANALYSIS_FILE
ANALYSIS_FILE_ALIGNED=$OUTPUT_DIR/$START_TIME/$TEXT_FILE_BASENAME.aligned.easy.xml
STORED_ANALYSIS_FILE_ALIGNED=$OUTPUT_DIR/$START_TIME/$TEXT_FILE_BASENAME.aligned.easy.xml
EVAL_FILE=$OUTPUT_DIR/$START_TIME/$TEXT_FILE_BASENAME.eval.out
rm -f $EVAL_FILE

# Remote files copy
echo "Creating remote output paths: $OUTPUT_DIR_REMOTE {., ./text/, ./ref, ./output}"
$SSH_CMD leffe@calamondin install -d $OUTPUT_DIR_REMOTE $OUTPUT_DIR_REMOTE/text $OUTPUT_DIR_REMOTE/ref $OUTPUT_DIR_REMOTE/output
REFERENCE_FILE_BASENAME=`basename $REFERENCE_FILE`
rsyncexec $TEXT_FILE leffe@calamondin:$OUTPUT_DIR_REMOTE/text/$TEXT_FILE_BASENAME
rsyncexec $REFERENCE_FILE leffe@calamondin:$OUTPUT_DIR_REMOTE/ref/$REFERENCE_FILE_BASENAME
# rsyncexec $ANALYSIS_FILE leffe@calamondin:$OUTPUT_DIR_REMOTE/output/$TEXT_FILE_BASENAME.easy.xml

echo "----------------------------------"
echo "ANALYSIS, ALIGNMENT AND EVALUATION"
echo "----------------------------------"

# Remote analysis
echo "$TEXT_FILE"
echo "Analyzing , aligning and evaluating $TEXT_FILE against $REFERENCE_FILE"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/analyzer-evaluator-distribute.sh $OUTPUT_DIR_REMOTE/text/$TEXT_FILE_BASENAME $OUTPUT_DIR_REMOTE/ref/$REFERENCE_FILE_BASENAME $START_TIME


# Retrieving remote file and deleting remote output path
echo "Retrieving results and deleting remote temp files"
rsyncexec leffe@calamondin:$OUTPUT_DIR_REMOTE/output/$TEXT_FILE_BASENAME.easy.xml $ANALYSIS_FILE
rsyncexec leffe@calamondin:$OUTPUT_DIR_REMOTE/output/$TEXT_FILE_BASENAME.eval.out $EVAL_FILE
echo "dist res file: '$OUTPUT_DIR_REMOTE'/output/'$TEXT_FILE_BASENAME'.aligned.easy.xml"
echo "local res $STORED_ANALYSIS_FILE_ALIGNED"
rsyncexec leffe@calamondin:$OUTPUT_DIR_REMOTE/output/$TEXT_FILE_BASENAME.aligned.easy.xml $STORED_ANALYSIS_FILE_ALIGNED
# Files may remain unmodified for current user ($USER) : text, ref and output filenames should be unique
# sshexec leffe@calamondin rm -Rf $OUTPUT_DIR_REMOTE

echo "Evaluation results"
grep "EVAL SUBCORPUS_LEFFE" $EVAL_FILE;

echo "Errors list"
grep "^[FAT][|]" $EVAL_FILE;

echo "Done, output should be in $EVAL_FILE"
