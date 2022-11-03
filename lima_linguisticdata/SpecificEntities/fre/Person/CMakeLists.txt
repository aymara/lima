# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES firstname.lst function_title.lst person.lst persons.rules prefirstname.lst title.lst)

SPECIFICENTITIES(main fre Person 
  PERSON-fre.rules 
  EMBEDED-fre.rules)

