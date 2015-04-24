include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )
#include_directories($ENV{AMOSE_EXTERNALS}/crfsuite-0.12/include)

#link_directories(/usr/local/lib)

add_definitions(-DLIMA_SPECIFICENTITIESCRF_EXPORTING)
#add_definitions(-std=c99)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#add_definitions(-std=c++0x)

########### main target ###############

SET(lima-lp-specificentitiesCrf_LIB_SRCS
   SpecificEntitiesCRFLearn.cpp
   TaggerWap.cpp
   TrainerWap.cpp
   GraphToWap.cpp
   AbstractTrainerFactory.cpp
   AbstractTaggerFactory.cpp
   SpecificEntitiesCRFTag.cpp
   AddSE.cpp
   bcd.c
   decoder.c
   gradient.c
   lbfgs.c
   model.c
   pattern.c
   progress.c
   quark.c
   reader.c
   rprop.c
   sgdl1.c
   thread.c
   tools.c
   vmath.c
   wapiti.c
)

DECLARE_LIMA_PLUGIN(lima-lp-specificentitiesCrf)

target_link_libraries(lima-lp-specificentitiesCrf 
  lima-common-factory 
  lima-common-misc 
  lima-common-data 
  lima-common-fsaaccess 
  lima-common-mediaticdata 
  lima-common-time 
  lima-common-factory 
  lima-common-xmlconfigurationfiles 
  lima-common-processunitframework 
  lima-common-mediaprocessors 
  lima-lp-linguisticprocessors 
  lima-lp-linguisticresources 
  lima-lp-annotationgraph 
  lima-lp-automaton 
  lima-lp-linguisticanalysisstructure 
  lima-lp-syntacticanalysis 
  #${QT_LIBRARIES}
  #lima-common-qslog ${Boost_LIBRARIES}
)

set_target_properties(lima-lp-specificentitiesCrf PROPERTIES VERSION ${AMOSE_LP_LIB_VERSION} SOVERSION ${AMOSE_LP_LIB_SOVERSION})

install(TARGETS lima-lp-specificentitiesCrf DESTINATION lib)

########### install files ###############
install(FILES  
  SpecificEntitiesCRFLearn.h
  SpecificEntitiesCRFTag.h
  AbstractTrainer.h
  AbstractTagger.h
  AbstractTrainerFactory.h
  AbstractTaggerFactory.h
  TaggerWap.h
  TrainerWap.h
  GraphToWap.h
  AddSE.h
  SpecificEntitiesCRFExport.h
  decoder.h
  gradient.h
  model.h
  pattern.h
  progress.h
  quark.h
  reader.h
  sequence.h
  thread.h
  tools.h
  trainers.h
  vmath.h
  wapiti.h
  DESTINATION include/linguisticProcessing/core/SpecificEntitiesCRF)