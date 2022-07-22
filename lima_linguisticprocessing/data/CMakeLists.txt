# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############

IF(TRE_FOUND)
  install(
    FILES test-eng.approx.xml
    DESTINATION share/apps/lima/tests
    COMPONENT runtime
  )
ENDIF(TRE_FOUND)

    #test-eng.compounds.xml
install(
  FILES
    test-eng.abbrev.xml
    test-eng.default.xml
    test-eng.disambiguated.xml
    test-eng.hyphen.xml
    test-eng.idiom.xml
    test-eng.sa.xml
    test-eng.se.xml
    test-eng.se-PERSON.xml
    test-eng.se-DATE.xml
    test-eng.se-EMBEDED.xml
    test-eng.se-LOCATION.xml
    test-eng.se-ORGANISATION.xml
    test-eng.simpleword.xml
    test-eng.tokenizer.xml
    test-fre.compounds.xml
    test-fre.default.xml
    test-fre.disambiguated.xml
    test-fre.hyphen.xml
    test-fre.idiom.xml
    test-fre.sa.xml
    test-fre.se.xml
    test-fre.se-PERSON.xml
    test-fre.se-DATE.xml
    test-fre.se-LOCATION.xml
    test-fre.simpleword.xml
    test-fre.tokenizer.xml
    #test-fre.se-beforepos.xml
    test-fre.multilevel.xml
  DESTINATION share/apps/lima/tests
  COMPONENT runtime
)

install(
  FILES
    test-fre.xmlreader.xml
    text-fre.xmlreader.0.xml
    text-fre.xmlreader.1.xml
    text-fre.xmlreader.2.xml
    text-fre.xmlreader.3.xml
    text-fre.xmlreader.4.xml
    text-fre.xmlreader.5.xml
  DESTINATION share/apps/lima/tests/xmlreader
  COMPONENT runtime
)
