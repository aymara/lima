# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

gawk -F"	" '{ print $3 }' $1 >  succession_categs.txt
gawk -F"	" '{ print $2"	"$3 }' $1 >  corpus_ger_merge.txt
