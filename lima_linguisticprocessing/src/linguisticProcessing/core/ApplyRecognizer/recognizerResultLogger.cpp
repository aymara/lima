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
  LinguisticMetaData* metadata=dynamic_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
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
  AnalysisData* resultData=analysis.getData(m_data);
  if (resultData == 0) {
    APPRLOGINIT;
    LERROR << "no data " << m_data << "in AnalysisContent";
    return MISSING_DATA;
  }
  RecognizerResultData* recoData=dynamic_cast<RecognizerResultData*>(resultData);
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
    vector<Automaton::RecognizerMatch>::const_iterator
      m=(*it).begin(),
      m_end=(*it).end();
    for (; m!=m_end; m++) {
      //LimaString normalizedForm=(*m).getNormalizedForm().normStr();
      fout << "<entity>" 
           << "<pos>" << offset+(*m).positionBegin() << "</pos>" 
           << "<len>" << (*m).length() << "</len>" 
        //<< "<typeNum>" << (*m).getType() << "</typeNum>"
           << "<type>" 
           << Common::MediaticData::MediaticData::single().
              getEntityName((*m).getType())
           << "</type>"
           << "<string>"<< (*m).getString() << "</string>" 
           << "<norm>";
      const Automaton::EntityFeatures& features=m->features();
      for (Automaton::EntityFeatures::const_iterator 
             featureItr=features.begin(),features_end=features.end();
           featureItr!=features_end; featureItr++)
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
