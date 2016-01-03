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
set (DEPENDENCIES annonceurOrg.lst carMakers.lst computers.lst organization.lst ../../common/organizationCommon.lst carMakers.rules journaux_fr.rules newspapersAndMedia.rules organizations.rules partis_politiques.rules)

SPECIFICENTITIES(main fre Organization ORGANIZATION-fre.rules ORGANIZATION_COMP-fre.rules)

add_dependencies(rules-Organization-fre-main rules-fre-Location-configEnv-main2)
add_dependencies(rules-Organization-fre-main rules-fre-Organization-configEnv-main)
