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

#Fail if anything goes wrong 
set -e

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
  -G Generator <(Unix)|MSYS|NMake|VS> which cmake generator to use.  
EOF
exit 1
}

[ -z "$LIMA_BUILD_DIR" ] && echo "Need to set LIMA_BUILD_DIR" && exit 1;
[ -z "$LIMA_DIST" ] && echo "Need to set LIMA_DIST" && exit 1;

mode="debug"
version="val"
resources="build"
parallel="true"
CMAKE_GENERATOR="Unix"

while getopts ":m:p:r:v:G:" o; do
    case "${o}" in
        m)
            mode=${OPTARG}
            [[ "$mode" == "debug" || "$mode" == "release" ]] || usage
            ;;
        G)
          CMAKE_GENERATOR=${OPTARG}
          echo "CMAKE_GENERATOR=$CMAKE_GENERATOR"
          [[     "$CMAKE_GENERATOR" == "Unix"  || 
                 "$CMAKE_GENERATOR" == "MSYS"  ||
                 "$CMAKE_GENERATOR" == "NMake" ||
                 "$CMAKE_GENERATOR" == "VS"
          ]] || usage
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
current_project_name="`head -n1 CMakeLists.txt`"
build_prefix=$LIMA_BUILD_DIR/$current_branch
source_dir=$PWD

if [[ $version = "rev" ]]; then
release="$current_timestamp-$current_revision"
else
release="2"
fi

if [[ $parallel = "true" ]]; then
#j=`grep -c ^processor /proc/cpuinfo`
j=`WMIC CPU Get NumberOfCores | head -n 2 | tail -n 1 | sed -n "s/\s//gp"`
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

if [[ $CMAKE_GENERATOR == "Unix" ]]; then
  make_cmd="make -j$j"
  generator="Unix Makefiles"
elif [[ $CMAKE_GENERATOR == "MSYS" ]]; then
  make_cmd="make -j$j"
  generator="MSYS Makefiles"
elif [[ $CMAKE_GENERATOR == "NMake" ]]; then
  make_cmd="nmake && exit 0"
  generator="NMake Makefiles"
elif [[ $CMAKE_GENERATOR == "VS" ]]; then
  make_cmd="""
  pwd &&
  MSBuild.exe -p:configuration=$mode -t:Build lima_common-prefix/src/lima_common-build/limacommon.sln &&
  MSBuild.exe -p:configuration=$mode -t:Build lima_linguisticprocessing-prefix/src/lima_linguisticprocessing-build/limalinguisticprocessing.sln
  """
  generator="Visual Studio 10 2010"
else
  make_cmd="make -j$j"
fi

echo "version='$release'"
mkdir -p $build_prefix/$mode/$current_project
pushd $build_prefix/$mode/$current_project
#consider linking this current place to $LIMA_BUILD_PATH if different
#this could be usefull to trick windows path length limitation
#when building with VS
mkdir -p $LIMA_BUILD_PATH
pushd $LIMA_BUILD_PATH

echo "LIMA_BUILD_PATH=$LIMA_BUILD_PATH"
echo "Launching cmake"
cmake  -G "$generator" -DCMAKE_BUILD_TYPE:STRING=$cmake_mode -DLIMA_RESOURCES:PATH="$resources" -DLIMA_VERSION_RELEASE:STRING="$release" -DCMAKE_INSTALL_PREFIX:PATH=$LIMA_DIST $source_dir

echo "Running command:"
echo "$make_cmd"
eval $make_cmd

#make -j$j 
#nmake
result=$?

exit $result

if [ "x$current_project_name" != "xproject(Lima)" ];
then
  #make test && make install
  nmake test && nmake install
  result=$?
fi

popd

exit $result
