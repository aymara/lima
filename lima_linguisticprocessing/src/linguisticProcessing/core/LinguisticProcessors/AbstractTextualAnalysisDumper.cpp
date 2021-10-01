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
 * @file       AbstractAnalysisDumper.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 *
 ***********************************************************************/

#include <common/LimaCommon.h>
#include "AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include <iostream>
#include <fstream>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// constructors and destructors
AbstractTextualAnalysisDumper::AbstractTextualAnalysisDumper():
MediaProcessUnit(),
m_language(0),
m_out(0),
m_handlerName(),
m_outputFile(),
m_outputSuffix(),
m_stripInputSuffix(false),
m_append(false),
m_temporaryFileMetadata()
{
}

AbstractTextualAnalysisDumper::~AbstractTextualAnalysisDumper()
{
}

//***********************************************************************
void AbstractTextualAnalysisDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  m_language = manager->getInitializationParameters().media;

  try
  {
    m_handlerName=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )  { }  // do nothing, optional

  try
  {
    m_temporaryFileMetadata = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("temporaryFileMetadata").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value (empty)

  try
  {
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& ) {} // do nothing, optional

  try
  {
    m_stripInputSuffix=unitConfiguration.getBooleanParameter("stripInputSuffix");
  }
  catch (NoSuchParam& ) {} // do nothing, optional

  try
  {
    m_outputFile=unitConfiguration.getParamsValueAtKey("outputFile");
  }
  catch (NoSuchParam& ) {} // do nothing, optional

  try
  {
    string val=unitConfiguration.getParamsValueAtKey("append");
    if (val=="true" || val=="yes" || val=="1") {
      m_append=true;
    }
    else {
      m_append=false;
    }
  }
  catch (NoSuchParam& ) {} // keep default value
}

std::shared_ptr<DumperStream> AbstractTextualAnalysisDumper::initialize(
    AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
// #ifdef DEBUG_LP
  LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream" << m_handlerName;
// #endif

  // if handler is defined, find handler
  if (! m_handlerName.empty()) {
// #ifdef DEBUG_LP
    LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream with handler "<< m_handlerName;
// #endif
    AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
    AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handlerName));
    if (handler==0)
    {
      DUMPERLOGINIT;
      LWARN << "AbstractTextualAnalysisDumper::initialize handler " << m_handlerName << " has not been given to the core client";
    }
    else {
      return std::make_shared<DumperStream>(handler);
    }
  }

  if (! m_temporaryFileMetadata.isEmpty()) {
// #ifdef DEBUG_LP
    LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream with temporary file metadata";
// #endif
    LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0) {
        LERROR << "AbstractTextualAnalysisDumper::initialize no LinguisticMetaData ! abort";
    }
// #ifdef DEBUG_LP
    LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream with metadata value"<< metadata->getMetaData(m_temporaryFileMetadata.toUtf8().constData());
// #endif
    return std::make_shared<DumperStream>(
      metadata->getMetaData(m_temporaryFileMetadata.toUtf8().constData()),
      m_append);
  }

  if (! m_outputFile.empty()) {
// #ifdef DEBUG_LP
    LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream with output file"<< m_outputFile << m_append;
// #endif
    return std::make_shared<DumperStream>(m_outputFile, m_append);
  }

  if (! m_outputSuffix.empty()) {
    LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0) {
      DUMPERLOGINIT;
      LERROR << "AbstractTextualAnalysisDumper::initialize: no LinguisticMetaData ! abort";
    }
    else {
      std::string sourceFile(metadata->getMetaData("FileName"));
      // remove suffix if necessary
      if (m_stripInputSuffix) {
        size_t lastIndex = sourceFile.find_last_of(".");
        if (lastIndex != std::string::npos) {
          sourceFile = sourceFile.substr(0, lastIndex);
        }
      }

// #ifdef DEBUG_LP
      LDEBUG << "AbstractTextualAnalysisDumper: initialize DumperStream with output suffix "
             << m_outputSuffix << " on file " << sourceFile;
// #endif
      string outputFile=sourceFile + m_outputSuffix;
      return std::make_shared<DumperStream>(outputFile, m_append);
    }
  }

  // return
  LERROR << "AbstractTextualAnalysisDumper::initialize: missing parameters to initialize output stream: use default file 'output'";
  return std::make_shared<DumperStream>("output", m_append);
}

} // end namespace
} // end namespace
