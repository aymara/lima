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
set (DEPENDENCIES annonceurLieu.lst cities.lst greeklocations.lst highways.lst miscLocations.lst regionsAndCountries.lst riversAndSeas.lst)

SPECIFICENTITIES(main fre Location LOCATION-fre.rules LOCATION_COMP-fre.rules LOCATION_RATP-fre.rules PostLOCATION-fre.rules PostLOCATION_COMP-fre.rules)
