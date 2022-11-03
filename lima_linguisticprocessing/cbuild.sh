# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

if [[ $1 == "debug" ]]; then
    install -d debug
    pushd debug
    cmake -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Debug ..
    make -j2 && \
        make install
    result=$?
    popd
else if [[ $1 == "release" ]]; then
    install -d release
    pushd release
    cmake -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Release ..
    make -j2 && \
        make install
    result=$?
    popd
else
    # default is : compile in debug mode in build/ directory
    install -d build
    pushd build
    cmake -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Debug ..
    make -j2 && \
        make install
    result=$?
    popd
fi
fi

exit $result
