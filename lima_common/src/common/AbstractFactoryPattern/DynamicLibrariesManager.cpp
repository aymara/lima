// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Wed Feb  6 2008
 */

#include "DynamicLibrariesManager.h"
#include "common/LimaCommon.h"

#include <boost/filesystem.hpp>

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#else
#include <link.h>
#endif

#include <iostream>
#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
using namespace std;

namespace Lima {
namespace Common {

class DynamicLibrariesManagerPrivate
{
friend class DynamicLibrariesManager;
  DynamicLibrariesManagerPrivate();

  bool isSomethingSimilarLoaded(const std::string& libName) const;

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

bool DynamicLibrariesManagerPrivate::isSomethingSimilarLoaded(const std::string& libName) const
{
    bool alreadyLoaded = false;

#ifdef WIN32
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hModuleSnap == INVALID_HANDLE_VALUE)
        throw LimaException("CreateToolhelp32Snapshoot returned INVALID_HANDLE_VALUE.");

    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);
        throw LimaException("Module32First failed.");
    }

    do
    {
        //std::cerr << me32.szModule << std::endl;
        std::string s(me32.szModule);
        if (s.size() == 0)
            return false;

        boost::filesystem::path p(me32.szModule);
        std::string fn = p.filename().string();
        if (fn.find(libName, 0) == 0) // [plugin-name]
        {
            //std::cerr << "An attempt to load same lib twice detected" << std::endl;
            return true;
        }

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
#else
    std::pair<bool&, const std::string&> data(alreadyLoaded, libName);

    dl_iterate_phdr([](dl_phdr_info *info, size_t size, void* data) -> int {
        LIMA_UNUSED(size);

        if (data == NULL)
            throw LimaException("Callback passed to dl_iterate_phdr unexpectedly got NULL in \"data\" argument.");

        auto& d = *((std::pair<bool&, const std::string&>*)data);

        //std::cerr << info->dlpi_name << std::endl;

        if (info == NULL || info->dlpi_name == NULL)
            throw LimaException("Callback passed to dl_iterate_phdr unexpectedly got NULL in \"info\" argument.");

        std::string s(info->dlpi_name);
        if (s.size() == 0)
            return 0;

        boost::filesystem::path p(info->dlpi_name);
        std::string fn = p.filename().string();
        if (fn.find(d.second, 0) == 3) // lib[plugin-name]
        {
            d.first = true;
            return 1;
        }

        return 0;
    }, &data);
#endif

    return alreadyLoaded;
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
  m_d->isSomethingSimilarLoaded(libName);
  auto it=m_d->m_handles.find(libName);
  return (it!=m_d->m_handles.end());
}

bool DynamicLibrariesManager::loadLibrary(const std::string& libName)
{
  ABSTRACTFACTORYPATTERNLOGINIT;
  LDEBUG << "DynamicLibrariesManager::loadLibrary() -- libName=" << libName;

  if (m_d->isSomethingSimilarLoaded(libName))
  {
    LTRACE << "DynamicLibrariesManager::loadLibrary trying to reload dynamic library " << libName;
    LTRACE << "DynamicLibrariesManager::loadLibrary this is not an error, silently ignoring.";
    return true;
  }

  auto it=m_d->m_handles.find(libName);
  if (it != m_d->m_handles.end())
  {
    LTRACE << "DynamicLibrariesManager::loadLibrary trying to reload dynamic library." << libName;
    LTRACE << "DynamicLibrariesManager::loadLibrary this is not an error, silently ignoring.";
    return true;
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
      LDEBUG << "DynamicLibrariesManager::loadLibrary() -- "
             << "Failed to open supplementary lib " << libhandle->errorString();
      libhandle.reset();
    }
  }
  // now try system default search path
  if (!libhandle)
  {
    LDEBUG << "Trying " << libName.c_str();
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
      LERROR << "DynamicLibrariesManager::loadLibrary() -- "
             << "Failed to open system lib " << libhandle->errorString();
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
  LINFO << "adding search path '" << searchPath.c_str() << "'";
//   std::cerr << "adding search path '" <<searchPath.c_str() << "'" << std::endl;
#endif
  m_d->m_supplementarySearchPath.push_back(searchPath);
}

void DynamicLibrariesManager::addSearchPathes(QString searchPathes)
{
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
#endif

#ifdef WIN32
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  auto list = searchPathes.replace("\\","/").split(QRegularExpression("[;]"),
                                                   QString::SkipEmptyParts);
#else
  auto list = searchPathes.replace("\\","/").split(QRegularExpression("[;]"),
                                                   Qt::SkipEmptyParts);
#endif
#else
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  auto list = searchPathes.replace("\\","/").split(QRegularExpression("[:;]"),
                                                   QString::SkipEmptyParts);
#else
  auto list = searchPathes.replace("\\","/").split(QRegularExpression("[:;]"),
                                                   Qt::SkipEmptyParts);
#endif
#endif

  for(const auto& searchPath: list)
  {
#ifdef DEBUG_CD
    LINFO << "DynamicLibrariesManager::addSearchPathes() adding:" << searchPath;
#endif
    this->addSearchPath(searchPath.toStdString());
  }
}


} // end namespace
} // end namespace
