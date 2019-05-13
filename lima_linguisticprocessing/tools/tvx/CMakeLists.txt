include_directories(BEFORE ${CMAKE_SOURCE_DIR}/. ${CMAKE_SOURCE_DIR}/src )


########### next target ###############

SET(tvx_SRCS
   ReaderTestCase.cpp
   tvx.cpp
)

add_executable(tvx ${tvx_SRCS})

target_link_libraries(tvx
  lima-common-data
  lima-common-datahandler
  lima-common-factory
  lima-common-fsaaccess
  lima-common-mediaprocessors
  lima-common-mediaticdata
  lima-common-misc
  lima-common-processunitframework
  lima-common-tools
  lima-common-tools
  lima-common-xmlconfigurationfiles
  lima-lp-analysisdict
  lima-lp-analysisdumpers
  lima-lp-analysishandlers
  lima-lp-annotationgraph
  lima-lp-applyrecognizer
  lima-lp-automaton
  lima-lp-bagofwords
  lima-lp-client
  lima-lp-flattokenizer
  lima-lp-linguisticanalysisstructure
  lima-lp-linguisticdata
  lima-lp-linguisticprocessing-core
  lima-lp-linguisticprocessors
  lima-lp-linguisticresources
  lima-lp-misc
  lima-lp-morphologicanalysis
  lima-lp-postagger
  lima-lp-propertycode
  lima-lp-specificentities
  lima-lp-syntacticanalysis
  lima-lp-textsegmentation
  lima-lp-tgv
#   lima-xp-multimediadocumenthandler
  lima-xp-client
  lima-xp-core

  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
  ${optionalLibs}
)

install(TARGETS tvx DESTINATION bin)


########### install files ###############
