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
execute_process(
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/copy_rules.py --mapping ../pos_mapping.txt --from eng --to ud-eng ../../eng/Numex
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  RESULT_VARIABLE ret)
if(ret EQUAL "1")
    message(FATAL_ERROR "Error in configure: failed to copy rules")
endif()

file(GLOB LST_FILES *.lst)

set (DEPENDENCIES ${LST_FILES})

SPECIFICENTITIES(main ud-eng Numex
  NUMBER-ud-eng.rules
  UNIT-ud-eng.rules
  NUMEX-ud-eng.rules)
