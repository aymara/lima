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
#elif __APPLE__
//
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

#ifdef WIN32
// Directory of the DLL containing this code (lima-common-factory.dll).
static std::string ownLibraryDirectory()
{
  HMODULE module = nullptr;
  if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                            | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          reinterpret_cast<LPCWSTR>(&ownLibraryDirectory),
                          &module))
    return std::string();
  wchar_t path[4096];
  auto length = GetModuleFileNameW(module, path, sizeof(path) / sizeof(path[0]));
  if (length == 0 || length >= sizeof(path) / sizeof(path[0]))
    return std::string();
  return boost::filesystem::path(std::wstring(path, length)).parent_path().string();
}
#endif

DynamicLibrariesManagerPrivate::DynamicLibrariesManagerPrivate() :
    m_handles(),
    m_supplementarySearchPath()
{
#ifdef WIN32
  // Windows has no RPATH, and plain LoadLibrary does not search the
  // directories added with AddDllDirectory (e.g. by Python): look for the
  // plugins next to the LIMA libraries (bin/ of an install, aymara.libs/ of
  // the Python wheel). Loaded by absolute path, their own dependencies are
  // then also searched in that directory.
  auto dir = ownLibraryDirectory();
  if (!dir.empty())
    m_supplementarySearchPath.push_back(dir);
#endif
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
        // szModule is a TCHAR array (wide when UNICODE is defined, as with Qt):
        // let boost::filesystem::path do the conversion in both cases.
        boost::filesystem::path p(me32.szModule);
        if (p.empty())
        {
            CloseHandle(hModuleSnap);
            return false;
        }

        std::string fn = p.filename().string();
        if (fn.find(libName, 0) == 0) // [plugin-name]
        {
            //std::cerr << "An attempt to load same lib twice detected" << std::endl;
            CloseHandle(hModuleSnap);
            return true;
        }

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
#elif __APPLE__
//
#else
    std::pair<bool&, const std::string&> data(alreadyLoaded, libName);

    dl_iterate_phdr([](dl_phdr_info *info, size_t size, void* data) -> int {
        LIMA_UNUSED(size);

        if (data == nullptr)
            throw LimaException("Callback passed to dl_iterate_phdr unexpectedly got NULL in \"data\" argument.");

        auto& d = *((std::pair<bool&, const std::string&>*)data);

        //std::cerr << info->dlpi_name << std::endl;

        if (info == nullptr || info->dlpi_name == nullptr)
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

bool DynamicLibrariesManager::isLoaded(const std::string& libName)
{
  m_d->isSomethingSimilarLoaded(libName);
  auto it = m_d->m_handles.find(libName);
  return (it != m_d->m_handles.end());
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

  auto it = m_d->m_handles.find(libName);
  if (it != m_d->m_handles.end())
  {
    LTRACE << "DynamicLibrariesManager::loadLibrary trying to reload dynamic library." << libName;
    LTRACE << "DynamicLibrariesManager::loadLibrary this is not an error, silently ignoring.";
    return true;
  }

  std::shared_ptr< QLibrary > libhandle;
  std::vector< std::pair< std::string, std::string> > triedPaths;
  // try supplementary search path
  for (const auto& path: m_d->m_supplementarySearchPath)
  {
    LDEBUG << "Trying supplementary " << (path+"/"+libName).c_str();
#ifdef WIN32
    // QLibrary uses LoadLibrary, which searches the dependencies of the
    // plugin in the standard directories only (executable's, system, PATH).
    // Load it first so that they are also searched next to it and in the
    // directories added with AddDllDirectory (e.g. by Python's
    // os.add_dll_directory); QLibrary then gets the already loaded module.
    {
      auto dllPath = boost::filesystem::path(path) / (libName + ".dll");
      if (boost::filesystem::exists(dllPath))
      {
        if (LoadLibraryExW(dllPath.wstring().c_str(), nullptr,
                           LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
                           | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) == nullptr)
        {
          ABSTRACTFACTORYPATTERNLOGINIT;
          LERROR << "DynamicLibrariesManager::loadLibrary() -- LoadLibraryExW"
                 << dllPath.string() << "failed with error" << GetLastError();
        }
      }
    }
#endif
    libhandle = std::make_shared<QLibrary>((path+"/"+libName).c_str());

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
      triedPaths.emplace_back( path, libhandle->errorString().toStdString() );
      libhandle.reset();

    }
  }
  // now try system default search path
  if (!libhandle)
  {
    LDEBUG << "Trying " << libName;
    libhandle = std::make_shared<QLibrary>( libName.c_str() );
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
      for (const auto& pathErr: triedPaths)
      {
        LERROR << "Failed to load library " << libName << "from " << pathErr.first << " due to " << pathErr.second;
      }
      LERROR << "Failed to load library " << libName << "also from system paths";
      LERROR << "DynamicLibrariesManager::loadLibrary() -- "
             << "Failed to open system lib " << libhandle->errorString();
      return false;
    }
  }
  else
  {
    m_d->m_handles[libName] = libhandle;
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
