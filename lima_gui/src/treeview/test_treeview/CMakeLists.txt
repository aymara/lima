

########### next target ###############
SET (test_treeview_MOC_HEADERS
  treemodel.h
)

qt5_wrap_cpp (test_treeview_MOC_OUTFILES ${test_treeview_MOC_HEADERS})

set (test_treeview_SRCS
  main.cpp
  treeitem.cpp
  treemodel.cpp
)
qt5_add_resources(test_treeview_SRCS qml.qrc)

add_executable(test_treeview 
  ${test_treeview_SRCS}
  ${test_treeview_MOC_OUTFILES}
)
target_link_libraries(test_treeview 
  ${Qt5_LIBRARIES}
)

install(TARGETS test_treeview DESTINATION bin)

########### install files ###############
