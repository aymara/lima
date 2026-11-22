# SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

set (DEPENDENCIES organization.lst organizations.rules partis_politiques.rules
  ../../fre/Organization/annonceurOrg.lst ../../fre/Organization/carMakers.lst
  ../../fre/Organization/computers.lst ../../common/organizationCommon.lst
  ../../fre/Organization/carMakers.rules ../../fre/Organization/journaux_fr.rules
  ../../fre/Organization/newspapersAndMedia.rules)

SPECIFICENTITIES(main ud-fra Organization
  ORGANIZATION-ud-fra.rules)
