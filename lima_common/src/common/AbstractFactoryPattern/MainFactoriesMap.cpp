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
#include "MainFactoriesMap.h"

#include <iostream>

// static const int init =  initMainFactoriesMap();
// 
// LIMA_FACTORY_EXPORT int initMainFactoriesMap()
// {
//   MainFactoriesMap::mainFactoriesMap();
//   return 0;
// }

std::map<std::string,void*>* MainFactoriesMap::s_mainFactoriesMap = 0;
std::map<std::string,void*> MainFactoriesMap::s_mainFactoriesMap_one_by_dll;

MainFactoriesMap::MainFactoriesMap()
{
  std::cerr << "MainFactoriesMap constructor" << std::endl;
}

std::map<std::string,void*>& MainFactoriesMap::mainFactoriesMap()
{
  //std::cerr << "MainFactoriesMap accessor" << std::endl;
  if (s_mainFactoriesMap == 0)
  {
    s_mainFactoriesMap = &s_mainFactoriesMap_one_by_dll;
  }
  return *s_mainFactoriesMap;
}
