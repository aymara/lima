//
// Created by tderouet on 29/09/22.
//

#ifndef LIMA_RNNTOKENSANALYZEREXPORT_H
#define LIMA_RNNTOKENSANALYZEREXPORT_H

#include <common/LimaCommon.h>

#ifdef WIN32

#ifdef LIMA_RNN_DEPENDENCY_PARSER_EXPORTING
   #define LIMA_RNNDEPENDENCYPARSER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_RNNDEPENDENCYPARSER_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_RNNDEPENDENCYPARSER_EXPORT

#endif

#endif //LIMA_RNNTOKENSANALYZEREXPORT_H
