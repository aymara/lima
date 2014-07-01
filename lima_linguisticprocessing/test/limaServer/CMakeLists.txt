INCLUDE(${QT_USE_FILE})

########### next target ###############



include_directories( ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

SET(limaserver_SRCS
   LimaServer.cpp
   main.cpp
   analysisthread.cpp
)

set (limaserver_MOC_HEADERS 
    LimaServer.h
    analysisthread.h )


qt4_wrap_cpp (limaserver_MOC_OUTFILES ${limaserver_MOC_HEADERS})

# qt4_add_resources(limaserver amoseserver.qrc)

# add_executable(limaserver ${amoseserver_SRCS} ${amoseserver_QRCS} ${amoseserver_MOC_OUTFILES})
add_executable(limaserver ${limaserver_SRCS} ${limaserver_MOC_OUTFILES})

target_link_libraries(limaserver
    lima-common
    lima-linguisticprocessing
    ${optionalLibs}
    ${Boost_LIBRARIES}
    ${QT_LIBRARIES}
# lima-lp-analysisdumpers 
# lima-lp-analysishandlers 
# lima-lp-applyrecognizer 
# lima-lp-automaton
# lima-lp-bagofwords 
# lima-lp-annotationgraph 
# #lima-lp-compounds
# lima-lp-regexmatcher 
# lima-lp-linguisticdata 
# lima-lp-linguisticprocessors 
# lima-lp-linguisticresources 
# lima-lp-client
# lima-lp-linguisticprocessing-core 
# lima-lp-morphologicanalysis
# lima-lp-analysisdict 
# lima-lp-linguisticanalysisstructure 
# lima-lp-misc
# lima-lp-postagger 
# lima-lp-propertycode 
# lima-lp-specificentities
# lima-lp-syntacticanalysis 
# lima-lp-flattokenizer
# lima-lp-textsegmentation

# lima-common-data 
# lima-common-misc 
# lima-common-datahandler 
# lima-common-fsaaccess
# lima-common-qslog 
# lima-common-mediaprocessors 
# lima-common-mediaticdata
# lima-common-processunitframework 
# lima-common-xmlconfigurationfiles
# lima-common-time

${QHTTPSERVER_NAMES}
${qjson_LIBRARIES} ${QJSON_LIBRARIES}
${OpenCV_LIBS} ${QT_LIBRARIES} ${JAVA_JVM_LIBRARY} ${Boost_LIBRARIES})

install(TARGETS limaserver DESTINATION bin)

