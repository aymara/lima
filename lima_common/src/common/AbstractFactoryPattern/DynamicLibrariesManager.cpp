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
 * @file       DynamicLibrariesManager.cpp
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Wed Feb  6 2008
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

class DynamicLibrariesManagerPrivate
{
friend class DynamicLibrariesManager;
  DynamicLibrariesManagerPrivate();

  std::map<std::string, std::shared_ptr< QLibrary > > m_handles;
  // at load time, will try to load the libraries 
  // from these paths before the default ones
  std::vector<std::string> m_supplementarySearchPath;
};

DynamicLibrariesManagerPrivate::DynamicLibrariesManagerPrivate() :
    m_handles(),
    m_supplementarySearchPath()
{
}

  
DynamicLibrariesManager::DynamicLibrariesManager() : 
    m_d(new DynamicLibrariesManagerPrivate())
{
}

DynamicLibrariesManager::~DynamicLibrariesManager() 
{
}

bool DynamicLibrariesManager::
isLoaded(const std::string& libName)
{
  auto it=m_d->m_handles.find(libName);
  return (it!=m_d->m_handles.end());
}

bool DynamicLibrariesManager::loadLibrary(const std::string& libName)
{
  ABSTRACTFACTORYPATTERNLOGINIT;
  LDEBUG <<"DynamicLibrariesManager::loadLibrary() -- libName=" << libName ;
  auto it=m_d->m_handles.find(libName);
  if (it != m_d->m_handles.end()) 
  {
    LDEBUG << "DynamicLibrariesManager::loadLibrary trying to reload dynamic library" 
            << libName;
    return false;
  }

  std::shared_ptr< QLibrary > libhandle;
  // try supplementary search path
  for (auto it = m_d->m_supplementarySearchPath.begin(); 
       it != m_d->m_supplementarySearchPath.end(); it++)
  {
    LDEBUG << "Trying supplementary " << ((*it)+"/"+libName).c_str();
    libhandle = std::shared_ptr<QLibrary>(new QLibrary( ((*it)+"/"+libName).c_str() ));
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint 
                          | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_d->m_handles.insert(std::make_pair(libName,libhandle));
      LDEBUG << "the library " << libName.c_str() 
              << " was loaded from supplementary search path";
      LDEBUG << "the library fully-qualified name: " << libhandle->fileName();
      return true;
    }
    else
    {
      ABSTRACTFACTORYPATTERNLOGINIT;
      LERROR <<"DynamicLibrariesManager::loadLibrary() -- "
              <<"Failed to open lib " << libhandle->errorString();
    }
  }
  // now try system default search path
  if (libhandle == 0)
  {
    LINFO << "Trying " << libName.c_str();
    libhandle = std::shared_ptr<QLibrary>( new QLibrary( libName.c_str() ) );
    libhandle->setLoadHints(QLibrary::ResolveAllSymbolsHint 
                          | QLibrary::ExportExternalSymbolsHint);
    if (libhandle->load())
    {
      m_d->m_handles.insert(std::make_pair(libName,libhandle));
      LDEBUG << "the library " << libName 
              << " was loaded from system default search path";
      LDEBUG << "the library fully-qualified name: " << libhandle->fileName();
      return true;
    }
    else
    {
      ABSTRACTFACTORYPATTERNLOGINIT;
      LERROR <<"DynamicLibrariesManager::loadLibrary() -- "
              << "Failed to open lib " << libhandle->errorString();
      return false;
    }
  }
  else 
  {
    m_d->m_handles[libName]=libhandle;
    LDEBUG << "the library " << libName << " was loaded";
    return true;
  }
}

void DynamicLibrariesManager::addSearchPath(const std::string& searchPath)
{
  if( std::find(
                m_d->m_supplementarySearchPath.begin(), 
                m_d->m_supplementarySearchPath.end(), searchPath) 
      != m_d->m_supplementarySearchPath.end())
  {
    return;
  }
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
  LINFO << "adding search path '"<<searchPath.c_str()<<"'";
  std::cout<< "adding search path '"<<searchPath.c_str()<<"'" << std::endl;
#endif
  m_d->m_supplementarySearchPath.push_back(searchPath);
}

void DynamicLibrariesManager::addSearchPathes(QString searchPathes)
{
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
#endif
  QStringList list = searchPathes.replace("\\","/")
                                    .split(QRegularExpression("[;]"), 
                                          QString::SkipEmptyParts);
  for(auto it = list.begin(); it!=list.end();++it) 
  {
    QString searchPath = *it;
#ifdef DEBUG_CD
    LINFO << "DynamicLibrariesManager::addSearchPathes() -- " 
          << "adding:" << searchPath.toUtf8().data();
#endif
    this->addSearchPath(searchPath.toUtf8().data());
  }
}


} // end namespace
} // end namespace
