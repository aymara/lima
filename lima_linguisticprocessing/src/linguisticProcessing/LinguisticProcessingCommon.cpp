// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "common/LimaCommon.h"

#ifdef WIN32

#ifdef LIMA_LP_EXPORTING
#define LIMA_LP_EXPORT    __declspec(dllexport)
#else
#define LIMA_LP_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_LP_EXPORT

#endif


namespace Lima
{
  namespace LinguisticProcessing
  {
    
    LIMA_LP_EXPORT void fakeSymbolForWindowsLinking() {}
    
  }
}
