# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### next target ###############

SET(convertMultXmlToMultBin_SRCS
   convertMultXmlToMultBin.cpp
   MultXmlReader.cpp
)

add_executable(convertMultXmlToMultBin ${convertMultXmlToMultBin_SRCS})

target_link_libraries(convertMultXmlToMultBin
lima-common-data
lima-common-factory
lima-common-fsaaccess
lima-common-mediaticdata
lima-common-misc
lima-common-tools
lima-common-tools
lima-common-xmlconfigurationfiles

lima-lp-analysishandlers
lima-lp-bagofwords
lima-lp-client
lima-lp-linguisticdata
lima-lp-misc
lima-lp-propertycode

${Boost_LIBRARIES}
${QT_LIBRARIES}
${optionalLibs})

install(TARGETS convertMultXmlToMultBin DESTINATION bin)


