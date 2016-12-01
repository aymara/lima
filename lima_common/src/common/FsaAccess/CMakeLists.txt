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
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_definitions( -DLIMA_FSAACCESS_EXPORTING )

########### next target ###############

SET(lima-common-fsaaccess_LIB_SRCS
   AbstractLexiconIdAccessor.cpp
   AbstractLexiconIdGenerator.cpp
   FsaAccessBuilder16.cpp
   FsaAccessBuilderRandom16.cpp
   FsaAccessHeader.cpp
   FsaAccessIOStreamWrapper.cpp
   FsaAccessSpare16.cpp
   PrefixIterator.cpp
   lexiconCompoundElement.cpp
   lexiconIdAccessor.cpp
   lexiconIdGenerator.cpp
)

add_library(lima-common-fsaaccess SHARED ${lima-common-fsaaccess_LIB_SRCS})

target_link_libraries(lima-common-fsaaccess 
  lima-common-misc
)

set_target_properties(lima-common-fsaaccess PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-fsaaccess DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############
install(FILES
    lexiconIdGenerator.h lexiconIdGenerator.tcc
    lexiconIdAccessor.h lexiconIdAccessor.tcc
    lexiconCompoundElement.h
    FsaAccessBuilderRandom16.h
    FsaAccessIOStreamWrapper.h
    PrefixIterator.h
    FsaAccessReader16.h FsaAccessReader16.tcc
    FsaAccessIOHandler.h FsaAccessIOHandler.tcc
    CompoundStringAccess.h CompoundStringAccess.tcc
    FsaAccessHeader.h
    FsaAccessSpare16.h
    AbstractLexiconIdAccessor.h
    FsaAccess16.h FsaAccess16.tcc
    FsaAccessBuilder16.h
    FsaExceptions.h
    AbstractLexiconIdGenerator.h
 DESTINATION include/common/FsaAccess)
