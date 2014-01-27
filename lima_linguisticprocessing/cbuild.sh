#   Copyright 2002-2013 CEA LIST
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

if [[ $1 == "debug" ]]; then
    install -d debug
    pushd debug
    cmake -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Debug ..
    make -j2 && \
        make install
    result=$?
    popd
else if [[ $1 == "release" ]]; then
    install -d release
    pushd release
    cmake -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Release ..
    make -j2 && \
        make install
    result=$?
    popd
else 
    # default is : compile in debug mode in build/ directory
    install -d build
    pushd build
    cmake -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Debug ..
    make -j2 && \
        make install
    result=$?
    popd
fi
fi

exit $result
