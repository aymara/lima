# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES firstname.lst wd_firstname.lst function_title.lst person.lst person_ending.lst title.lst tussenvoegsel.lst speech_verb.lst wd_lastname.lst)

SPECIFICENTITIES(main ud Person 
  PERSON-ud.rules 
  EMBEDED-ud.rules)
