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
SPECIFICENTITIESCONFIGENV(main eng DateTime)

SPECIFICENTITIESCONFIGENV(main eng Event)

SPECIFICENTITIESCONFIGENV(main eng Location)

SPECIFICENTITIESCONFIGENV(main eng Numex)

SPECIFICENTITIESCONFIGENV(main eng Organization)

SPECIFICENTITIESCONFIGENV(main eng Person)

SPECIFICENTITIESCONFIGENV(main eng Product)


add_subdirectory(resources)
add_subdirectory(Numex)
add_subdirectory(DateTime)
add_subdirectory(Organization)
add_subdirectory(Location)
add_subdirectory(Person)
add_subdirectory(Event)
add_subdirectory(Product)
