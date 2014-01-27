set encoding=utf8
using modex Organization-modex.xml
using groups Organization
set defaultAction=>CreateSpecificEntity()

# .com companies
com:$NP .::ORGANIZATION:
=>CreateSpecificEntity()

# list of organizations common to all languages
include ../../common/companies.rules

# a list of known organizations
include organization.rules

# newspapers and television
include newspapersAndMedia.rules

use ../Location/cities.lst,../Location/states.lst,../Location/countriesAndRegions.lst

# location based organization
@locationOrg=(
University,
International\ University,
Supreme\ Court,
Circuit\ Court,
Prison,
Police,
Police\ Department
)

@organizationKey=(
Co,
Inc,
AB,
GmbH,
S.A.)


@City::(@locationOrg):ORGANIZATION:
@locationOrg::of? (@City|@State):ORGANIZATION:
@State::state? (@locationOrg):ORGANIZATION:
@locationOrg:::ORGANIZATION:

university::of (@City|@State):ORGANIZATION:
#university::of (@City|@State) (@City|@State)?:ORGANIZATION:

#------------------------------
# a list of known organizations
use newspaperKey.lst,organization.lst,../../common/organizationCommon.lst

# abbreviated newspaper names
@newspaperPrincipalKey:::ORGANIZATION:

@organization:::ORGANIZATION:

@organizationCommon:::ORGANIZATION:

@organizationKey=(
Co,
Inc,
AB,
GmbH,
S.A.)
