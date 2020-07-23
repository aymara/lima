#!/bin/bash
invoked=$_   # needs to be first thing in the script
OPTIND=1    # also remember to initialize your flags and other variables

#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

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
            if [[ "$mode" != "debug" && "$mode" != "release" ]] ;
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


# Replace $PWD below by the path of  where you downloaded LIMA if you wish to
# be able to source this script from elsewhere


if [ ${#suggested_root} -le 0 ] ; then
  export LIMA_ROOT=$PWD/..
else
  export LIMA_ROOT=$suggested_root
fi

LIMA_SOURCES=$PWD
pushd $LIMA_SOURCES > /dev/null
current_branch=`git rev-parse --abbrev-ref HEAD`
popd > /dev/null

# Path to the nltk data necessary to learn the English part of speech tagging
# model.
export NLTK_PTB_DP_FILE=$HOME/nltk_data/corpora/dependency_treebank/nltk-ptb.dp

# Path to the LIMA linguistic data sources
export LINGUISTIC_DATA_ROOT=$LIMA_SOURCES/lima_linguisticdata

# Path to where the build will be done, where temporary binary files are written
export LIMA_BUILD_DIR=$LIMA_ROOT/Builds-clang
install -d $LIMA_BUILD_DIR

# Path where LIMA will be installed
export LIMA_DIST=$LIMA_ROOT/Dist-clang/$current_branch/$mode-clang
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
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib:$LD_LIBRARY_PATH

