# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES annonceurLieu.lst cities.lst greeklocations.lst highways.lst miscLocations.lst regionsAndCountries.lst riversAndSeas.lst)

SPECIFICENTITIES(main fre Location
  PreLOCATION-fre.rules
  LOCATION-fre.rules
  LOCATION_RATP-fre.rules
  PostLOCATION-fre.rules)
