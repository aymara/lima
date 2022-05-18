# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES airports.lst cities.lst greeklocations.lst countries.lst location.rules locationKey.lst locationPre.lst miscLocations.lst stateAbbrev.lst states.lst streetKey.lst)

SPECIFICENTITIES(main eng Location 
  LOCATION-eng.rules 
  PreLOCATION-eng.rules 
  LOCATION_RATP-eng.rules)

SPECIFICENTITIES(post eng Location 
  PostLOCATION-eng.rules)
