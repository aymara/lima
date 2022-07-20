# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

cat $1 | sort | uniq -c | awk -F" " '{print $2"	"$1}' > $2
