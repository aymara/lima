# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

install -d build
pushd build
cmake -DCMAKE_INSTALL_PREFIX=$AMOSE_DIST -DCMAKE_BUILD_TYPE=Debug ..
make && make install
result=$?
popd

exit $result
