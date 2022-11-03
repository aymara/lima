# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
echo "TEXT_FILE_DIR $TEXT_FILE_DIR"
echo "TEXT_FILE_BASENAME $TEXT_FILE_BASENAME"
echo "OUTPUT_DIR $OUTPUT_DIR"

install -d $OUTPUT_DIR
pushd $TEXT_FILE_DIR
analyzeXmlDocuments --language=fre --pipeline=easy --dumper=easyXmlDumper $TEXT_FILE
echo "Moving files to output folder"
mv *.easy.xml *.err *.log ../output
popd
/home/leffe/Passage/Scripts/evaluator-easy.sh $TEXT_FILE $REFERENCE_FILE $START_TIME > $OUTPUT_DIR/$TEXT_FILE_BASENAME.eval.out
