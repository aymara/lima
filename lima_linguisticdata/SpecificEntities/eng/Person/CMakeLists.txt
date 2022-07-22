# Copyright 2002-2018 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES firstname.lst function_title.lst person.lst person_ending.lst title.lst speech_verb.lst wd_firstname.lst)

SPECIFICENTITIES(main eng Person
  PERSON-eng.rules
  EMBEDED-eng.rules)
