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
set (DEPENDENCIES airports.lst cities.lst greeklocations.lst countriesAndRegions.lst location.rules locationKey.lst locationPre.lst miscLocations.lst stateAbbrev.lst states.lst streetKey.lst)

SPECIFICENTITIES(main eng Location 
  LOCATION-eng.rules 
  LOCATION_COMP-eng.rules 
  PreLOCATION-eng.rules 
  PreLOCATION_COMP-eng.rules 
  LOCATION_RATP-eng.rules)

SPECIFICENTITIES(post eng Location 
  PostLOCATION-eng.rules 
  PostLOCATION_COMP-eng.rules)
