##!bin/sh

# command line parameters
if [ $1 ]; then
  TEXT_FILE=`readlink -f $1`
fi
if [ ! $1 ] || [ ! -f $TEXT_FILE ]; then
  echo "Usage: analyzer.sh analysis-file"
  echo "  analysis-file : analysis file to analyze"
  exit
fi

# Remote output path
OUTPUT_DIR_REMOTE=/home/leffe/Passage/LeffeTmps/$USER

# ssh and rsync configuration
source $PELF_COMMANDS_ROOT/sshcommands.sh

# Preparing
TEXT_FILE_DIR=`dirname $TEXT_FILE`
TEXT_FILE_BASENAME=`basename $TEXT_FILE`
OUTPUT_DIR=`echo $TEXT_FILE_DIR/../output | xargs readlink -f`
OUTPUT_FILE=$OUTPUT_DIR/$TEXT_FILE_BASENAME.easy.xml
rm -f $OUTPUT_FILE
echo "Creating output path: "$OUTPUT_DIR
mkdir -p $OUTPUT_DIR

# Remote files copy
echo "Creating remote output paths: $OUTPUT_DIR_REMOTE {., ./text/, ./ref, ./output}"
$SSH_CMD leffe@calamondin mkdir -p $OUTPUT_DIR_REMOTE $OUTPUT_DIR_REMOTE/text $OUTPUT_DIR_REMOTE/ref $OUTPUT_DIR_REMOTE/output
rsyncexec  $TEXT_FILE leffe@calamondin:$OUTPUT_DIR_REMOTE/text/$TEXT_FILE_BASENAME

echo "---------"
echo "ANALYSIS"
echo "---------"

# Remote analysis
echo "Analyzing $TEXT_FILE"
sshexec leffe@calamondin /home/leffe/Passage/Scripts/analyzer-distribute.sh $OUTPUT_DIR_REMOTE/text/$TEXT_FILE_BASENAME

# Retrieving remote file and deleting remote output path
echo "Retrieving results and deleting remote temp files"
rsyncexec leffe@calamondin:$OUTPUT_DIR_REMOTE/output/$TEXT_FILE_BASENAME.easy.xml $OUTPUT_FILE
# Files may remain unmodified for current user ($USER) : text, ref and output filenames should be unique
# sshexec leffe@calamondin rm -Rf $OUTPUT_DIR_REMOTE

echo "Done, output should be in $OUTPUT_FILE"
