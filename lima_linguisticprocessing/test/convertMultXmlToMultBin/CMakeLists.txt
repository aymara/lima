########### next target ###############

SET(convertMultXmlToMultBin_SRCS
   convertMultXmlToMultBin.cpp
   MultXmlHandler.cpp
   MultXmlReader.cpp
)

add_executable(convertMultXmlToMultBin ${convertMultXmlToMultBin_SRCS})

target_link_libraries(convertMultXmlToMultBin 
lima-common-data 
lima-common-factory 
lima-common-fsaaccess 
lima-common-mediaticdata 
lima-common-misc
lima-common-tools 
lima-common-tools 
lima-common-xmlconfigurationfiles 

lima-lp-bagofwords 
lima-lp-client
lima-lp-linguisticdata 
lima-lp-misc 
lima-lp-propertycode 

lima-xp-multimediadocumenthandler

${Boost_LIBRARIES}  
${QT_LIBRARIES} 
${optionalLibs})

install(TARGETS convertMultXmlToMultBin DESTINATION bin)


