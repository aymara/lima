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
SPECIFICENTITIESEXECENV(por)

SPECIFICENTITIESCONFIGENV(main por DateTime)
SPECIFICENTITIESCONFIGENV(main por Event)
SPECIFICENTITIESCONFIGENV(main por Location)
SPECIFICENTITIESCONFIGENV(main por Numex)
SPECIFICENTITIESCONFIGENV(main por Organization)
SPECIFICENTITIESCONFIGENV(main por Person)
SPECIFICENTITIESCONFIGENV(main por Product)

add_custom_target(
  specificentitiesconfigenv-por-all
  ALL
  DEPENDS rules-por-DateTime-configEnv-main
  rules-por-Event-configEnv-main
  rules-por-Location-configEnv-main
  rules-por-Numex-configEnv-main
  rules-por-Organization-configEnv-main
  rules-por-Person-configEnv-main
  rules-por-Product-configEnv-main
)

add_subdirectory(Resources)
add_subdirectory(Numex)
add_subdirectory(DateTime)
add_subdirectory(Organization)
add_subdirectory(Location)
add_subdirectory(Person)
add_subdirectory(Event)
add_subdirectory(Product)
