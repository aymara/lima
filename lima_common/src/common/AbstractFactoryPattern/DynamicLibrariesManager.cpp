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
#include "common/LimaCommon.h"

#include <iostream>
#include<QString>
#include<QStringList>
#include<QRegularExpression>

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
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
  LDEBUG <<"DynamicLibrariesManager::loadLibrary() -- "<<"libName="<<libName ;
#endif
  std::map<std::string,QLibrary*>::const_iterator
    it=m_handles.find(libName);
  if (it!=m_handles.end()) {
#ifdef DEBUG_CD
    LWARN << "DEBUG_CD: trying to reload dynamic library " << libName.c_str();
#endif
  }

  QLibrary* libhandle = 0;
  // try supplementary search path
  for (std::vector<std::string>::const_iterator it = m_supplementarySearchPath.begin(); it != m_supplementarySearchPath.end(); it++)
  {
#ifdef DEBUG_FACTORIES
    LDEBUG << "Trying supplementary " << ((*it)+"/"+libName).c_str();
#endif
    libhandle = new QLibrary( ((*it)+"/"+libName).c_str() );
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_handles.insert(std::make_pair(libName,libhandle));
#ifdef DEBUG_CD
    LDEBUG << "the library " << libName.c_str() << " was loaded from supplementary search path";
    LDEBUG << "the library fully-qualified name: " << libhandle->fileName();
#endif
      return true;
    }
    else
    {
//      if ( QLibrary::isLibrary(((*it)+"/"+libName).c_str()) )
      ABSTRACTFACTORYPATTERNLOGINIT;
      LERROR <<"DynamicLibrariesManager::loadLibrary() -- "<<"Failed to open lib " << libhandle->errorString().toUtf8().data();
      delete libhandle;
      libhandle = 0;
    }
  }
  // now try system default search path
  if (libhandle == 0)
  {
#ifdef DEBUG_FACTORIES
    LINFO << "Trying " << libName.c_str();
#endif
    libhandle = new QLibrary( libName.c_str() );
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_handles.insert(std::make_pair(libName,libhandle));
#ifdef DEBUG_CD
    LDEBUG << "the library " << libName.c_str() << " was loaded from system default search path";
    LDEBUG << "the library fully-qualified name: " << libhandle->fileName();
#endif
      return true;
    }
    else
    {
      ABSTRACTFACTORYPATTERNLOGINIT;
      LINFO <<"DynamicLibrariesManager::loadLibrary() -- "<< "Failed to open lib " << libhandle->errorString().toUtf8().data();
      delete libhandle;
      libhandle = 0;
      return false;
    }
  }
  else {
    m_handles[libName]=libhandle;
#ifdef DEBUG_CD
    LDEBUG << "the library " << libName.c_str() << " was loaded";
#endif
    return true;
  }
}

void DynamicLibrariesManager::
addSearchPath(const std::string& searchPath)
{
  if(std::find(m_supplementarySearchPath.begin(), m_supplementarySearchPath.end(), searchPath)!=m_supplementarySearchPath.end()){
    return;
  }
#ifdef DEBUG_CD
    ABSTRACTFACTORYPATTERNLOGINIT;
    LINFO << "adding search path '"<<searchPath.c_str()<<"'";
    std::cout<< "adding search path '"<<searchPath.c_str()<<"'" << std::endl;
#endif
    m_supplementarySearchPath.push_back(searchPath);
  
}

void DynamicLibrariesManager::
addSearchPathes(QString searchPathes)
{
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
#endif
  QStringList list = searchPathes.replace("\\","/").split(QRegularExpression("[;]"), QString::SkipEmptyParts);
  for(QStringList::iterator it = list.begin();
        it!=list.end();++it) {
      QString searchPath = *it;
#ifdef DEBUG_CD
      LINFO << "DynamicLibrariesManager::addSearchPathes() -- " <<"adding:"<<searchPath.toUtf8().data();
#endif
      this->addSearchPath(searchPath.toUtf8().data());
    }
}


} // end namespace
} // end namespace
