# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES ../../common/organizationCommon.lst newspaperKey.lst newspapersAndMedia.rules organization.lst organizationkey.lst organization.rules AutomobileManu.lst companies.lst wd_organization.lst organizationType.lst)

SPECIFICENTITIES(main ud-eng Organization
  ORGANIZATION-ud-eng.rules)
