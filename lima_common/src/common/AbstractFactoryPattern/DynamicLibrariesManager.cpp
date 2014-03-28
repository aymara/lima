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
/************************************************************************
 *
 * @file       DynamicLibrariesManager.cpp
 * @author      (romaric.besancon@cea.fr)
 * @date       Wed Feb  6 2008
 * copyright   Copyright (C) 2008-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "DynamicLibrariesManager.h"

#include <iostream>

using namespace std;

namespace Lima {
namespace Common {

DynamicLibrariesManager::DynamicLibrariesManager() 
{
}

DynamicLibrariesManager::~DynamicLibrariesManager() 
{
  for (std::map<std::string,QLibrary*>::iterator
         it=m_handles.begin(),it_end=m_handles.end(); it!=it_end; it++) 
  {
    delete (*it).second;
  }
}

bool DynamicLibrariesManager::
isLoaded(const std::string& libName)
{
  std::map<std::string,QLibrary*>::const_iterator
    it=m_handles.find(libName);
  return (it!=m_handles.end());
}

bool DynamicLibrariesManager::
loadLibrary(const std::string& libName)
{
  std::map<std::string,QLibrary*>::const_iterator
    it=m_handles.find(libName);
  if (it!=m_handles.end()) {
// #ifdef DEBUG_CD
//     std::cerr << "DEBUG_CD: trying to reload dynamic library " << libName.c_str() << std::endl;
// #endif
  }

  QLibrary* libhandle = 0;
  // try supplementary search path
  for (std::vector<std::string>::const_iterator it = m_supplementarySearchPath.begin(); it != m_supplementarySearchPath.end(); it++)
  {
#ifdef DEBUG_FACTORIES
    std::cerr << "Trying supplementary " << ((*it)+"/"+libName).c_str() << std::endl;
#endif
    libhandle = new QLibrary( ((*it)+"/"+libName).c_str() );
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_handles.insert(std::make_pair(libName,libhandle));
      return true;
    }
    else
    {
//      if ( QLibrary::isLibrary(((*it)+"/"+libName).c_str()) )
        std::cerr << "Failed to open lib " << libhandle->errorString().toUtf8().data() << std::endl;
      delete libhandle;
      libhandle = 0;
    }
  }
  // now try system default search path
  if (libhandle == 0)
  {
#ifdef DEBUG_FACTORIES
    std::cerr << "Trying " << libName.c_str() << std::endl;
#endif
    libhandle = new QLibrary( libName.c_str() );
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_handles.insert(std::make_pair(libName,libhandle));
      return true;
    }
    else
    {
      std::cerr << "Failed to open lib " << libhandle->errorString().toUtf8().data() << std::endl;
      delete libhandle;
      libhandle = 0;
      return false;
    }
  }
  else {
#ifdef DEBUG_CD
    std::cerr << "the library " << libName.c_str() << " was loaded" << std::endl;
#endif
    m_handles[libName]=libhandle;
    return true;
  }
}

void DynamicLibrariesManager::
addSearchPath(const std::string& searchPath)
{
#ifdef DEBUG_CD
  std::cerr << "adding search path '"<<searchPath.c_str()<<"'" << std::endl;
#endif
  m_supplementarySearchPath.push_back(searchPath);
}


} // end namespace
} // end namespace
