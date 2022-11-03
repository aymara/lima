# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

##!bin/sh

source /home/leffe/Lima/leffe.env
cd /home/leffe/Lima/Sources/linguisticData/rules-idiom/fre
make clean && make && make install
