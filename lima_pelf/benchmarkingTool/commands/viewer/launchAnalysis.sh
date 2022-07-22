#!/bin/bash

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

echo "I got the analysis! $1 $2 $PWD"

syanot ref/$1.easyref.xml >& /dev/null &
syanot output/$1.aligned.easy.xml >& /dev/null &

tmpfile=`mktemp leffe-XXXXXX`

xpath text/$1 "//E[@id=\"$2\"]/text()" | recode latin1..utf8 > $tmpfile

analyzeText -l fre -p easy -d fullXmlDumper $tmpfile

kgraphviewer $tmpfile.dot >& /dev/null &
kgraphviewer $tmpfile.sent0.sa.dot >& /dev/null &

kate -u $tmpfile.anal.xml
