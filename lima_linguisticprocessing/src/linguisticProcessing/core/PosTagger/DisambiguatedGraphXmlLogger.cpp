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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "DisambiguatedGraphXmlLogger.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
// #include "linguisticProcessing/core/Graph/phoenixGraph.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "common/misc/fsaStringsPool.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"


#include <iostream>


#include <fstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::Misc;

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

SimpleFactory<MediaProcessUnit,DisambiguatedGraphXmlLogger> disambiguatedGraphXmlLoggerFactory("DisambiguatedGraphXmlLogger");

DisambiguatedGraphXmlLogger::DisambiguatedGraphXmlLogger()
: AbstractLinguisticLogger(".output.xml")
{}


DisambiguatedGraphXmlLogger::~DisambiguatedGraphXmlLogger()
{}

void DisambiguatedGraphXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="DictionaryCode"&gt;</b>
   *    -  dictionaryCode : DictionaryCode resource
   */

  AbstractLinguisticLogger::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;
  m_macroManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO"));
  m_microManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO"));

}

LimaStatusCode DisambiguatedGraphXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    PTLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  ofstream out;
  if (!openLogFile(out,metadata->getMetaData("FileName")))
  {
    PTLOGINIT;
    LERROR << "Can't open log file ";
    return CANNOT_OPEN_FILE_ERROR;
  }

  AnalysisGraph* posTokenList=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  const LinguisticGraph* posGraph=posTokenList->getGraph();
  
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  out << "<vertices>" << endl;

  LinguisticGraphInEdgeIt inItr,inItrEnd;
  LinguisticGraphVertexIt vxItr,vxItrEnd;
  boost::tie(vxItr,vxItrEnd) = vertices(*posGraph);
  for (;vxItr!=vxItrEnd;vxItr++)
  {
    MorphoSyntacticData* dw=get(vertex_data,*posGraph,*vxItr);
    Token* ft=get(vertex_token,*posGraph,*vxItr);

    if (ft == 0 || dw == 0)
    {
      continue;
    }

    uint64_t pos=0;
    uint64_t len=0;
    if (ft!=0)
    {
      pos=ft->position();
      len=ft->length();
    }

    LinguisticCode macro = dw->firstValue(m_macroManager->getPropertyAccessor());
    LinguisticCode micro = dw->firstValue(m_microManager->getPropertyAccessor());

    std::string smacro = m_macroManager->getPropertySymbolicValue(macro);
    std::string smicro = m_microManager->getPropertySymbolicValue(micro);

    out << "<vertex id=\"" << *vxItr << "\" position=\"" << pos << "\" length=\"" << len << "\" >" << endl;
    std::set<StringsPoolIndex> lemmas = dw->allLemma();
    std::set<StringsPoolIndex>::const_iterator lemmaIt, lemmaIt_end;
    lemmaIt = lemmas.begin(); lemmaIt_end = lemmas.end();
    for (; lemmaIt != lemmaIt_end; lemmaIt++)
    {
      out << "  <lemma>" << limastring2utf8stdstring(sp[*lemmaIt]) << "</lemma>" << endl;
    }
    out << "  <macro>" << smacro << "</macro>" << endl;
    out << "  <micro>" << smicro << "</micro>" << endl;
    boost::tie(inItr,inItrEnd) = in_edges(*vxItr,*posGraph);
    for (;inItr!=inItrEnd;inItr++)
    {
      out << "  <pred id=\"" << source(*inItr,*posGraph) << "\"/>" << endl;
    }
    out << "</vertex>" << endl;
  }
  out << "</vertices>" << endl;
  out.close();

  TimeUtils::logElapsedTime("DisambiguatedGraphXmlLogger");

  return SUCCESS_ID;
}


}

}

}
