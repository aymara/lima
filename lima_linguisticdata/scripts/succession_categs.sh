# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

gawk -F"	" '{ print $3 }' $2 >  succession_categs_retag.txt
gawk -F"	" '{ print $1"	"$2 }' $2 >  corpus_$1_merge.txt
