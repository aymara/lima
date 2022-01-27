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
set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

echoerr() { printf "\e[31;1m%s\e[0m\n" "$*" >&2; }

usage()
{
cat << EOF 1>&2; exit 1;
Synopsis: $0 [OPTIONS]

Options default values are in parentheses.

  -a asan           <(OFF)|ON> compile with adress sanitizer
  -d debug-messages <(OFF)|ON> compile with debug messages on in release mode
  -j n              <INTEGER> set the compilation to a number of parallel processes.
                    Default 0 => the value is derived from CPUs available.
  -k packages       <(ON)|OFF> build packages if ON
  -m mode           <(Debug)|Release|RelWithDebInfo> compile mode
  -n arch           <(generic)|native> target architecture mode
  -p package        <OFF|(ON)> package building selection
  -r resources      <precompiled|(build)> build the linguistic resources or use the
                    precompiled ones
  -s                Do not shorten PoS corpora to speed up compilation.
  -v version        <(val)|rev> version number is set either to the value set by
                    config files or to the short git sha1
  -G Generator      <(Ninja)|Unix|MSYS|NMake|VS> which cmake generator to use.
  -T                Do not use TensorFlow (default is to use it)
  -P tfsrcpath      <> Path to TensorFlow sources
  -g gui            <OFF|(ON)> compile with GUI
EOF
exit 1
}

[ -z "$LIMA_BUILD_DIR" ] && echo "Need to set LIMA_BUILD_DIR" && exit 1;
[ -z "$LIMA_DIST" ] && echo "Need to set LIMA_DIST" && exit 1;

arch="generic"
j="0"
WITH_DEBUG_MESSAGES="OFF"
mode="Debug"
version="val"
resources="build"
CMAKE_GENERATOR="Ninja"
WITH_ASAN="OFF"
WITH_ARCH="OFF"
WITH_PACK="ON"
SHORTEN_POR_CORPUS_FOR_SVMLEARN="ON"
USE_TF=true
TF_SOURCES_PATH=""
WITH_GUI="ON"
LIMA_SOURCES=$PWD

while getopts ":d:m:n:r:v:G:a:p:P:sTj:g:" o; do
    case "${o}" in
        a)
            WITH_ASAN=${OPTARG}
            [[ "$WITH_ASAN" == "ON" || "$WITH_ASAN" == "OFF" ]] || usage
            ;;
        d)
            WITH_DEBUG_MESSAGES=${OPTARG}
            [[ "x$WITH_DEBUG_MESSAGES" == "xON" || "x$WITH_DEBUG_MESSAGES" == "xOFF" ]] || usage
            ;;
        j)
            j=${OPTARG}
            [[ -n "${j##*[!0-9]*}" ]] || usage
            ;;
        m)
            mode=${OPTARG}
            [[ "$mode" == "Debug" || "$mode" == "Release"  || "$mode" == "RelWithDebInfo" ]] || usage
            ;;
        n)
            arch=${OPTARG}
            [[ "x$arch" == "xnative" || "x$arch" == "xgeneric" ]] || usage
            ;;
        p)
            WITH_PACK=${OPTARG}
            [[ "$WITH_PACK" == "ON" || "$WITH_PACK" == "OFF" ]] || usage
            ;;
        r)
            resources=${OPTARG}
            [[ "$resources" == "precompiled" || "$resources" == "build" ]] || usage
            ;;
        s)
            SHORTEN_POR_CORPUS_FOR_SVMLEARN="OFF"
            ;;
        v)
            version=$OPTARG
            [[ "$version" == "val" ||  "$version" == "rev" ]] || usage
            ;;
        G)
            CMAKE_GENERATOR=${OPTARG}
            echo "CMAKE_GENERATOR=$CMAKE_GENERATOR"
            [[     "$CMAKE_GENERATOR" == "Unix"  ||
                   "$CMAKE_GENERATOR" == "Ninja" ||
                   "$CMAKE_GENERATOR" == "MSYS"  ||
                   "$CMAKE_GENERATOR" == "NMake" ||
                   "$CMAKE_GENERATOR" == "VS"
            ]] || usage
            ;;
        P)
            TF_SOURCES_PATH=$OPTARG
            ;;
        T)
            USE_TF=false
            ;;
        g)
            WITH_GUI=${OPTARG}
            echo "WITH_GUI=$WITH_GUI"
            [[ "$WITH_GUI" == "ON" || "$WITH_GUI" == "OFF" ]] || usage
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if type git && git rev-parse --git-dir; then
    current_branch=$(git rev-parse --abbrev-ref HEAD)
    current_revision=$(git rev-parse --short HEAD)
    current_timestamp=$(git show -s --format=%cI HEAD | sed -e 's/[^0-9]//g')
else
    # use default values
    current_branch="default"
    current_revision="default"
    current_timestamp=1
fi
current_project=`basename $PWD`
current_project_name="$(head -n2 CMakeLists.txt | tail -n1)"
build_prefix=$LIMA_BUILD_DIR/$current_branch
source_dir=$PWD

if [[ $version = "rev" ]]; then
  release="$current_timestamp-$current_revision"
else
  release="0"
fi

if [[ "$j" == "0" ]]; then
  if [[ $CMAKE_GENERATOR == "VS" ]]; then
    j=`WMIC CPU Get NumberOfCores | head -n 2 | tail -n 1 | sed -n "s/\s//gp"`
  elif [[ $CMAKE_GENERATOR == "Unix" || $CMAKE_GENERATOR == "Ninja" ]]; then
    j=`grep -c ^processor /proc/cpuinfo`
  fi
fi
if [[ "$j" == "1" ]]; then
  echoerr "Linear build"
else
  echoerr "Parallel build on $j processors"
fi

# export VERBOSE=1
if [[ $mode == "Release" ]]; then
  cmake_mode="Release"
elif [[ $mode == "RelWithDebInfo" ]]; then
  cmake_mode="RelWithDebInfo"
else
  cmake_mode="Debug"
fi

if [[ $arch == "native" ]]; then
  WITH_ARCH="ON"
else
  WITH_ARCH="OFF"
fi

if [[ $CMAKE_GENERATOR == "Unix" ]]; then
  make_cmd="make -j$j"
  make_test="make test"
  make_install="make install"
  make_package="make package"
  generator="Unix Makefiles"
elif [[ $CMAKE_GENERATOR == "Ninja" ]]; then
  make_cmd="ninja -j $j"
  make_test="ninja test"
  make_install="ninja install"
  make_package="ninja package"
  generator="Ninja"
elif [[ $CMAKE_GENERATOR == "MSYS" ]]; then
  make_cmd="make -j$j"
  make_test="make test"
  make_install="make install"
  make_package="make package"
  generator="MSYS Makefiles"
elif [[ $CMAKE_GENERATOR == "NMake" ]]; then
  make_cmd="nmake && exit 0"
  make_test="nmake test"
  make_install="nmake install"
  make_package=""
  generator="NMake Makefiles"
elif [[ $CMAKE_GENERATOR == "VS" ]]; then
  make_cmd="""
  pwd &&
  cmake --build . --config $cmake_mode
  """
  make_test=""
  make_install=""
  generator="Visual Studio 14 2015 Win64"
else
  make_cmd="make -j$j"
fi

echo "version='$release'"

if [[ $CMAKE_GENERATOR == "VS" ]]; then
  #consider linking this current place to $LIMA_BUILD_DIR if different
  #this could be usefull to trick windows path length limitation
  #when building with VS
  mkdir -p $LIMA_BUILD_DIR
  pushd $LIMA_BUILD_DIR
else
  mkdir -p $build_prefix/$mode/$current_project
  pushd $build_prefix/$mode/$current_project
fi

if [ "$USE_TF" = false ] ; then
  TF_SOURCES_PATH=""
else
  if [ ${#TF_SOURCES_PATH} -le 0 ] ; then
    TF_SOURCES_PATH=/usr/include/tensorflow-for-lima/
  fi

  echoerr "Path to TensorFlow sources: $TF_SOURCES_PATH"
fi

LIBTORCH_PATH=${LIMA_SOURCES}/extern/libtorch/
echo "libTorch: " $LIBTORCH_PATH

# export LSAN_OPTIONS=suppressions=${LIMA_SOURCES}/suppr.txt
export ASAN_OPTIONS=halt_on_error=0,fast_unwind_on_malloc=0

echoerr "Launching cmake from $PWD"
cmake  -G "$generator" \
    -DWITH_DEBUG_MESSAGES=$WITH_DEBUG_MESSAGES \
    -DWITH_ARCH=$WITH_ARCH \
    -DWITH_ASAN=$WITH_ASAN \
    -DSHORTEN_POR_CORPUS_FOR_SVMLEARN=$SHORTEN_POR_CORPUS_FOR_SVMLEARN \
    -DCMAKE_BUILD_TYPE:STRING=$cmake_mode \
    -DLIMA_RESOURCES:PATH="$resources" \
    -DLIMA_VERSION_RELEASE:STRING="$release" \
    -DCMAKE_INSTALL_PREFIX:PATH=$LIMA_DIST \
    -DTF_SOURCES_PATH:PATH=$TF_SOURCES_PATH \
    -DWITH_GUI=$WITH_GUI $source_dir \
    -DCMAKE_PREFIX_PATH=$LIBTORCH_PATH
result=$?
if [ "$result" != "0" ]; then echorr "Failed to configure LIMA."; popd; exit $result; fi


echoerr "Running make command:"
echo "$make_cmd"
eval $make_cmd
result=$?
if [ "$result" != "0" ]; then echorr "Failed to build LIMA."; popd; exit $result; fi

echoerr "Running make test:"
echo "$make_test"
eval $make_test
result=$?
if [ "$result" != "0" ]; then echoerr "Failed to build LIMA."; popd; exit $result; fi

echoerr "Running make install:"
echo "$make_install"
eval $make_install
result=$?
if [ "$result" != "0" ]; then echoerr "Failed to build LIMA."; popd; exit $result; fi

if [ $WITH_PACK == "ON" ];
then
  echoerr "Running make package:"
  echo "$make_package"
  eval $make_package
  result=$?
  if [ "$result" != "0" ]; then echoerr "Failed to build LIMA."; popd; exit $result; fi
fi

if [ $CMAKE_GENERATOR == "Unix" ] && [ "x$cmake_mode" == "xRelease" ] ;
then
  echoerr "Install package:"
  install -d $LIMA_DIST/share/apps/lima/packages
  if compgen -G "*/src/*-build/*.rpm" > /dev/null; then
    install */src/*-build/*.rpm $LIMA_DIST/share/apps/lima/packages
  fi
  if compgen -G "*/src/*-build/*.deb" > /dev/null; then
    install */src/*-build/*.deb $LIMA_DIST/share/apps/lima/packages
  fi
fi

echoerr "Built LIMA successfully.";
popd

exit $result


