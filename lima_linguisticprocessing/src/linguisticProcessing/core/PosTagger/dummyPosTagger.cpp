// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "dummyPosTagger.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

#include <iostream>
#include <iterator>
#include <set>
#include <map>
#include <algorithm>

using namespace boost;
using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

SimpleFactory<MediaProcessUnit,DummyPosTagger> dummyPosTaggerFactory(DUMMYPOSTAGGER_CLASSID);

void DummyPosTagger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure&,
  Manager* manager)

{
  m_language=manager->getInitializationParameters().media;
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
}

LimaStatusCode DummyPosTagger::process(
  AnalysisContent& analysis) const
{

  TimeUtils::updateCurrentTime();

  // start postagging here !
  PTLOGINIT;
  LINFO << "Start of Dummy posTagging";

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));

  AnalysisGraph* posgraph=new AnalysisGraph("PosGraph",m_language,false,true,*anagraph);
  analysis.setData("PosGraph",posgraph);
  LinguisticGraph* graph=posgraph->getGraph();

  const LanguageData& ld=static_cast<const Common::MediaticData::LanguageData&>(MediaticData::single().mediaData(m_language));

  // process all vertices with fulltoken
  LinguisticGraphVertexIt it,itEnd;
  boost::tie(it,itEnd) = vertices(*graph);
  for (;it != itEnd; it++)
  {
    MorphoSyntacticData* data = get(vertex_data,*graph,*it);
    if (data != 0)
    {
      MorphoSyntacticData* posdata=new MorphoSyntacticData(*data);
      put(vertex_data,*graph,*it,posdata);
      // select the first empty micro if one, else put the first non empty micro.
      LinguisticCode micro;
      if (posdata->size()>0)
      {
        for (MorphoSyntacticData::const_iterator dataItr=posdata->begin();
             dataItr!=posdata->end();
             dataItr++)
        {
          LinguisticCode mic(m_microAccessor->readValue(dataItr->properties));
          if (dataItr==posdata->begin()) {
            micro=mic;
          }
          if (ld.isAnEmptyMicroCategory(mic))
          {
            // there is an empty micro, so choose it.
            micro=mic;
            break;
          }
        }
        CheckDifferentPropertyPredicate cdpp(*m_microAccessor,micro);
        posdata->erase(remove_if(posdata->begin(),posdata->end(),cdpp),posdata->end());
      }
      else
      {
        Token* tok= get(vertex_token,*graph,*it);
        LWARN << "The MorphoSyntacticData "
          << Common::Misc::limastring2utf8stdstring(tok->stringForm()) << " is empty !";
      }
    }
  }

  // Affichage du graphe après le POSTagging
  LDEBUG << "Graph after Dummy posTagging:";

  LINFO << "End of Dummy posTagging";

  TimeUtils::logElapsedTime("DummyPosTagger");
  return SUCCESS_ID;

}


} // PosTagger
} // LinguisticProcessing
} // Lima
