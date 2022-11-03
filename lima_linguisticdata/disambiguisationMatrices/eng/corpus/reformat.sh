# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

awk -F "|" '{ print $2"#"$3 }' $1 | ../../../scripts/niemesTokens.pl -osep="	" "#" 1,3 | sed -r -e 's/ *	/	/g' -e 's/^ *//'
