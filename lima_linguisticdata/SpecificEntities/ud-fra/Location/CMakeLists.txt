# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES regionsAndCountries.lst
  ../../fre/Location/annonceurLieu.lst ../../fre/Location/cities.lst
  ../../fre/Location/greeklocations.lst ../../fre/Location/highways.lst
  ../../fre/Location/miscLocations.lst ../../fre/Location/riversAndSeas.lst)

SPECIFICENTITIES(main ud-fra Location
  PreLOCATION-ud-fra.rules
  LOCATION-ud-fra.rules
  LOCATION_RATP-ud-fra.rules
  PostLOCATION-ud-fra.rules)
