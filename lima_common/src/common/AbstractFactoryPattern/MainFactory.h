/*
    Copyright 2002-2019 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORY_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

#ifdef WIN32
#include "common/AbstractFactoryPattern/MainFactoriesMap.h"
#endif

#include <map>
#include <deque>
#include <string>
#include <typeinfo>

/**
 * @Note (Gael de Chalendar, 06/06/2012) There is two versions of the MainFactory class: one for
 * MS Windows and another one for Linux. The Windows one uses a trick to ensure having only one
 * instance of the static factory object in all DLLs. This version should also work the same under
 * Linux but for a reason I don't understand, it don't... Thus the two versions.
 */


#ifdef WIN32

#include "MainFactoryWin32.h"

#else // Linux

#include "MainFactoryLinux.h"

#endif // WIN32 test

#endif
