# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES hours.lst timezone.lst)
SPECIFICENTITIES(main fre DateTime 
  DATE-fre.rules 
  TIME-fre.rules)
