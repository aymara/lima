/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
#include "common/LimaCommon.h"

/* FWI 22/02/2016 déplacé dans le .h
#ifdef WIN32

#ifdef LIMA_COMMON_EXPORTING
#define LIMA_COMMON_EXPORT    __declspec(dllexport)
#else
#define LIMA_COMMON_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_COMMON_EXPORT

#endif
*/

#include <fstream>
#include <qthread.h>

#ifdef ANTINNO_SPECIFIC
namespace Lima
{
#ifdef _DEBUG
StopAnalyze::StopAnalyze(bool v) : _v(v)
{
}
StopAnalyze::StopAnalyze(StopAnalyze const& o) : _v(o._v)
{
}
StopAnalyze::operator bool() const
{
  return _v;
}
StopAnalyze& StopAnalyze::operator=(StopAnalyze const& o)
{
  _v = o._v;
  return *this;
}
bool StopAnalyze::operator==(StopAnalyze const& o)
{
  return _v == o._v;
}
bool StopAnalyze::operator!=(StopAnalyze const& o)
{
  return _v != o._v;
}
#else
//  nothing
#endif

StopAnalyze defaultStopAnalyze(false);

}
#endif

namespace Lima
{
namespace Common
{

LIMA_COMMON_EXPORT void fakeSymbolFoWindowsLinking() {}

}
}
