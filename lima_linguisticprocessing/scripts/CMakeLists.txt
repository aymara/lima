# ########### install files ###############
file(GLOB bashs *.sh)
install(PROGRAMS
    ${bashs}
  DESTINATION bin)

file(GLOB pys *.py)
install(PROGRAMS
    ${pys}
  DESTINATION bin)

