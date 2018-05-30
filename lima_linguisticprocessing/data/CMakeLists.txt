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

########### install files ###############

IF(TRE_FOUND)
install(FILES
    test-eng-ud.default.xml 
    test-eng-ud.disambiguated.xml 
    test-eng-ud.simpleword.xml 
    test-eng.abbrev.xml 
    test-eng.default.xml 
    test-eng.approx.xml
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
  COMPONENT runtime)
ELSE(TRE_FOUND)
install(FILES
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
  COMPONENT runtime)
ENDIF(TRE_FOUND)

