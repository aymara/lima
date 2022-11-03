# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

##!bin/sh

source /home/leffe/Lima/leffe.env
cd /home/leffe/Lima/Sources/linguisticData/analysisDictionary
./build.sh fre && ./deploy.sh fre
