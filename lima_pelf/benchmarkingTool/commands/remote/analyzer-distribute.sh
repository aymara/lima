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

# Send analysis to node and wait for result
echo "Starting job"
cd `dirname $TEXT_FILE`
JOB_NAME=`qsub -l nodes=1:ppn=1 -o $TEXT_FILE.log -e $TEXT_FILE.err -v TEXT_FILE=$TEXT_FILE /home/leffe/Passage/Scripts/analyzer-node.sh`
FINISHED="False"
while [ -n "$FINISHED" ]; do
  echo " ... waiting for job $JOB_NAME to terminate"
  FINISHED=`qstat $JOB_NAME 2> /dev/null`
  usleep 500000
done
echo "Moving files to output folder"
mv *.easy.xml *.err *.log ../output
echo "Job terminated"
