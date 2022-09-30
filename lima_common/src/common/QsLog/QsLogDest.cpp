// Copyright (c) 2010, Razvan Petru
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "QsLogDest.h"
#include "QsDebugOutput.h"

#include "common/tools/LimaFileSystemWatcher.h"
#include "common/tools/FileUtils.h"
#include "common/LimaCommon.h"

#ifdef WIN32
#pragma warning(disable: 4127)
#endif
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QFileInfo>
#include <QDir>
#ifdef WIN32
#pragma warning(enable: 4127)
#endif

using namespace Lima;
using namespace Lima::Common::Misc;

namespace QsLogging
{

class DestinationsImpl
{
public:
   DestinationsImpl()
   {
   }
   QMap< QString, DestinationPtr > m_destinations;
   /// Association destination name / file name. Allows to inform of overwrites.
   QMap< QString, QString > m_destinationSources;

   LimaFileSystemWatcher m_configFileWatcher;
   /// Store the list of configuration files already loaded. Allows to avoid
   /// reading several times the same file.
   QStringList m_configuredFiles;
};

Destinations::Destinations(QObject* parent) :
  QObject(parent),
  d(new DestinationsImpl())
{
  connect(&d->m_configFileWatcher,SIGNAL(fileChanged(QString)),
          this,SLOT(configureFileChanged(QString)));
}

Destinations::~Destinations()
{
   delete d;
}

const QMap< QString, DestinationPtr >& Destinations::destinations() const
{
  return d->m_destinations;
}

void Destinations::configureFileChanged ( const QString & path )
{
  if (QFile(path).exists())
  {
    configure(path, true);
  }
  else
  {
    if (d->m_configuredFiles.contains(path))
    {
      d->m_configuredFiles.removeAll(path);
    }
  }
}

bool Destinations::setDefault()
{
//   std::cerr << "Destinations::setDefault" << std::endl;
  d->m_destinations.insert("DefaultOutput",
                            DestinationFactory::MakeDebugOutputDestination());
  return true;
}

bool Destinations::removeDefault()
{
  d->m_destinations.remove("DefaultOutput");
  return true;
}

bool Destinations::configure(const QString& fileName, bool reload)
{
  QFile file(fileName);
  QFileInfo fileInfo(fileName);
  QDir configDir = fileInfo.dir();

//   std::cerr << "Destinations::configure " << fileName.toStdString() << " " << reload << std::endl;

  if (!file.open(QIODevice::ReadOnly))
  {
    std::cerr << "Destinations::configure Unable to open qslog configuration file: \""
              << fileName.toStdString() << "\"" << std::endl;
    return false;
  }
  if (!reload && d->m_configuredFiles.contains(fileName))
  {
    LOGINIT("Logging");
    LDEBUG << "Destinations::configure configuration file: \""
              << fileName << "\" already configured";
    return true;
  }

  d->m_configFileWatcher.addPath(fileName);
  d->m_configuredFiles.append(fileName);
  d->m_configuredFiles.removeDuplicates();

  bool res = true;
  QTextStream in(&file);
  QString line = in.readLine();
  while (!line.isNull())
  {
    if (!line.startsWith("#"))
    {
      QStringList elts = line.split("=");
      if (elts.size()>0 && elts.at(0).trimmed().startsWith("log4j.appender."))
      {
        auto destination = elts.at(0).trimmed().split(".").last();
        QString value;
        if (elts.size()==2)
          value = elts.at(1).trimmed();
        if (d->m_destinations.contains(destination))
        {
          LOGINIT("Logging");
          LDEBUG << fileName << "overwrites destination definition"
                << destination << "from"
                << d->m_destinationSources[destination];
        }
        if (destination == "DebugOutput")
        {
          d->m_destinationSources.insert(destination, fileName);
          d->m_destinations.insert(destination,
                                   DestinationFactory::MakeDebugOutputDestination());
          d->m_destinations.remove("DefaultOutput");
        }
        else if (destination == "File")
        {
          d->m_destinationSources.insert(destination, fileName);
          d->m_destinations.insert(destination,
                                   DestinationFactory::MakeFileDestination(value));
        }
        else
        {
          std::cerr << "Error reading \"" << fileName.toUtf8().constData()
                    << "\": unknown destination \""
                    << destination.toUtf8().constData()
                    << std::endl;
          res = false;
        }
      }
      else if (elts.size()==2 && elts.at(0).trimmed() == "include")
      {
        QString includedFileName = elts.at(1).trimmed();
        QString includedInitFileName = includedFileName;
        if  (!QFileInfo(includedInitFileName).isAbsolute())
        {
          includedInitFileName = configDir.filePath(includedInitFileName);
        }
        res &= configure(includedInitFileName);
      }
    }
    line = in.readLine();
  }
  return res;
}


//! file message sink
class FileDestination : public Destination
{
public:
   FileDestination(const QString& filePath);
   virtual void write(const QString& message, const QString& zone = "") override;

private:
   QFile mFile;
   QTextStream mOutputStream;
};


FileDestination::FileDestination(const QString& filePath)
{
  mFile.setFileName(filePath);
  mFile.open(QFile::WriteOnly|QFile::Append); //fixme: should throw on failure
//   std::cerr << "FileDestination::FileDestination "
//             << QFileInfo(mFile).absoluteFilePath().toUtf8().constData()
//             << std::endl;
  mOutputStream.setDevice(&mFile);
}

void FileDestination::write(const QString& message, const QString& zone)
{
  if (!zone.isEmpty())
    mOutputStream << " : " << zone << " : ";
  mOutputStream << message << QTENDL;
  mOutputStream.flush();
}

void DebugOutputDestination::write(const QString& message, const QString& zone)
{
  if (!zone.isEmpty())
    QsDebugOutput::output(QString(QLatin1String(" : %1 : %2")).arg(zone, message));
  else
    QsDebugOutput::output(message);
}

DestinationPtr DestinationFactory::MakeFileDestination(const QString& filePath)
{
   return DestinationPtr(new FileDestination(filePath));
}

DestinationPtr DestinationFactory::MakeDebugOutputDestination()
{
   return DestinationPtr(new DebugOutputDestination);
}

} // end namespace

