// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       recognizerResultLogger.cpp
 * @author     besancon (romaric.besancon@cea.fr)
 * @date       Tue Oct 25 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 *
 ***********************************************************************/

#include "recognizerResultLogger.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include <fstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

SimpleFactory<MediaProcessUnit,RecognizerResultLogger>
RecognizerResultLogger(RECOGNIZERRESULTLOGGER_CLASSID);

RecognizerResultLogger::RecognizerResultLogger():
AbstractLinguisticLogger(".recoData.xml"),
m_language(0),
m_data()
{
}

RecognizerResultLogger::~RecognizerResultLogger() {
}

void RecognizerResultLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try {
    m_data=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    APPRLOGINIT;
    LERROR << "missing \"data\" parameter in configuration of RecognizerResultLogger"
           << " for language " << (int) m_language;
    throw InvalidConfiguration();
  }
}

LimaStatusCode RecognizerResultLogger::process(AnalysisContent& analysis) const
{

  APPRLOGINIT;
  LDEBUG << "RecognizerResultLogger: logging data "<< m_data;
  // get metadata (for filename)
  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
    APPRLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  std::ofstream fout;
  if (!openLogFile(fout,metadata->getMetaData("FileName"))) {
      APPRLOGINIT;
      LERROR << "DebugSyntacticAnalysisLogger::Cannot open log file";
      return CANNOT_OPEN_FILE_ERROR;
  }

  uint64_t offset(0);
  try {
    offset=atoi(metadata->getMetaData("StartOffset").c_str());
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  uint64_t offsetIndexingNode(0);
  try {
    offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

 // get result data
  auto resultData = analysis.getData(m_data);
  if (resultData == 0) {
    APPRLOGINIT;
    LERROR << "no data " << m_data << "in AnalysisContent";
    return MISSING_DATA;
  }
  auto recoData = std::dynamic_pointer_cast<RecognizerResultData>(resultData);
  if (recoData == 0) {
    APPRLOGINIT;
    LERROR << "data " << m_data << "in AnalysisContent is not a RecognizerResultData";
    return MISSING_DATA;
  }

  // print results in log file
  RecognizerResultData::const_iterator
    it=recoData->begin(),
    it_end=recoData->end();
  fout << "<entities offsetNode=\"" << offsetIndexingNode << "\">" << endl;
  for (; it!=it_end; it++) {
    auto m = (*it).cbegin(), m_end = (*it).cend();
    for (; m != m_end; m++)
    {
      //LimaString normalizedForm=(*m).getNormalizedForm().normStr();
      fout << "<entity>"
           << "<pos>" << offset+(*m).positionBegin() << "</pos>"
           << "<len>" << (*m).length() << "</len>"
        //<< "<typeNum>" << (*m).getType() << "</typeNum>"
           << "<type>"
           << Common::MediaticData::MediaticData::single().
              getEntityName((*m).getType()).toStdString()
           << "</type>"
           << "<string>"<< (*m).getString().toStdString() << "</string>"
           << "<norm>";
      const auto& features = m->features();
      for (auto featureItr = features.cbegin(), features_end = features.cend();
           featureItr != features_end; featureItr++)
      {
        fout << "<" << featureItr->getName() << ">"
             << featureItr->getValueString()
             << "</" << featureItr->getName() << ">";
      }

      fout << "</norm>"
           << "</entity>"
           << endl;
    }
  }
  fout << "</entities>" << endl;
  fout.close();
  return SUCCESS_ID;
}


} // end namespace
} // end namespace
} // end namespace
