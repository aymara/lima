################### tests ########################

########### next test ###############

SET(AnnotationGraphTest0_SRCS
   AnnotationGraphTest0.cpp
)

add_executable(AnnotationGraphTest0 ${AnnotationGraphTest0_SRCS})

target_link_libraries(AnnotationGraphTest0
  lima-common-data
  lima-common-qslog
  lima-lp-annotationgraph
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
)

add_test(AnnotationGraphTest0 AnnotationGraphTest0)

