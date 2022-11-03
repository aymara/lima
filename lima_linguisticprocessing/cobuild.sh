# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

install -d obuild
pushd obuild
cmake -DCMAKE_INSTALL_PREFIX=$LIMA_DIST -DCMAKE_BUILD_TYPE=Release ..
make -j2 && \
make install
popd
