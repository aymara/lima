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
  -t buildtype  <text|textse|(all)> either build only linguistic analysis modules
                (text), text only search engine (textse) or all amose (all)
  -v version    <(val)|rev> version number is set either to the value set by  
                config files or to the short git sha1
EOF
exit 1
}

mode="debug"
version="val"
buildtype="all"
resources="build"
parallel="true"

while getopts ":m:p:r:t:v:" o; do
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
        t)
            buildtype=${OPTARG}
            [[ "$buildtype" == "text" || "$buildtype" == "textse" || "$buildtype" == "all" ]] || usage
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

if [[ $mode == "release" ]]; then
echo "version='$version'"
    install -d $build_prefix/release/$current_project
    pushd $build_prefix/release/$current_project
    cmake -DLIMA_RESOURCES="$resources" -DLIMA_BUILD_TYPE="$buildtype" -DLIMA_VERSION_RELEASE="$release" -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Release $source_dir
    make -j$j && make test && make install && make package
    result=$?
    popd
else
    # default is : compile in debug mode in debug/ directory
    install -d $build_prefix/debug/$current_project
    pushd $build_prefix/debug/$current_project
    cmake -DLIMA_RESOURCES="$resources" -DLIMA_BUILD_TYPE="$buildtype" -DLIMA_VERSION_RELEASE=$release -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Debug $source_dir
    make -j$j && make test && make install
    result=$?
    popd
fi

exit $result
