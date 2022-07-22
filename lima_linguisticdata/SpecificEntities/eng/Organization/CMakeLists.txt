# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES
  ../../common/companies.rules
  ../../common/organizationCommon.lst
  ../Location/cities.lst
  ../Location/countriesAndRegions.lst
  ../Location/states.lst
  AutomobileManu.lst
  companies.lst
  newspaperKey.lst
  newspapersAndMedia.rules
  organization.lst
  organization.rules
  organizationkey.lst
  organizationType.lst
  wd_organization.lst
  wd_organization.rules
)
SPECIFICENTITIES(main eng Organization
  ORGANIZATION-eng.rules)


