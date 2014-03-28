##!bin/sh

#  Environmnent parameters
if [ ! -f $TEXT_FILE ] || [ ! -f $REFERENCE_FILE ]; then
  echo "Usage: analyzer.sh - environment variables TEXT_FILE and REFERENCE_FILE must be defined as absolute paths"
  exit
fi

# Do analysis
source /home/leffe/Lima/leffe.env
TEXT_FILE_DIR=`dirname $TEXT_FILE`
TEXT_FILE_BASENAME=`basename $TEXT_FILE`
OUTPUT_DIR=`echo $TEXT_FILE_DIR/../output | xargs readlink -f`
/home/leffe/Passage/CeaTools/EvalScript/evaluator-easy.sh $TEXT_FILE $REFERENCE_FILE $START_TIME > $OUTPUT_DIR/$TEXT_FILE_BASENAME.eval.out
