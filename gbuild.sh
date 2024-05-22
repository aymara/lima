#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#Fail if anything goes wrong
# set -o errexit
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
  -m mode           <(Debug)|Release|RelWithDebInfo> compile mode
  -n arch           <(generic)|native> target architecture mode
  -p package        <(OFF)|ON> package building selection
  -r resources      <precompiled|(build)|none> build the linguistic resources or use the
                    precompiled ones
  -s                Do not shorten PoS corpora to speed up compilation.
  -t tests          <(OFF)|ON> run unit tests after install
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
WITH_PACK="OFF"
RUN_UNIT_TESTS="OFF"
SHORTEN_POR_CORPUS_FOR_SVMLEARN="ON"
USE_TF=true
TF_SOURCES_PATH=""
WITH_GUI="ON"
LIMA_SOURCES=$PWD
QT_VERSION_MAJOR="6"

while getopts ":d:m:n:r:v:G:a:p:P:st:Tj:g:" o; do
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
            [[ "$resources" == "precompiled" || "$resources" == "build" || "$resources" == "none" ]] || usage
            ;;
        s)
            SHORTEN_POR_CORPUS_FOR_SVMLEARN="OFF"
            ;;
        t)
            RUN_UNIT_TESTS=${OPTARG}
            [[ "x$RUN_UNIT_TESTS" == "xON" || "x$RUN_UNIT_TESTS" == "xOFF" ]] || usage
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
    if [[ `uname` == "Darwin" ]]; then
      CORE_COUNT=`sysctl -a | grep core_count | sed -e "s/machdep.cpu.core_count: //"`
    else
      CORE_COUNT=`grep -c ^processor /proc/cpuinfo`
    fi
    j=$CORE_COUNT
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

if [[ $resources == "build" ]]; then
  WITH_LIMA_RESOURCES="ON"
else
  WITH_LIMA_RESOURCES="OFF"
fi
 
if [[ $CMAKE_GENERATOR == "Unix" ]]; then
  make_cmd="make -j$j"
  make_test="make test"
  make_install="make install"
  make_package="make package"
  generator="Unix Makefiles"
elif [[ $CMAKE_GENERATOR == "Ninja" ]]; then
  make_cmd="ninja -j $j"
  make_test="ctest"
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

echo "version='$version' release='$release'"

if [[ $CMAKE_GENERATOR == "VS" ]]; then
  #consider linking this current place to $LIMA_BUILD_DIR if different
  #this could be usefull to trick windows path length limitation
  #when building with VS
  mkdir -p $LIMA_BUILD_DIR
  pushd $LIMA_BUILD_DIR
else
  mkdir -p $build_prefix/$mode-$WITH_ASAN/$current_project
  pushd $build_prefix/$mode-$WITH_ASAN/$current_project
fi

if [ "$USE_TF" = false ] ; then
  TF_SOURCES_PATH=""
else
  if [ ${#TF_SOURCES_PATH} -le 0 ] ; then
    TF_SOURCES_PATH=$build_prefix/$mode-$WITH_ASAN/$current_project/extern/tensorflow-for-lima/
    install -d $build_prefix/$mode-$WITH_ASAN/$current_project/extern
    docker pull aymara/manylinux_2_28_with_tensorflow_for_lima_1_9:latest
    docker create -ti --name dummy aymara/manylinux_2_28_with_tensorflow_for_lima_1_9:latest bash
    docker cp dummy:/tensorflow_for_lima $build_prefix/$mode-$WITH_ASAN/$current_project/extern/tensorflow-for-lima
    docker rm -f dummy
    install -d $LIMA_DIST/extern/lib
    install $TF_SOURCES_PATH/lib/libtensorflow-for-lima.so $LIMA_DIST/extern/lib
  fi

  echoerr "Path to TensorFlow sources: $TF_SOURCES_PATH"
fi

LIBTORCH_PATH=${LIMA_SOURCES}/extern/libtorch/
echo "libTorch: " $LIBTORCH_PATH

# export LSAN_OPTIONS=suppressions=${LIMA_SOURCES}/suppr.txt
export ASAN_OPTIONS=halt_on_error=0,fast_unwind_on_malloc=0

echoerr "Launching cmake from $PWD on $source_dir WITH_ASAN=$WITH_ASAN"
cmake  -G "$generator" \
    -DQT_VERSION_MAJOR=${QT_VERSION_MAJOR} \
    -DWITH_DEBUG_MESSAGES=$WITH_DEBUG_MESSAGES \
    -DWITH_ARCH=$WITH_ARCH \
    -DWITH_ASAN=$WITH_ASAN \
    -DSHORTEN_POR_CORPUS_FOR_SVMLEARN=$SHORTEN_POR_CORPUS_FOR_SVMLEARN \
    -DCMAKE_BUILD_TYPE:STRING=$cmake_mode \
    -DLIMA_RESOURCES:PATH="$resources" \
    -DLIMA_VERSION_RELEASE:STRING="$release" \
    -DCMAKE_INSTALL_PREFIX:PATH=$LIMA_DIST \
    -DTF_SOURCES_PATH:PATH=$TF_SOURCES_PATH \
    -DWITH_GUI=$WITH_GUI \
    -DCMAKE_PREFIX_PATH=$LIBTORCH_PATH \
    -DWITH_LIMA_RESOURCES=$WITH_LIMA_RESOURCES \
    -DCTEST_OUTPUT_ON_FAILURE="ON" \
    $source_dir
result=$?
if [ "$result" != "0" ]; then echoerr "Failed to configure LIMA."; popd; exit $result; fi

echoerr "Running make command:"
echo "$make_cmd"
eval $make_cmd
result=$?
if [ "$result" != "0" ]; then echoerr "Failed to build LIMA."; popd; exit $result; fi

echoerr "Running make install:"
echo "$make_install"
eval $make_install
result=$?
if [ "$result" != "0" ]; then echoerr "Failed to install LIMA."; popd; exit $result; fi

if [ $RUN_UNIT_TESTS == "ON" ];
#if [[ $OSTYPE == ’darwin’* ]] ;
then
echoerr "Running make test:"
eval $make_test
result=$?
echoerr "Done make test:"
if [ "$result" != "0" ];
then
  echoerr "Some LIMA tests failed."
  ctest --rerun-failed --output-on-failure
  popd
  exit $result
fi
fi

if [ $WITH_PACK == "ON" ];
then
  echoerr "Running make package:"
  echo "$make_package"
  eval $make_package
  result=$?
  if [ "$result" != "0" ]; then echoerr "Failed to package LIMA."; popd; exit $result; fi
fi

if [[ ( $CMAKE_GENERATOR == "Ninja" || $CMAKE_GENERATOR == "Unix" ) && "x$cmake_mode" == "xRelease" ]] ;
then
  echoerr "Install packages:"
  install -d $LIMA_DIST/share/apps/lima/packages
  if compgen -G "*/src/*-build/*.rpm" > /dev/null; then
    echo "Install RPM packages into $LIMA_DIST/share/apps/lima/packages"
    install */src/*-build/*.rpm $LIMA_DIST/share/apps/lima/packages
  fi
  if compgen -G "*/src/*-build/*.deb" > /dev/null; then
    echo "Install DEB packages into $LIMA_DIST/share/apps/lima/packages"
    install */src/*-build/*.deb $LIMA_DIST/share/apps/lima/packages
  fi
fi

echoerr "Built LIMA successfully.";
popd

exit $result


