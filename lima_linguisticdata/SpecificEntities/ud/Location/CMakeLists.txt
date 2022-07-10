# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES airports.lst cities.lst greeklocations.lst countriesAndRegions.lst location.rules locationKey.lst locationPre.lst miscLocations.lst stateAbbrev.lst states.lst streetKey.lst)

SPECIFICENTITIES(main ud Location 
  LOCATION-ud.rules 
  PreLOCATION-ud.rules 
  LOCATION_RATP-ud.rules)

SPECIFICENTITIES(post ud Location 
  PostLOCATION-ud.rules)
