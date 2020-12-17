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
#include "QsLogCategories.h"
#include "common/tools/LimaFileSystemWatcher.h"
#include "common/tools/FileUtils.h"

#ifdef WIN32
#pragma warning(disable: 4127)
#endif
#include <QFile>
#include <QList>
#include <QDateTime>
#include <QtGlobal>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <cassert>
#include <cstdlib>
#include <stdexcept>

using namespace Lima;
using namespace Lima::Common::Misc;

namespace QsLogging
{

//static const int init = initQsLog();

class CategoriesImpl
{
public:
   CategoriesImpl()
   {
   }
   QMap<QString,Level> categories;
   /// Association category name / file name. Allows to inform of overwrites.
   QMap< QString, QString > m_categorySources;

   LimaFileSystemWatcher m_configFileWatcher;
   bool m_initialized = false;

   /// Store the list of configuration files already loaded. Allows to avoid
   /// reading several times the same file.
   QStringList m_configuredFiles;
};

Categories::Categories(QObject* parent) :
  QObject(parent),
  d(new CategoriesImpl())
{
  const QString category = "FilesReporting";
#ifdef DEBUG_CD
  d->categories.insert(category, QsLogging::InfoLevel);
#else
  d->categories.insert(category, QsLogging::ErrorLevel);
#endif
  d->categories.insert("Logging", QsLogging::TraceLevel);

}

void Categories::connectSignals()
{
  connect(&d->m_configFileWatcher, SIGNAL(fileChanged(QString)),
          this, SLOT(configureFileChanged(QString)));
}

Categories::~Categories()
{
   delete d;
}

void Categories::configureFileChanged( const QString & path )
{
  if (QFile(path).exists())
  {
    configure(path);
  }
}

bool Categories::configure(const QString& fileName, bool reload)
{
  d->m_initialized = true;
  const QString local_zone("FilesReporting");
  static LogInit logInit(local_zone.toUtf8().constData());
  auto& logger = *(logInit.pLogger);

//   std::cerr << "Categories::configure " << fileName.toStdString() << " "
//             << reload << std::endl;

  QFile file(fileName);
  QFileInfo fileInfo(fileName);
  QDir configDir = fileInfo.dir();

  if (!file.open(QIODevice::ReadOnly))
  {
    std::cerr << "Unable to open qslog configuration file: \"" << fileName.toUtf8().data() << "\"" << std::endl;
    return false;
  }
  if (!reload && d->m_configuredFiles.contains(fileName))
  {
    LOGINIT("Logging");
    LDEBUG << "Destinations::configure configuration file: \"" << fileName
            << "\" already configured";
    return true;
  }
  d->m_configFileWatcher.addPath(fileName);
  d->m_configuredFiles.append(fileName);
  d->m_configuredFiles.removeDuplicates();

  bool res = true;
  QTextStream in(&file);
  auto line = in.readLine();
  while (!line.isNull())
  {
    if (!line.startsWith("#"))
    {
      auto elts = line.split("=");
      if (elts.size()==2 && elts.at(0).trimmed().startsWith("log4j.category."))
      {
        auto category = elts.at(0).trimmed().remove(0, QLatin1String("log4j.category.").size());
        auto value = elts.at(1).trimmed();
        if (d->m_categorySources.contains(category))
        {
          LOGINIT("Logging");
          LDEBUG << fileName << "overwrites category definition"
                << category << "from"
                << d->m_categorySources[category];
        }
        d->m_categorySources.insert(category, fileName);
        if (value == "TRACE")
          d->categories.insert(category,QsLogging::TraceLevel);
        else if (value == "DEBUG")
          d->categories.insert(category,QsLogging::DebugLevel);
        else if (value == "INFO")
          d->categories.insert(category,QsLogging::InfoLevel);
        else if (value == "WARN")
          d->categories.insert(category,QsLogging::WarnLevel);
        else if (value == "ERROR")
          d->categories.insert(category,QsLogging::ErrorLevel);
        else if (value == "FATAL")
          d->categories.insert(category,QsLogging::FatalLevel);
        else
        {
          std::cerr << "Error reading \"" << fileName.toUtf8().constData()
                    << "\": unknown level \"" << value.toUtf8().constData()
                    << "\". Using TRACE" << std::endl;
          res = false;
          d->categories.insert(category,QsLogging::TraceLevel);
        }
        if(category == local_zone) {
            // Ensure the local logger knows all the latest declared destinations
            for (auto destination: Destinations::instance().destinations())
            {
              logger.addDestination(destination);
            }
            // Ensure the local logger has its latest declared level
            const QsLogging::Level lvl = QsLogging::Categories::instance().levelFor(local_zone);
            logger.setLoggingLevel(lvl);
        }
      }
      else if (elts.size()==2 && elts.at(0).trimmed() == "include")
      {
        auto includedFileName = elts.at(1).trimmed();
        auto includedInitFileName = includedFileName;
        if  (!QFileInfo(includedInitFileName).isAbsolute())
        {
          includedInitFileName = configDir.filePath(includedInitFileName);
        }
        res &= configure(includedInitFileName);
      }
    }
    line = in.readLine();
  }
  LINFO << "QsLog conf file loaded:" << fileName;
  return res;
}

Level Categories::levelFor(const QString& category) const
{
  if (!d->m_initialized) initQsLog();
#ifdef DEBUG_CD
  // Do not compile this costly check in release
  if (!d->categories.contains(category))
  {
    std::cerr << "Error: unknown category. Using TRACE for \""
              << category.toUtf8().constData() << "\"" << std::endl;
  }
#endif
  return d->categories.value(category, QsLogging::TraceLevel);
}

LIMA_COMMONQSLOG_EXPORT bool initQsLog(const QString& configString)
{
//   std::cerr << "initQsLog " << configString.toStdString() << std::endl;
  QsLogging::Categories::instance();
  QsLogging::Destinations::instance().setDefault();

  bool atLeastOneSuccessfulLoad = false;
  bool atLeastOneDestinationSuccessfulLoad = false;
  QStringList configDirsList;
  if (configString.isEmpty())
  {
    configDirsList = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                       QStringList());
  }
  else
  {
    configDirsList = configString.split(LIMA_PATH_SEPARATOR);
  }
  try
  {
    while (! configDirsList.isEmpty() )
    {
      QString configDir = configDirsList.last();
      configDirsList.pop_back();
      QString initFileName = configDir + "/log4cpp.properties";
      if (QFileInfo::exists(initFileName))
      {
        if (QsLogging::Destinations::instance().configure(initFileName))
        {
          atLeastOneDestinationSuccessfulLoad = true;
          QsLogging::Destinations::instance().removeDefault();
        }
//         else
//         {
//           std::cerr << "Configure Problem \"" << initFileName.toUtf8().constData() << "\"" << std::endl;
//           return false;
//         }
        if (QsLogging::Categories::instance().configure(initFileName))
        {
          atLeastOneSuccessfulLoad = true;
        }
//         else
//         {
//           std::cerr << "Configure Problem \"" << initFileName.toUtf8().constData() << "\"" << std::endl;
//           return false;
//         }
      }
      QDir initDir( configDir + "/log4cpp", "*.properties",
                    QDir::Unsorted, QDir::Files);
      if (initDir.exists())
      {
        QStringList entryList = initDir.entryList();
        for(QString entry : entryList)
        {
          if (QsLogging::Destinations::instance().configure(configDir + "/log4cpp/" + entry))
          {
            atLeastOneDestinationSuccessfulLoad = true;
            QsLogging::Destinations::instance().removeDefault();
          }
          else
          {
//             std::cerr << "Configure Problem \"" << entry.toUtf8().constData() << "\". No destination available" << std::endl;
            return false;
          }
          if (QsLogging::Categories::instance().configure(configDir + "/log4cpp/" + entry))
          {
            atLeastOneSuccessfulLoad = true;
          }
          else
          {
//             std::cerr << "Configure Problem \"" << entry.toUtf8().constData() << "\". No category loaded" << std::endl;
            return false;
          }
        }
      }

    }
  }
  catch(...)
  {
    std::cerr << "Exception during logging system configuration" << std::endl;
    return false;
  }
//   if (!atLeastOneSuccessfulLoad)
//   {
//     std::cerr << "Configure Problem: no log configuration file has been found in \"" << configString.toStdString() << "\"" << std::endl;
//     return false;
//   }
  if (atLeastOneDestinationSuccessfulLoad)
  {
    QsLogging::Destinations::instance().removeDefault();
  }

  return true;
}

} // end namespace
