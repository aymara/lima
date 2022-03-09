project(LangDetector)

include_directories(BEFORE ../../../../../extern)

SET(langdetectorlib_LIB_SRCS langDetector.cpp)
link_directories(/usr/local/lib)

add_library(langdetector SHARED ${langdetectorlib_LIB_SRCS})

add_executable(LangDetector main.cpp langDetector.cpp langDetector.h)

add_executable(test test.cpp langDetector.cpp langDetector.h)

target_link_libraries(langdetector fasttext-lima)
target_link_libraries(LangDetector fasttext-lima)
target_link_libraries(test fasttext-lima)
