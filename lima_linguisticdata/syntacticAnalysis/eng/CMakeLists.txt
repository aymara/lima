# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
