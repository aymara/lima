include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/extern/fastText/src)

########### next target ###############

SET(lima-lp-langdetector_LIB_SRCS
        LangDetector.cpp
        LangDetectorCore.cpp
        )

DECLARE_LIMA_PLUGIN(lima-lp-langdetector)

target_compile_definitions(lima-lp-langdetector PRIVATE "-DLIMA_LANGDETECTOR_EXPORTING")

target_link_libraries(lima-lp-langdetector
        lima-common-processunitframework
        lima-lp-linguisticdata
        lima-lp-linguisticresources
        lima-lp-annotationgraph
        lima-lp-linguisticanalysisstructure
        fasttext-static
        )

set_target_properties(lima-lp-langdetector PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-langdetector DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT runtime)
install(FILES
        model/lid.176.ftz
        DESTINATION share/apps/lima/resources/LinguisticProcessings/LangDetector/
        COMPONENT runtime
        )
########### next target ###############
add_executable(langDetector main.cpp)
target_link_libraries(langDetector
        lima-lp-langdetector
        lima-lp-linguisticresources
        lima-common-misc
        fasttext-static
        ${QT_LIBRARIES}
        )

install(TARGETS langDetector DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT runtime)
