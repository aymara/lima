if(NOT WIN32)
  string(ASCII 27 Esc)
  set(C_Norm        "${Esc}[m")
  set(C_Bold        "${Esc}[1m")
  set(C_Red         "${Esc}[31m")
  set(C_Green       "${Esc}[32m")
  set(C_Yellow      "${Esc}[33m")
  set(C_Blue        "${Esc}[34m")
  set(C_Magenta     "${Esc}[35m")
  set(C_Cyan        "${Esc}[36m")
  set(C_White       "${Esc}[37m")
  set(C_BoldRed     "${Esc}[1;31m")
  set(C_BoldGreen   "${Esc}[1;32m")
  set(C_BoldYellow  "${Esc}[1;33m")
  set(C_BoldBlue    "${Esc}[1;34m")
  set(C_BoldMagenta "${Esc}[1;35m")
  set(C_BoldCyan    "${Esc}[1;36m")
  set(C_BoldWhite   "${Esc}[1;37m")
  set(LIMA_PATH_SEPARATOR ":")
else()
  set(LIMA_PATH_SEPARATOR "\;") # WANING: must be protected against cmake semicolon substitution
endif()

set(LIMA_CONF "${CMAKE_BINARY_DIR}/execEnv/config${LIMA_PATH_SEPARATOR}$ENV{LIMA_CONF}")
set(LIMA_RESOURCES "${CMAKE_BINARY_DIR}/execEnv/resources${LIMA_PATH_SEPARATOR}$ENV{LIMA_RESOURCES}")

############
# Dictionary

# Code compilation
macro (CODES _lang)
  set(CODES_FILES)
  foreach(CODE_FILE ${ARGN})
    set(CODES_FILES ${CODES_FILES} ${CODE_FILE})
  endforeach(CODE_FILE ${ARGN})

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt
    COMMAND convertSymbolicCodes  --code=code-${_lang}.xml --output=${CMAKE_CURRENT_BINARY_DIR}/convjys.txt ${CODES_FILES}
    COMMAND parseXMLPropertyFile --code=code-${_lang}.xml --output=${CMAKE_CURRENT_BINARY_DIR}/code-${_lang}.xml.log
    DEPENDS code-${_lang}.xml ${ARGN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "COMMAND convertSymbolicCodes  --code=code-${_lang}.xml --output=${CMAKE_CURRENT_BINARY_DIR}/convjys.txt ${CODES_FILES}"
    VERBATIM
  )

  add_custom_target(
    code${_lang}
    ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/convjys.txt
  )

  install(
    FILES code-${_lang}.xml 
    COMPONENT ${_lang} 
    DESTINATION share/apps/lima/resources/LinguisticProcessings/${_lang})

endmacro (CODES _lang)

# Flexion
macro (FLEXION _lang)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/formes-${_lang}.txt
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/flex.pl def.txt  mots-simples.txt ${CMAKE_CURRENT_BINARY_DIR} formes-${_lang}.txt exclude.txt
    DEPENDS def.txt  mots-simples.txt exclude.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_custom_target(
    flex${_lang}
    ALL
    DEPENDS formes-${_lang}.txt
  )
  add_dependencies(flex${_lang} code${_lang})

endmacro (FLEXION _lang)

# Convert
macro(CONVERT _lang)

  add_custom_command(
    OUTPUT dicotabs.txt
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/pointvirgules2tabs.pl ${CMAKE_CURRENT_BINARY_DIR}/../flex/formes-${_lang}.txt dicotabs.txt
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../flex/formes-${_lang}.txt
    COMMENT "COMMAND ${PROJECT_SOURCE_DIR}/scripts/pointvirgules2tabs formes-${_lang}.txt dicotabs.txt"
    VERBATIM
  )
  add_custom_target(
    dicotabs${_lang}
    ALL
    DEPENDS dicotabs.txt
  )
  add_dependencies(dicotabs${_lang} flex${_lang})

  add_custom_command(
    OUTPUT dicostd.txt
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertstd.pl dicotabs.txt ${CMAKE_CURRENT_SOURCE_DIR}/convstd.txt dicostd.txt
    DEPENDS dicotabs.txt ${CMAKE_CURRENT_SOURCE_DIR}/convstd.txt
    COMMENT "${PROJECT_SOURCE_DIR}/scripts/convertstd dicotabs.txt dicostd.txt"
    VERBATIM
  )
  add_custom_target(
    dicostd${_lang}
    ALL
    DEPENDS dicostd.txt
  )
  add_dependencies(dicostd${_lang} dicotabs${_lang})

  set (ADDED_LIST_FILES_RESULT)
  foreach(ADDED_LIST_FILE ${ADDED_LIST_FILES})
    add_custom_command(
      OUTPUT ${ADDED_LIST_FILE}.add
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/addnormfield.pl ${CMAKE_CURRENT_SOURCE_DIR}/${ADDED_LIST_FILE} > ${ADDED_LIST_FILE}.add
      DEPENDS dicostd.txt ${ADDED_LIST_FILE}
      COMMENT perl "${PROJECT_SOURCE_DIR}/scripts/addnormfield.pl ${CMAKE_CURRENT_SOURCE_DIR}/${ADDED_LIST_FILE} > ${ADDED_LIST_FILE}.add"
    )
    set (ADDED_LIST_FILES_RESULT ${ADDED_LIST_FILES_RESULT} ${ADDED_LIST_FILE}.add)
  endforeach(ADDED_LIST_FILE ${ADDED_LIST_FILES})
  add_custom_target(
    dicoadd${_lang}
    ALL
    DEPENDS ${ADDED_LIST_FILES_RESULT}
  )
  add_dependencies(dicoadd${_lang} dicostd${_lang})

  set(ENV{LC_ALL} "C")

  if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    add_custom_command(
      OUTPUT dicocompletstd.txt
      COMMAND LC_ALL="C" sort -u dicostd.txt ${ADDED_LIST_FILES_RESULT} > dicocompletstd.txt
      DEPENDS dicostd.txt ${ADDED_LIST_FILES_RESULT}
      COMMENT "sort -u dicostd.txt ${ADDED_LIST_FILES_RESULT} > dicocompletstd.txt"
      VERBATIM
    )
  else (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    add_custom_command(
      OUTPUT dicocompletstd.txt
      COMMAND sort -u dicostd.txt ${ADDED_LIST_FILES_RESULT} > dicocompletstd.txt
      DEPENDS dicostd.txt ${ADDED_LIST_FILES_RESULT}
      COMMENT "sort -u dicostd.txt ${ADDED_LIST_FILES_RESULT} > dicocompletstd.txt"
      VERBATIM
    )
  endif (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    add_custom_target(
      dicocomplet${_lang}
      ALL
      DEPENDS dicocompletstd.txt
    )
    add_dependencies(dicocomplet${_lang} dicoadd${_lang})

  if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    add_custom_command(
      OUTPUT dico.xml
      COMMAND echo "<dictionary>" > dico.xml.tmp
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl dicocompletstd.txt dico.xml.tmp
      COMMAND bash -c "if [ -n \"${ARGN}\" ]; then cat ${ARGN} >> dico.xml.tmp; fi"
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/addnormfield.pl ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt > dicoponctu.norm.txt
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl -desacc=no dicoponctu.norm.txt dico.xml.tmp
      COMMAND echo "</dictionary>" >> dico.xml.tmp
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-${_lang}.txt ../code/convjys.txt default-${_lang}.dat
      COMMAND mv dico.xml.tmp dico.xml
      DEPENDS dicocompletstd.txt ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt ${CMAKE_CURRENT_SOURCE_DIR}/default-${_lang}.txt
      COMMENT "CONVERT ${_lang} produce XML dico"
      VERBATIM
    )
  else (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    # WARNING: VERBATIM option add unintentional double quotes symbols in XML file
    add_custom_command(
      OUTPUT dico.xml
      COMMAND echo ^<dictionary^> > dico.xml.tmp
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl dicocompletstd.txt dico.xml.tmp
      COMMAND bash -c "if [ -n \"${ARGN}\" ]; then cat ${ARGN} >> dico.xml.tmp; fi"
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/addnormfield.pl ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt > dicoponctu.norm.txt
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/xmlforms.pl -desacc=no dicoponctu.norm.txt dico.xml.tmp
      COMMAND echo ^</dictionary^> >> dico.xml.tmp
      COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/cmakeconvertdefautjys.pl ${CMAKE_CURRENT_SOURCE_DIR}/default-${_lang}.txt ../code/convjys.txt default-${_lang}.dat
      COMMAND mv dico.xml.tmp dico.xml
      DEPENDS dicocompletstd.txt ${CMAKE_CURRENT_SOURCE_DIR}/dicoponctu.txt ${CMAKE_CURRENT_SOURCE_DIR}/default-${_lang}.txt
      COMMENT "produce XML dico"
    )
  endif (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))

  add_custom_target(
    dicoxml${_lang}
    ALL
    DEPENDS dico.xml
  )
  add_dependencies(dicoxml${_lang} dicocomplet${_lang})

  add_custom_target(
    convert${_lang}
    ALL
  )
  add_dependencies(convert${_lang} dicoxml${_lang} ${ARGN} )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-${_lang}.dat COMPONENT ${_lang} DESTINATION share/apps/lima/resources/LinguisticProcessings/${_lang})

endmacro(CONVERT _lang)

# Compile XML dictionary
macro(COMPILEXMLDIC _lang _dico _subdir)
#   string(REPLACE "/" "" _dicostr ${_dico})
  get_filename_component(_dicostr ${_dico} NAME_WE)
#  set (CHARCHART "${CMAKE_INSTALL_PREFIX}/share/apps/lima/resources/LinguisticProcessings/${_lang}/tokenizerAutomaton-${_lang}.chars.tok")
  set (CHARCHART "${PROJECT_SOURCE_DIR}/scratch/LinguisticProcessings/${_lang}/tokenizerAutomaton-${_lang}.chars.tok")
  get_filename_component(DICOFILENAME ${_dico} NAME_WE)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  add_custom_command(
    OUTPUT ${DICOFILENAME}Dat-${_lang}.dat
    COMMAND compile-dictionary --charChart=${CHARCHART} --extractKeyList=keys ${_dico}
    COMMAND LC_ALL="C" sort -T . -u keys > keys_${_dicostr}.sorted
    COMMAND testDict16 --charSize=2 --listOfWords=keys_${_dicostr}.sorted --output=${DICOFILENAME}Key-${_lang}.dat > output_${_dicostr}
#    COMMAND testDict16 --charSize=2 --input=${DICOFILENAME}Key-${_lang}.dat.tmp --spare --output=${DICOFILENAME}Key-${_lang}.dat >> output_${_dicostr}
    COMMAND compile-dictionary --charChart=${CHARCHART} --fsaKey=${DICOFILENAME}Key-${_lang}.dat --propertyFile=${CMAKE_CURRENT_SOURCE_DIR}/../code/code-${_lang}.xml --symbolicCodes=${CMAKE_CURRENT_SOURCE_DIR}/../code/symbolicCode-${_lang}.xml --output=${DICOFILENAME}Dat-${_lang}.dat ${_dico}
    DEPENDS ${_dico} ${CMAKE_CURRENT_SOURCE_DIR}/../code/code-${_lang}.xml ${CMAKE_CURRENT_SOURCE_DIR}/../code/symbolicCode-${_lang}.xml ${CHARCHART} ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml
    COMMENT "compile-dictionary"
    VERBATIM
  )
else ()
  add_custom_command(
    OUTPUT ${DICOFILENAME}Dat-${_lang}.dat
    COMMAND compile-dictionary --charChart=${CHARCHART} --extractKeyList=keys ${_dico}
    COMMAND sort -T . -u keys > keys_${_dicostr}.sorted
    COMMAND testDict16 --charSize=2 --listOfWords=keys_${_dicostr}.sorted --output=${DICOFILENAME}Key-${_lang}.dat > output_${_dicostr}
#    COMMAND testDict16 --charSize=2 --input=${DICOFILENAME}Key-${_lang}.dat.tmp --spare --output=${DICOFILENAME}Key-${_lang}.dat >> output_${_dicostr}
    COMMAND compile-dictionary --charChart=${CHARCHART} --fsaKey=${DICOFILENAME}Key-${_lang}.dat --propertyFile=${CMAKE_CURRENT_SOURCE_DIR}/../code/code-${_lang}.xml --symbolicCodes=${CMAKE_CURRENT_SOURCE_DIR}/../code/symbolicCode-${_lang}.xml --output=${DICOFILENAME}Dat-${_lang}.dat ${_dico}
    DEPENDS ${_dico} ${CMAKE_CURRENT_SOURCE_DIR}/../code/code-${_lang}.xml ${CMAKE_CURRENT_SOURCE_DIR}/../code/symbolicCode-${_lang}.xml ${CHARCHART} ${CMAKE_CURRENT_BINARY_DIR}/../convert/dico.xml
    COMMENT "compile-dictionary"
    VERBATIM
  )
endif ()

  add_custom_target(
    compilexmldic${_lang}${_dicostr}
    ALL
    DEPENDS ${DICOFILENAME}Dat-${_lang}.dat
  )
  add_dependencies(compilexmldic${_lang}${_dicostr} convert${_lang})

  install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${DICOFILENAME}Key-${_lang}.dat
      ${CMAKE_CURRENT_BINARY_DIR}/${DICOFILENAME}Dat-${_lang}.dat
    COMPONENT ${_lang} DESTINATION share/apps/lima/resources/LinguisticProcessings/${_lang}/${_subdir})

endmacro(COMPILEXMLDIC _lang)

###############
# Generate disambiguation matrices

macro(DISAMBMATRICES _lang _succession_categs _codesymbol _priorscript _tableconvert)

  add_custom_command(
    OUTPUT trigramMatrix-${_lang}.dat
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_extract.pl ${_succession_categs}
    COMMAND cat ${_succession_categs} | sort | uniq -c | gawk -F" " "{print $2\"\t\"$1}" > unigramMatrix-${_lang}.dat
    COMMAND perl ${_priorscript} corpus_${_lang}_merge.txt priorUnigramMatrix-${_lang}.dat ${_codesymbol} ${_tableconvert}
    COMMAND mv bigramsend.txt bigramMatrix-${_lang}.dat
    COMMAND perl ${PROJECT_SOURCE_DIR}/scripts/disamb_matrices_normalize.pl trigramsend.txt trigramMatrix-${_lang}.dat

    DEPENDS ${_codesymbol} ${_succession_categs}
    COMMENT "compile ${_lang} trigram matrix"
    VERBATIM
  )

  add_custom_target(
    trigrammatrix-${_lang}
    ALL
    DEPENDS trigramMatrix-${_lang}.dat
  )

  install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/unigramMatrix-${_lang}.dat
        ${CMAKE_CURRENT_BINARY_DIR}/priorUnigramMatrix-${_lang}.dat
        ${CMAKE_CURRENT_BINARY_DIR}/bigramMatrix-${_lang}.dat
        ${CMAKE_CURRENT_BINARY_DIR}/trigramMatrix-${_lang}.dat
      COMPONENT ${_lang} DESTINATION share/apps/lima/resources/Disambiguation)

endmacro(DISAMBMATRICES _lang)

###############
# Compile rules

macro (COMPILE_RULES _lang)
  set (COMPILE_RULES_DEBUG_MODE)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set (COMPILE_RULES_DEBUG_MODE "--debug")
  endif ()
  foreach(_current ${ARGN})
    add_custom_command(
      OUTPUT ${_current}.bin
      COMMAND compile-rules --resourcesDir=${CMAKE_BINARY_DIR}/execEnv/resources --configDir=${LIMA_CONF} ${COMPILE_RULES_DEBUG_MODE} --language=${_lang} ${_current} -o${CMAKE_CURRENT_BINARY_DIR}/${_current}.bin
      DEPENDS ${_current}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endforeach()
endmacro (COMPILE_RULES)

###############
# Idiomatic entities rules

# Idiomatic entities Exec Environment

macro (IDIOMATICENTITIES _lang)
  set (COMPILE_RULES_DEBUG_MODE)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set (COMPILE_RULES_DEBUG_MODE "--debug")
  endif ()
  add_custom_command(
    OUTPUT idiomaticExpressions-${_lang}.bin
    COMMAND compile-rules --resourcesDir=${CMAKE_BINARY_DIR}/execEnv/resources --configDir=${LIMA_CONF} --language=${_lang} ${COMPILE_RULES_DEBUG_MODE} -oidiomaticExpressions-${_lang}.bin idiomaticExpressions-${_lang}.rules
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/idiomaticExpressions-${_lang}.rules
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

  add_custom_target(
    rules-idiom-${_lang}
    ALL
    DEPENDS idiomaticExpressions-${_lang}.bin )
    
  add_dependencies(rules-idiom-${_lang} 
    rules-${_lang}-execEnv)
  # add the link between the current target and its execution environment dependencies

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/idiomaticExpressions-${_lang}.bin 
    COMPONENT ${_lang} 
    DESTINATION share/apps/lima/resources/LinguisticProcessings/${_lang})
    
endmacro (IDIOMATICENTITIES _lang)

###############
#
# LIMA_GENERIC_CONFIGENV
#
#
####################
# Specific Entities Exec Environment

macro (SPECIFICENTITIES_GENERIC_CONFIGENV)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/SRLIntegration/FrameNet-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SRLIntegration/FrameNet-modex.xml
    COMMENT "create config env for specific entities rules (FrameNet-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/SRLIntegration/VerbNet-modex.xml
      ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SRLIntegration/VerbNet-modex.xml
    COMMENT "create config env for specific entities rules (VerbNet-modex.xml)"
    VERBATIM
  )


  # Permet de créer un lien build/execEnv/config/<group>-modex.xml
  # vers ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/<group>-modex.xml
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/SpecificEntities-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/SpecificEntities-modex.xml
      ${CMAKE_BINARY_DIR}/execEnv/config/SpecificEntities-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/SpecificEntities-modex.xml
    COMMENT "create config env for specific entities rules (SpecificEntities-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/AuthorPosition-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/AuthorPosition-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/AuthorPosition-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/AuthorPosition-modex.xml
    COMMENT "create config env for specific entities rules (AuthorPosition-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/DateTime-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/DateTime-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/DateTime-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/DateTime-modex.xml
    COMMENT "create config env for specific entities rules (DateTime-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Event-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Event-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Event-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Event-modex.xml
    COMMENT "create config env for specific entities rules (Event-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Location-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Location-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Location-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Location-modex.xml
    COMMENT "create config env for specific entities rules (Location-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Numex-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Numex-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Numex-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Numex-modex.xml
    COMMENT "create config env for specific entities rules (Numex-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Organization-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Organization-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Organization-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Organization-modex.xml
    COMMENT "create config env for specific entities rules (Organization-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Person-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Person-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Person-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Person-modex.xml
    COMMENT "create config env for specific entities rules (Person-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Product-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Product-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Product-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Product-modex.xml
    COMMENT "create config env for specific entities rules (Product-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/Miscellaneous-modex.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Miscellaneous-modex.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/Miscellaneous-modex.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/conf/Miscellaneous-modex.xml
    COMMENT "create config env for specific entities rules (Miscellaneous-modex.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-analysis.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-analysis.xml
    COMMENT "create config env for specific entities rules (lima-analysis.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common.xml
    COMMENT "create config env for specific entities rules (lima-common.xml)"
    VERBATIM
  )

  # defini l'ensemble des dépendances (ce qui doit exister dans la partie configuration de l'environnement
  # d'execution de la cible rules-${_group}-${_lang}-${_subtarget}
  add_custom_target(
    rules-configEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-analysis.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/SpecificEntities-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/AuthorPosition-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/DateTime-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Event-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Location-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Numex-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Organization-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Person-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Product-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/Miscellaneous-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/FrameNet-modex.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/VerbNet-modex.xml
  )
endmacro (SPECIFICENTITIES_GENERIC_CONFIGENV)

macro (SPECIFICENTITIESCONFIGENV _lang)
  # TODO: some tools like compile-rule need access to a set of very
  # complete configuration files (lima-lp-<lang>.xml, <group>-modex.xml...)
  # Many macro could be simplified if the dependencies of these tools
  # could be reduced to really usefull ones.

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${_lang}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common-${_lang}.xml
      ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${_lang}.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-common-${_lang}.xml
    COMMENT "create config env for specific entities rules (lima-common-${_lang}.xml)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${_lang}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/config
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-lp-${_lang}.xml
     ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${_lang}.xml
    DEPENDS
      ${CMAKE_INSTALL_PREFIX}/share/config/lima/lima-lp-${_lang}.xml
    COMMENT "create config env for specific entities rules (lima-lp-${_lang}.xml)"
    VERBATIM
  )

  # definit l'ensemble des dépendances (ce qui doit exister dans la partie 
  # configuration de l'environnement d'execution de la cible 
  # rules-${_lang}
  add_custom_target(
    rules-${_lang}-configEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-common-${_lang}.xml
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/config/lima-lp-${_lang}.xml
  )

  add_dependencies(
    rules-${_lang}-configEnv 
    rules-${_lang}-execEnv
    rules-configEnv)

endmacro (SPECIFICENTITIESCONFIGENV _subtarget _lang _group)

macro (LIMA_GENERIC_CONFIGENV _lang)
  MESSAGE( "${C_BoldYellow}LIMA_GENERIC_CONFIGENV(${_lang})${C_Norm}" )

  # Permet de créer un lien build/execEnv/resources/SpecificEntities/tz-db-${_lang}.dat
  # vers ${CMAKE_SOURCE_DIR}/SpecificEntities/${_lang}/resources/tz-db-${_lang}.dat
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/tz-db-${_lang}.dat
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/${_lang}/resources/tz-db-${_lang}.dat
     ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/tz-db-${_lang}.dat
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/${_lang}/resources/tz-db-${_lang}.dat
    COMMENT "create exec env for ${_lang} specific entities rules (tz-db-${_lang}.dat)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/monthsdays-${_lang}.dat
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities
    COMMAND ${CMAKE_COMMAND} -E copy
     ${CMAKE_SOURCE_DIR}/SpecificEntities/${_lang}/resources/monthsdays-${_lang}.dat
     ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/monthsdays-${_lang}.dat
    DEPENDS
      ${CMAKE_SOURCE_DIR}/SpecificEntities/${_lang}/resources/monthsdays-${_lang}.dat
    COMMENT "create exec env for ${_lang} specific entities rules (monthsdays-${_lang}.dat)"
    VERBATIM
  )
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${_lang}/code-${_lang}.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${_lang}
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_SOURCE_DIR}/analysisDictionary/${_lang}/code/code-${_lang}.xml
      ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${_lang}/code-${_lang}.xml
    DEPENDS
      ${CMAKE_SOURCE_DIR}/analysisDictionary/${_lang}/code/code-${_lang}.xml
    COMMENT "create exec env for ${_lang} specific entities rules (code)"
    VERBATIM
  )

  add_custom_target(
    rules-${_lang}-execEnv
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/tz-db-${_lang}.dat
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/resources/SpecificEntities/monthsdays-${_lang}.dat
    DEPENDS ${CMAKE_BINARY_DIR}/execEnv/resources/LinguisticProcessings/${_lang}/code-${_lang}.xml
  )
endmacro (LIMA_GENERIC_CONFIGENV _lang)

####################
# Specific Entities
macro (SPECIFICENTITIES _subtarget _lang _group)
  set (COMPILE_RULES_DEBUG_MODE)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set (COMPILE_RULES_DEBUG_MODE "--debug")
  endif ()
  set (BINFILENAMES)
  foreach(_current ${ARGN})
    get_filename_component(BINFILENAME ${_current} NAME_WE)
    set(BINFILENAME ${CMAKE_CURRENT_BINARY_DIR}/${BINFILENAME}.bin)
    set (BINFILENAMES ${BINFILENAMES} ${BINFILENAME})
    add_custom_command(
      OUTPUT ${BINFILENAME}
	  COMMAND compile-rules --resourcesDir=${LIMA_RESOURCES} --configDir=${LIMA_CONF} --language=${_lang} ${COMPILE_RULES_DEBUG_MODE} -o${BINFILENAME} ${_current} --modex=${_group}-modex.xml
      DEPENDS ${_current} ${DEPENDENCIES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM
    )
  endforeach(_current ${ARGN})

  add_custom_target(
    rules-${_group}-${_lang}-${_subtarget}
    ALL
    DEPENDS ${BINFILENAMES}
  )

  install(FILES ${BINFILENAMES} COMPONENT ${_lang} DESTINATION share/apps/lima/resources/SpecificEntities)

  # add the link between the current target and its execution environment dependencies
  add_dependencies(rules-${_group}-${_lang}-${_subtarget} specificentitiesconfigenv-${_lang}-all rules-${_lang}-${_group}-configEnv-${_subtarget} rules-${_lang}-execEnv)

endmacro (SPECIFICENTITIES _lang _group)


####################
# Syntactic analysis

macro (COMPILE_SA_RULES_WRAPPER _lang)
  set(${_lang}_BIN_RULES_FILES)
  foreach(RULES_FILE ${ARGN})
    set (${_lang}_BIN_RULES_FILES ${RULES_FILE}.bin ${${_lang}_BIN_RULES_FILES})
  endforeach(RULES_FILE ${ARGN})

  COMPILE_RULES(${_lang} ${ARGN})

  add_custom_target(
    syntanalrules-${_lang}
    ALL
    DEPENDS ${${_lang}_BIN_RULES_FILES} ${${_lang}_SA_DEPENDS_FILES} rules-${_lang}-execEnv rules-configEnv
  )

  foreach (file ${${_lang}_BIN_RULES_FILES})
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${file}
      COMPONENT ${_lang} DESTINATION share/apps/lima/resources/SyntacticAnalysis)
  endforeach (file ${${_lang}_BIN_RULES_FILES})
endmacro (COMPILE_SA_RULES_WRAPPER  _lang)

macro (ADD_SA_RULES_DEPENDS _lang)
  set(${_lang}_SA_DEPENDS_FILES)
  foreach(SA_DEPS_FILE ${ARGN})
    set (${_lang}_SA_DEPENDS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SA_DEPS_FILE} ${${_lang}_SA_DEPENDS_FILES})
  endforeach(SA_DEPS_FILE ${ARGN})
message("Execute ADD_SA_RULES_DEPENDS on ${${_lang}_SA_DEPENDS_FILES}")

  add_custom_command(
    OUTPUT syntanaldepends
    COMMAND touch syntanaldepends
    DEPENDS ${${_lang}_SA_DEPENDS_FILES}
    VERBATIM
  )

  add_custom_target(
    syntanaldepends-${_lang}
    ALL
    DEPENDS syntanaldepends
  )

  install(FILES ${${_lang}_SA_DEPENDS_FILES}
    COMPONENT ${_lang} DESTINATION share/apps/lima/resources/SyntacticAnalysis)

endmacro (ADD_SA_RULES_DEPENDS  _lang)

############
#
