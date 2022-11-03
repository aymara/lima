# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

LIBTORCH_PATH=$1

SRC_ROOT=$PWD
for conf in Debug Release #RelWithDebInfo
do
  mkdir -p build/$conf
  cd build/$conf
  cmake ../.. -DCMAKE_BUILD_TYPE=$conf -DCMAKE_PREFIX_PATH=$LIBTORCH_PATH
  cmake --build . -- -j
  cd $SRC_ROOT
done
