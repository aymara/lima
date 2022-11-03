# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES annonceurOrg.lst carMakers.lst computers.lst organization.lst ../../common/organizationCommon.lst carMakers.rules journaux_fr.rules newspapersAndMedia.rules organizations.rules partis_politiques.rules)

SPECIFICENTITIES(main fre Organization 
  ORGANIZATION-fre.rules)

