# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

cp $1 $3
cat $2 >> $3
awk -F"	" '{print $2}' $3 > succession_categs.txt
