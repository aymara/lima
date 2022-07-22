// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      logger for xml-formatted linguistic data in graph.
  *
  * @file       disambiguationLogger.cpp
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  */

#include "disambiguationLogger.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/LimaString.h"
#include "common/time/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace boost;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

struct lToken
{
  bool operator()(const Token* t1, const Token* t2) const
  {
    if (t1->position()!=t2->position()) return t1->position() < t2->position();
    return t1->length() < t2->length();
  }
};

SimpleFactory<MediaProcessUnit,DisambiguationLogger> disambiguationLoggerFactory(DISAMBIGUATIONLOGGER_CLASSID);

DisambiguationLogger::DisambiguationLogger():
AbstractLinguisticLogger(".disambiguation.log")
{}

DisambiguationLogger::~DisambiguationLogger()
{}

void DisambiguationLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup LoggerConfiguration
   * - <b>&lt;group name="..." class="DisambiguationLogger"&gt;</b>
   *  -  logAll : if 'true' then log all categories. Default is 'false'
   */
    
  m_language=manager->getInitializationParameters().media;
  m_microManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO"));
  try
  {
    m_logAll=(unitConfiguration.getParamsValueAtKey("logAll")=="true");
  }
  catch (NoSuchParam& )
  {
    m_logAll=false;
  }
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode DisambiguationLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      PTLOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  ofstream fout;
  openLogFile(fout,metadata->getMetaData("FileName"));

  map<Token*,set<LinguisticCode>,lToken > categoriesMapping;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  LinguisticGraph* graph=tokenList->getGraph();

  LinguisticGraphVertexIt vxItr,vxItrEnd;
  boost::tie(vxItr,vxItrEnd) = vertices(*graph);
  for (;vxItr!=vxItrEnd;vxItr++)
  {
    MorphoSyntacticData* data=get(vertex_data,*graph,*vxItr);
    Token* token=get(vertex_token,*graph,*vxItr);
    if( data!=0)
    {
      if (m_logAll)
      {
        data->allValues(m_microManager->getPropertyAccessor(),categoriesMapping[token]);
      }
      else
      {
        if (data->begin() != data->end())
        {
          LinguisticCode micro=m_microManager->getPropertyAccessor().readValue(data->begin()->properties);
          categoriesMapping[token].insert(micro);
        }
      }
    }
  }

  for (map<Token*,set<LinguisticCode>,lToken >::const_iterator ftItr=categoriesMapping.begin();
       ftItr!=categoriesMapping.end();
       ftItr++)
  {

    Token* ft=ftItr->first;
//    uint64_t nbmicros=ft->countMicros();
    std::ostringstream os;
    os << ft->position() << " | ";
    fout << os.str();
    fout << Common::Misc::limastring2utf8stdstring(ft->stringForm());
    std::ostringstream os2;
    for (std::set<LinguisticCode>::const_iterator catItr=ftItr->second.begin();
           catItr!=ftItr->second.end();
           catItr++)
      {
        os2 << " | " << m_microManager->getPropertySymbolicValue(*catItr);
      }
    fout << os2.str() << endl;

  }

  fout.close();
  TimeUtils::logElapsedTime("DisambiguationLogger");
  return SUCCESS_ID;
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
