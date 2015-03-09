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
ADD_SA_RULES_DEPENDS(eng
        chainsMatrix-eng.bin
        categoriesClassesDeclaration-eng.txt
        compoundTenses-eng.bin
)


set(ENG_RULES_FILES
  rules-eng-homodeps-pass1.txt
  rules-eng-homodeps-pass2.txt
  rules-eng-pleonasticPronouns.txt
  rules-eng-heterodeps.txt
  rules-eng-dummy.txt
)
COMPILE_SA_RULES_WRAPPER (eng ${ENG_RULES_FILES})
