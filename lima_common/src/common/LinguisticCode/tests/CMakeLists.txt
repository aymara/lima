################### tests ########################

########### next test ###############

SET(LinguisticCodeTest0_SRCS
  LinguisticCodeTest0.cpp
)

add_executable(LinguisticCodeTest0
  ${LinguisticCodeTest0_SRCS}
  ${LinguisticCodeTest0_MOC_OUTFILES})

target_link_libraries(LinguisticCodeTest0
  lima-common
  Boost::unit_test_framework
)

add_test(NAME LinguisticCodeTest0 COMMAND LinguisticCodeTest0)
set_tests_properties(LinguisticCodeTest0 PROPERTIES ENVIRONMENT LIMA_CONF=${CMAKE_SOURCE_DIR}/conf)

