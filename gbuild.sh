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

usage() 
{ 
cat << EOF 1>&2; exit 1; 
Synopsis: $0 [OPTIONS]

Options default values are in parentheses.

  -m mode       <(debug)|release> compile mode
  -p boolean    <(true)|false> will build in parallel (make -jn) if true. 
                Necessary to be able to build with no parallelism as  it currently fail on 
                some machines.
  -r resources  <precompiled|(build)> build the linguistic resources or use the
                precompiled ones
  -v version    <(val)|rev> version number is set either to the value set by  
                config files or to the short git sha1
EOF
exit 1
}

[ -z "$LIMA_BUILD_DIR" ] && echo "Need to set LIMA_BUILD_DIR" && exit 1;
[ -z "$LIMA_DIST" ] && echo "Need to set LIMA_DIST" && exit 1;

mode="debug"
version="val"
resources="build"
parallel="true"

while getopts ":m:p:r:v:" o; do
    case "${o}" in
        m)
            mode=${OPTARG}
            [[ "$mode" == "debug" || "$mode" == "release" ]] || usage
            ;;
        p)
            parallel=${OPTARG}
            [[ "$parallel" == "true" || "$parallel" == "false" ]] || usage
            ;;
        r)
            resources=${OPTARG}
            [[ "$resources" == "precompiled" || "$resources" == "build" ]] || usage
            ;;
        v)
            version=$OPTARG
            [[ "$version" == "val" ||  "$version" == "rev" ]] || usage
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
build_prefix=$LIMA_BUILD_DIR/$current_branch
source_dir=$PWD

if [[ $version = "rev" ]]; then
release="$current_timestamp-$current_revision"
else
release="1"
fi

if [[ $parallel = "true" ]]; then
j=`grep -c ^processor /proc/cpuinfo`
echo "Parallel build on $j processors"
else
echo "Linear build"
j="1"
fi

# export VERBOSE=1
if [[ $mode == "release" ]]; then
cmake_mode="Release"
else
cmake_mode="Debug"
fi

echo "version='$release'"
install -d $build_prefix/$mode/$current_project
pushd $build_prefix/$mode/$current_project
cmake -DCMAKE_BUILD_TYPE:STRING=$cmake_mode -DLIMA_RESOURCES:PATH="$resources" -DLIMA_VERSION_RELEASE:STRING="$release" -DCMAKE_INSTALL_PREFIX:PATH=$LIMA_DIST $source_dir

make -j$j && [ $current_project != "lima" ] && make test && make install
result=$?
popd

exit $result
