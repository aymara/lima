# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

ADD_SA_RULES_DEPENDS(por
#         chainsMatrix-por.bin
        categoriesClassesDeclaration-por.txt
)


set(POR_RULES_FILES
  rules-por-homodeps-pass1.txt
  rules-por-homodeps-pass2.txt
#   rules-por-pleonasticPronouns.txt
#   rules-por-heterodeps.txt
#   rules-por-dummy.txt
)

COMPILE_SA_RULES_WRAPPER (por ${POR_RULES_FILES})
