# Copyright (C) 2012 Samsung Electronics
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

find_package(PkgConfig)
pkg_check_modules(PC_ENCHANT enchant)

find_path(ENCHANT_INCLUDE_DIRS
    NAMES enchant.h
    PATHS ${PC_ENCHANT_INCLUDEDIR}
          ${PC_ENCHANT_INCLUDE_DIRS}
)

find_library(ENCHANT_LIBRARIES
    NAMES enchant
    PATHS ${PC_ENCHANT_LIBDIR}
          ${PC_ENCHANT_LIBRARY_DIRS}
)

# handle the QUIETLY and REQUIRED arguments and set ENCHANT_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Enchant  DEFAULT_MSG  ENCHANT_LIBRARIES ENCHANT_INCLUDE_DIRS)

mark_as_advanced(ENCHANT_LIBRARIES ENCHANT_INCLUDE_DIRS )

