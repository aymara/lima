# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#add_subdirectory(applyAutomaton) # n'est pas compile dans S2!!!
add_subdirectory(automatonCompiler)
add_subdirectory(common) # executable de s2common
add_subdirectory(dictionary)
add_subdirectory(evalCoref)
add_subdirectory(normalize)
# add_subdirectory(lima-lp-tgv) # --> src/linguisticProcessing/common/lima-lp-tgv
add_subdirectory(tva)
add_subdirectory(tvx)

include_directories(${CMAKE_SOURCE_DIR}/src )

########### install files ###############
