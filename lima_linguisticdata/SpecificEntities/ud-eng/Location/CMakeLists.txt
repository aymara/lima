# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES airports.lst cities.lst greeklocations.lst countriesAndRegions.lst location.rules locationKey.lst locationPre.lst miscLocations.lst stateAbbrev.lst states.lst streetKey.lst)

SPECIFICENTITIES(main ud-eng Location
  LOCATION-ud-eng.rules
  PreLOCATION-ud-eng.rules
  LOCATION_RATP-ud-eng.rules)

SPECIFICENTITIES(post ud-eng Location
  PostLOCATION-ud-eng.rules)
