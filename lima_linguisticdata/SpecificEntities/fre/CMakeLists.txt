#   Copyright 2002-2013 CEA LIST
#
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
SPECIFICENTITIESEXECENV(fre)

SPECIFICENTITIESCONFIGENV(main fre DateTime)
SPECIFICENTITIESCONFIGENV(main fre Event)
SPECIFICENTITIESCONFIGENV(main fre Location)
SPECIFICENTITIESCONFIGENV(main fre Numex)
SPECIFICENTITIESCONFIGENV(main2 fre Location)
SPECIFICENTITIESCONFIGENV(main fre Organization)
SPECIFICENTITIESCONFIGENV(main fre Person)
SPECIFICENTITIESCONFIGENV(main fre Product)

add_custom_target(
  specificentitiesconfigenv-fre-all
  ALL
  DEPENDS rules-fre-DateTime-configEnv-main
  rules-fre-Event-configEnv-main
  rules-fre-Location-configEnv-main
  rules-fre-Location-configEnv-main2
  rules-fre-Numex-configEnv-main
  rules-fre-Organization-configEnv-main
  rules-fre-Person-configEnv-main
  rules-fre-Product-configEnv-main
)

add_subdirectory(resources)
add_subdirectory(Numex)
add_subdirectory(DateTime)
add_subdirectory(Organization)
add_subdirectory(Location)
add_subdirectory(Person)
add_subdirectory(Event)
add_subdirectory(Product)
