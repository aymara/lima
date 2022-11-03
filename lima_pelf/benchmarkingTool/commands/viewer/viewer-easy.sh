# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

##!bin/sh

# command line parameters
if [ $1 ] && [ $2 ]; then
  TEXT_FILE=`readlink -f $1`
  REFERENCE_FILE=`readlink -f $2`
fi
if [ ! $1 ] || [ ! -f $TEXT_FILE ] || [ ! $2 ] || [ ! -f $REFERENCE_FILE ]; then
  echo "Usage: viewer.sh analysis-file reference-file"
  echo "  analysis-file : analysis file already analysed"
  echo "  reference-file : reference file to be evaluate against"
  exit
fi

# retrieve output files
TEXT_FILE_DIR=`dirname $TEXT_FILE`
TEXT_FILE_BASENAME=`basename $TEXT_FILE`
OUTPUT_DIR=`echo $TEXT_FILE_DIR/../output | xargs readlink -f`
ANALYSIS_FILE=$OUTPUT_DIR/$TEXT_FILE_BASENAME.easy.xml
ANALYSIS_FILE_ALIGNED=$OUTPUT_DIR/$TEXT_FILE_BASENAME.aligned.easy.xml
EVAL_FILE=$OUTPUT_DIR/$TEXT_FILE_BASENAME.eval.out

# Syanot for viewing easy files
syanot $ANALYSIS_FILE_ALIGNED > /dev/null 2>&1 &
syanot $REFERENCE_FILE > /dev/null 2>&1 &

# Evaluator output - not very usefull :(
# kate -u $EVAL_FILE > /dev/null 2>&1 &

# Diff - doesn't show very interresting things :(
# cp $ANALYSIS_FILE_ALIGNED $ANALYSIS_FILE_ALIGNED.diff
# cp $REFERENCE_FILE $REFERENCE_FILE.diff
# sed -i "s/xlink:type=\"extended\"//g" $ANALYSIS_FILE_ALIGNED.diff $REFERENCE_FILE.diff
# sed -i "s/xlink:type=\"locator\"//g" $ANALYSIS_FILE_ALIGNED.diff $REFERENCE_FILE.diff
# diff -uwd $REFERENCE_FILE.diff $ANALYSIS_FILE_ALIGNED.diff | kate -iu > /dev/null 2>&1 &
# rm -f $ANALYSIS_FILE_ALIGNED.diff $REFERENCE_FILE $REFERENCE_FILE.diff

# Dot files output from Lima - not adapted to easy (XML) input :(
# kgraphviewer $TEXT_FILE.dot | kate -iu &
