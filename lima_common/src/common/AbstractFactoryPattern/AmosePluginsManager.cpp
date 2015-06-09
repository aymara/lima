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
#include "AmosePluginsManager.h"
#include "common/LimaCommon.h"
#include "common/AbstractFactoryPattern/DynamicLibrariesManager.h"

#include <iostream>
#include <QFile>
#include <QDir>

using namespace Lima;
using namespace Lima::Common;

AmosePluginsManager::AmosePluginsManager()
{
  loadPlugins();
}

bool AmosePluginsManager::loadPlugins()
{
  ABSTRACTFACTORYPATTERNLOGINIT;
  LINFO << "AmosePluginsManager::loadPlugins";
//   DynamicLibrariesManager::changeable().addSearchPath("c:\amose\lib");;
  // open LIMA_CONF/plugins file
  QDir pluginsDir(QString::fromUtf8(qgetenv("LIMA_CONF").constData()==0?"":qgetenv("LIMA_CONF").constData()) + "/plugins");
  QStringList pluginsFiles = pluginsDir.entryList(QDir::Files);
  Q_FOREACH(QString pluginsFile, pluginsFiles)
  {
    LINFO << "AmosePluginsManager::loadPlugins loding plugins file " << pluginsFile.toUtf8().data();
    QFile file(pluginsDir.path() + "/" + pluginsFile);
    if (!file.open(QIODevice::ReadOnly))
      return false;
    // for each entry, call load library
    while (!file.atEnd()) 
    {
      QByteArray line = file.readLine();
      if (line.endsWith('\n')) line.chop(1);
      // Allows empty and comment lines
      if ( !line.isEmpty() && !line.startsWith('#') )
      {
#ifdef WIN32
        QString strline = QString(line.data()).trimmed() + ".dll";
        QString library_path=QString::fromUtf8(qgetenv("LD_LIBRARY_PATH").constData()==0?"c:\amose\lib":qgetenv("LD_LIBRARY_PATH").constData());
        DynamicLibrariesManager::changeable().addSearchPathes( library_path.toUtf8().data());
#else
        QString strline = QString("lib") + line.data() + ".so";
#endif
        LDEBUG << "AmosePluginsManager::loadPlugins loading plugin '" << strline.toUtf8().data() << "'";
        std::cout << "AmosePluginsManager::loadPlugins loading plugin '" << strline.toUtf8().data() << "'"<<std::endl;
        DynamicLibrariesManager::changeable().loadLibrary(strline.toUtf8().data());
      }
    }
  }
  return true;
}
