# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

ADD_SA_RULES_DEPENDS(fre
        chainsMatrix-fre.bin
        categoriesClassesDeclaration-fre.txt
        compoundTenses-fre.bin
)

set(FRE_RULES_FILES
  rules-fre-heterodeps.txt
  rules-fre-homodeps-pass1.txt
  rules-fre-homodeps-pass2.txt
  rules-fre-homodeps-pass0.txt
  rules-fre-pleonasticPronouns.txt
  rules-fre-heterodeps.txt
  rules-fre-heterodeps-specializer.txt
  rules-fre-dummy.txt
  rules-compoundTense.txt
)
COMPILE_SA_RULES_WRAPPER (fre ${FRE_RULES_FILES})


# install(
#   FILES
#     selectionalPreferences-fre.bin
#   COMPONENT fre DESTINATION share/apps/lima/resources/SyntacticAnalysis
# )
