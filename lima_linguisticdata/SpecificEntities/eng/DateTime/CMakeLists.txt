# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES days.lst monthAbbrev.lst months.lst timekey.lst timezone.lst)

SPECIFICENTITIES(main eng DateTime 
  DATE-eng.rules 
  TIME-eng.rules)
