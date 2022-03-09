project(LangDetector)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

include_directories(BEFORE ../../../../../extern)

SET(langdetectorlib_LIB_SRCS langDetector.cpp)
link_directories(/usr/local/lib /usr/lib)

add_library(langdetector SHARED ${langdetectorlib_LIB_SRCS})

add_executable(LangDetector main.cpp langDetector.cpp langDetector.h)
target_link_libraries(LangDetector langdetector)

add_executable(test test.cpp langDetector.cpp langDetector.h)
target_link_libraries(test langdetector)

target_link_libraries(langdetector fasttext-lima)
target_link_libraries(LangDetector fasttext-lima)
target_link_libraries(test fasttext-lima)
