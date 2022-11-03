# ########### install files ###############
file(GLOB bashs *.sh)
install(PROGRAMS
    ${bashs}
  DESTINATION bin)

