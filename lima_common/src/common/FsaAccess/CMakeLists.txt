# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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

ADD_DLL_TO_PATH(lima-common-fsaaccess)

target_link_libraries(lima-common-fsaaccess 
  lima-common-misc
)

set_target_properties(lima-common-fsaaccess PROPERTIES VERSION ${LIMA_COMMON_LIB_VERSION} SOVERSION ${LIMA_COMMON_LIB_SOVERSION})
install(TARGETS lima-common-fsaaccess DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT runtime)


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
  DESTINATION include/common/FsaAccess
  COMPONENT devel
)
