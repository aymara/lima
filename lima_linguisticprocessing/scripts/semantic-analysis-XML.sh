#!/bin/bash
set -o errexit

#-------------------------------------------------------------------------------
# to ensure the script actually runs inside its own folder
MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
if [ -z "$MY_PATH" ] ; then
  # Error. for some reason, the path is not accessible
  # to the script (e.g. permissions re-evaled after suid)
  exit 1 # Fail
fi
#echo $MY_PATH


#WORK_DIR=$MY_PATH
WORK_DIR=`mktemp -d -p "/tmp"`

main() {
  check_args "$@"

  local CORPUS_DIR=$1
  local PIPELINE=$2
  local LANGUAGES=$3
  local NB_WORKERS=$4

  # find XML files
  xml_files=$WORK_DIR/xml_files.lst
  find -L "$CORPUS_DIR" -iname "*.xml" | sort > ${xml_files}
  
  # split list according to nb-workers
  rm -f ./x[a-z][a-z]
  split -n l/$NB_WORKERS --suffix-length=2 $xml_files

  # start background processes on sub-lists
  for f in ./x[a-z][a-z]
  do
    #echo "$f"
    process "$f" "${PIPELINE}" "${LANGUAGES}" &> "$f.log" &
  done

  # wait for background processes to end
  wait
}


function process() {
  local INPUT_FILE="$1"
  local PIPELINE=$2
  local LANGUAGES=$3
  while IFS= read -r line
  do
    analyzeXml ${LANGUAGES} -p ${PIPELINE} --Progress "$line"
  done < "$INPUT_FILE"
}

check_args() {
  if (($# != 4)); then
    echo "Error:
    Four arguments must be provided - $# provided.
  
    Usage:
      ./semantic-analysis-XML.sh <corpus-dir> <pipeline> <languages> <nb-workers>
    
    - Corpus-dir is a folder-tree containing amose xml-documents
    - Pipeline may be 'MyDomainXML'
    - Languages may be '-l fre' or '-l eng', or even both '-l eng -l fre' .
    - Nb-workers is an integer

Aborting."
    exit 1
  fi
}

main "$@"

