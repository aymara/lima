#    
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
#!/bin/bash

usage() { echo "Usage: $0 [-m <debug|release>] [-v <val|rev>]" 1>&2; exit 1; }

mode="debug"
release="1"

while getopts ":m:v:" o; do
    case "${o}" in
        m)
            mode=${OPTARG}
            ((mode == "debug" || mode == "release")) || usage
            ;;
        v)
            release=${OPTARG}
            ((release == "val" || release == "rev")) || usage
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

current_branch=`git rev-parse --abbrev-ref HEAD`
current_revision=`git rev-parse --short HEAD`
current_timestamp=`git show -s --format=%ct HEAD`
current_project=`basename $PWD`
build_prefix=$AMOSE_BUILD_DIR/$current_branch
source_dir=$PWD

if [[ $release = "rev" ]]; then
release="$current_timestamp-$current_revision"
fi

if [[ $mode == "release" ]]; then
echo "version='$release'"
    install -d $build_prefix/release/$current_project
    pushd $build_prefix/release/$current_project
    cmake -DAMOSE_VERSION_RELEASE="$release" -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Release $source_dir
    make -j4 && make install && make package
    result=$?
    popd
else
    # default is : compile in debug mode in debug/ directory
    install -d $build_prefix/debug/$current_project
    pushd $build_prefix/debug/$current_project
    cmake -DAMOSE_VERSION_RELEASE=$release -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Debug $source_dir
    make -j`grep -c ^processor /proc/cpuinfo` && make install
    result=$?
    popd
fi

exit $result
