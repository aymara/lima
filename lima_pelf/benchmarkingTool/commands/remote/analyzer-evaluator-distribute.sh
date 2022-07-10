# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

##!bin/sh

# command line parameters
if [ $1 ] && [ $2 ] && [ $3 ]; then
  TEXT_FILE=`readlink -f $1`
  REFERENCE_FILE=`readlink -f $2`
  START_TIME=$3
fi
if [ ! $1 ] || [ ! -f $TEXT_FILE ] || [ ! $2 ] || [ ! -f $REFERENCE_FILE ]\
  || [ ! $3 ]; then
  echo "Usage: evaluator.sh text-file reference-file"
  echo "  text-file : text file path, output should be analysis-file.easy.xml"
  echo "  reference-file : reference file to evaluate against"
  echo "  start-time : string storing the start time of this evaluation"
  exit
fi

# Send analysis to node and wait for result
echo "Starting job"
cd `dirname $TEXT_FILE`
JOB_NAME=`qsub -l nodes=1:ppn=1 -o $TEXT_FILE.log -e $TEXT_FILE.err -v TEXT_FILE=$TEXT_FILE,REFERENCE_FILE=$REFERENCE_FILE,START_TIME=$START_TIME /home/leffe/Passage/Scripts/analyzer-evaluator-node.sh`
FINISHED="False"
while [ -n "$FINISHED" ]; do
  echo " ... waiting for job $JOB_NAME to terminate"
  FINISHED=`qstat $JOB_NAME 2> /dev/null`
  usleep 500000
done
echo "Job terminated"
