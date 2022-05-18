# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

echo "CALLING: testDict16 --charSize=2 --listOfWords=$1 --output=$2 > output"
time testDict16 --charSize=2 --listOfWords=$1 --output=$2 > output
# hash
time testDict16 --charSize=2 --input=$2 --spare --output=$2 >> output
