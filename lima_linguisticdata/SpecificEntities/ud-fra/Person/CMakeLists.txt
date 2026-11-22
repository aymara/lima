# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES ../../fre/Person/firstname.lst ../../fre/Person/function_title.lst
  ../../fre/Person/person.lst ../../fre/Person/persons.rules
  ../../fre/Person/prefirstname.lst ../../fre/Person/title.lst)

SPECIFICENTITIES(main ud-fra Person
  PERSON-ud-fra.rules
  EMBEDED-ud-fra.rules)
