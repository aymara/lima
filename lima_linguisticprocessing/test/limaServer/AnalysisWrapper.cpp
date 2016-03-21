/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "AnalysisWrapper.h"
#include "common/LimaCommon.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/Handler/AbstractXmlDocumentHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/core/EventAnalysis/EventHandler.h"

// factories
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"

#include <QSharedPointer>
#include <QStringList>
#include <QPair>
#include <boost/graph/buffer_concepts.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <sstream>      // std::stringstream
#include <QtCore/QTemporaryFile>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;


class AnalysisWrapperPrivate
{
friend class AnalysisWrapper;
public:
  AnalysisWrapperPrivate (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
                        const std::set<std::string>& langs);
  ~AnalysisWrapperPrivate() {}
  
  std::ostream* openHandlerOutputFile(AbstractTextualAnalysisHandler* handler, const std::string& fileName, const std::set<std::string>&dumpers, const std::string& dumperId);
  void closeHandlerOutputFile(std::ostream* ofs);

  boost::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > m_analyzer;
  const std::set<std::string>& m_langs;
};

AnalysisWrapperPrivate::AnalysisWrapperPrivate(Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
                    const std::set<std::string>& langs) :
    m_analyzer(analyzer),
    m_langs(langs)
{
}

std::ostream* AnalysisWrapperPrivate::openHandlerOutputFile(AbstractTextualAnalysisHandler* handler, const std::string& fileName, const std::set<std::string>&dumpers, const std::string& dumperId)
{
  std::ostream* ofs = 0;
  if (dumpers.find(dumperId)!=dumpers.end())
  {
    if (fileName=="stdout")
    {
      ofs = &std::cout;
    }
    else
    {
    ofs = new std::ofstream(fileName.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    }
    if (ofs->good()) {
      handler->setOut(ofs);
    }
    else {
      std::cerr << "failed to open file " << fileName << std::endl;
      delete ofs; ofs = 0;
    }
  }
  return ofs;
}

void AnalysisWrapperPrivate::closeHandlerOutputFile(std::ostream* ofs)
{
  if (ofs != 0 && dynamic_cast<std::ofstream*>(ofs)!=0)
  {
    dynamic_cast<std::ofstream*>(ofs)->close();
    delete ofs;
    ofs = 0;
  }
}


AnalysisWrapper::AnalysisWrapper (Lima::LinguisticProcessing::AbstractLinguisticProcessingClient* analyzer,
                  const std::set<std::string>& langs, QObject* parent ):
    QObject(parent),
    m_d(new AnalysisWrapperPrivate(analyzer,langs))
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisWrapper::AnalysisWrapper()...";
}

AnalysisWrapper::~AnalysisWrapper()
{
  delete m_d;
}

QString AnalysisWrapper::analyze(const QString& text, const QString& language, const QString& pipeline)
{
  CORECLIENTLOGINIT;
  LDEBUG << "AnalysisWrapper::analyze" << text.left(100);

  QTemporaryFile tempFile;
  QPair<QString, QString> item;
  std::map<std::string,std::string> metaData;
  metaData["Lang"]=language.toUtf8().constData();
  metaData["FileName"]=tempFile.fileName().toUtf8().constData();

  std::set<std::string> inactiveUnits;

  std::map<std::string, AbstractAnalysisHandler*> handlers;
  BowTextWriter bowTextWriter;
  EventAnalysis::EventHandler eventHandler;
  BowTextHandler bowTextHandler;
  SimpleStreamHandler simpleStreamHandler;
  SimpleStreamHandler fullXmlSimpleStreamHandler;

  std::ostringstream oss;
  bowTextWriter.setOut(&oss);
  eventHandler.setOut(&oss);
  bowTextHandler.setOut(&oss);
  simpleStreamHandler.setOut(&oss);
  fullXmlSimpleStreamHandler.setOut(&oss);

  handlers.insert(std::make_pair("eventHandler", &eventHandler));
  handlers.insert(std::make_pair("bowTextWriter", &bowTextWriter));
  handlers.insert(std::make_pair("bowTextHandler", &bowTextHandler));
  handlers.insert(std::make_pair("simpleStreamHandler", &simpleStreamHandler));
  handlers.insert(std::make_pair("fullXmlSimpleStreamHandler", &fullXmlSimpleStreamHandler));

  try
  {
    m_d->m_analyzer->analyze(text,metaData, pipeline.toUtf8().constData(), handlers, inactiveUnits);
  }
  catch (const Lima::MediaNotInitialized& e)
  {
    LERROR << "Catch MediaNotInitialized" << e.what();
    return QString::fromUtf8(e.what());
  }
  return QString::fromUtf8(oss.str().c_str());
}
