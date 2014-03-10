##!bin/sh

#  Environmnent parameters
if [ ! -f $TEXT_FILE ]; then
  echo "Usage: analyzer.sh - environment variable TEXT_FILE must be defined as an absolute path"
  exit
fi

# Begin with a simple master analysis
source /home/leffe/Lima/leffe.env
cd `dirname $TEXT_FILE`
analyzeXmlDocuments --language=fre --pipeline=easy --dumper=easyXmlDumper $TEXT_FILE
