#####!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

invoked=$_   # needs to be first thing in the script
OPTIND=1    # also remember to initialize your flags and other variables

usage()
{
cat << EOF
Synopsis: $0 [OPTIONS]

Options default values are in parentheses.

  -m mode       <(debug)|release> compile mode
EOF
#   return 1 2>/dev/null || exit 1
}

mode="debug"

while getopts ":m:r:t" o; do
    case "${o}" in
        m)
            mode=${OPTARG}
            if [[ "$mode" != "asan" && "$mode" != "debug" && "$mode" != "release" ]] ;
            then
              usage
              return 1 2>/dev/null || exit 1
            fi
            ;;
        r)
            suggested_root="${OPTARG}"
            ;;
        t)
            suggested_root=/tmp/${PWD##*/}
            ;;
        *)
            usage
            return 1 2>/dev/null || exit 1
            ;;
    esac
done
shift $((OPTIND-1))


# Function to get the absolute directory path in bash
get_script_dir_bash() {
  echo "$(dirname "$(realpath "$0")")"
}

# Function to get the absolute directory path in zsh
get_script_dir_zsh() {
  echo "${0:a:h}"
}


if [ ${#suggested_root} -le 0 ] ; then
  # Check which shell is running the script
  if [ -n "$ZSH_VERSION" ]; then
    # Running in zsh
    script_dir=$(get_script_dir_zsh)
  else
    # Assume bash if not zsh
    script_dir=$(get_script_dir_bash)
  fi
  export LIMA_ROOT=$script_dir/..
else
  export LIMA_ROOT=$suggested_root
fi

LIMA_SOURCES=$PWD
cd $LIMA_SOURCES > /dev/null
current_branch=`git rev-parse --abbrev-ref HEAD`
cd - > /dev/null

# Path to the nltk data necessary to learn the English part of speech tagging
# model.
export NLTK_PTB_DP_FILE=$HOME/nltk_data/corpora/dependency_treebank/nltk-ptb.dp

# Path to the LIMA linguistic data sources
export LINGUISTIC_DATA_ROOT=$LIMA_SOURCES/lima_linguisticdata

# Path to where the build will be done, where temporary binary files are written
export LIMA_BUILD_DIR=$LIMA_ROOT/Builds
install -d $LIMA_BUILD_DIR

# Path where LIMA will be installed
export LIMA_DIST=$LIMA_ROOT/Dist/$current_branch/$mode
install -d $LIMA_DIST

# Path to where the LIMA configuration files are installed. Can be overriden at
# runtime
export LIMA_CONF=$LIMA_DIST/share/config/lima

# Path to where the LIMA runtime linguistic resources (compiled dictionary and
# rules, ...) are installed. Can be overriden at runtime
export LIMA_RESOURCES=$LIMA_DIST/share/apps/lima/resources

# Where to install external  libraries if not using system ones
export LIMA_EXTERNALS=$LIMA_ROOT/externals

# Add the path to LIMA executables to the search path
export PATH=$LIMA_DIST/bin:$LIMA_EXTERNALS/bin:$PATH

# Add the path to LIMA libraries to the libaries search path
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib:$LIMA_DIST/extern/lib:$LD_LIBRARY_PATH

export UD_TREEBANKS=/home/gael/Documents/UniversalDependencies/ud-treebanks-v2.9
