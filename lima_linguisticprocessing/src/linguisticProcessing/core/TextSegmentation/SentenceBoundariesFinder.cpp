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
#include "SentenceBoundariesFinder.h"
#include "SegmentationData.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/timeUtilsController.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

SimpleFactory<MediaProcessUnit,SentenceBoundariesFinder> sentenceBoundariesFinderFactory(SENTENCEBOUNDARIESFINDER_CLASSID);

SentenceBoundariesFinder::SentenceBoundariesFinder() :
MediaProcessUnit(),
m_microAccessor(0),
m_graph(),
m_boundaryValues(),
m_boundaryMicros()
{}


SentenceBoundariesFinder::~SentenceBoundariesFinder()
{}

void SentenceBoundariesFinder::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="SentenceBoundariesFinder"&gt;</b>
   */  
  SENTBOUNDLOGINIT;
  
  MediaId language=manager->getInitializationParameters().media;
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    m_graph=string("PosGraph");
  }

 try
  {
    m_data=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    m_data=string("SentenceBoundaries");
  }

  try
  {
    deque<string> boundariesRestrictions=unitConfiguration.getListsValueAtKey("values");
    for (deque<string>::const_iterator it=boundariesRestrictions.begin(),it_end=boundariesRestrictions.end();
    it!=it_end; it++) 
    {
#ifdef DEBUG_LP
      LDEBUG << "init(): add filter for value " << *it;
#endif
      m_boundaryValues.insert(Common::Misc::utf8stdstring2limastring(*it));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) {} // optional
  
  try
  {
    const Common::PropertyCode::PropertyManager& microManager=
    static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO");
    deque<string> boundariesRestrictions=unitConfiguration.getListsValueAtKey("micros");
    for (deque<string>::const_iterator it=boundariesRestrictions.begin(),it_end=boundariesRestrictions.end();
    it!=it_end; it++) 
    {
      LinguisticCode micro=microManager.getPropertyValue(*it);
      if (micro == 0) {
        LERROR << "init(): cannot find linguistic code for micro " << *it;
      }
      else {
#ifdef DEBUG_LP
        LDEBUG << "init(): add filter for micro " << micro;
#endif
        m_boundaryMicros.push_back(micro);
      }
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) {
    LERROR << "Warning: No boundaries categories defined for language " << language;
    //throw InvalidConfiguration();
  } 
}


LimaStatusCode SentenceBoundariesFinder::process(
  AnalysisContent& analysis) const
{
  
  Lima::TimeUtilsController timer("SentenceBoundariesFinder");
  
  SENTBOUNDLOGINIT;
  LINFO << "start finding sentence bounds";
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (anagraph==0) {
    LERROR << "no graph '" << m_graph << "' available !";
    return MISSING_DATA;
  }

  LinguisticGraphVertex lastVx=anagraph->lastVertex();
  LinguisticGraphVertex beginSentence=anagraph->firstVertex();
#ifdef DEBUG_LP
  LDEBUG << "found beginSentence at " << beginSentence;
#endif

  SegmentationData* sb=new SegmentationData(m_graph);
  analysis.setData(m_data,sb);
  
  if (m_boundaryValues.empty()) {
    while (beginSentence!=lastVx)
    {
      LinguisticGraphVertex endSentence=anagraph->nextMainPathVertex(beginSentence,*m_microAccessor,m_boundaryMicros,lastVx);
#ifdef DEBUG_LP
      LDEBUG << "found endSentence at " << endSentence;
#endif
      sb->add(Segment("sentence",beginSentence,endSentence,anagraph));
      beginSentence=endSentence;
    }
  }
  else { // apply restriction on values for sentence boundaries
    // cannot set endSentence from beginSentence inside the loop, because we have to continue 
    // moving forward even if there is no match (with restricted values)
    LinguisticGraphVertex endSentence=anagraph->nextMainPathVertex(beginSentence,*m_microAccessor,m_boundaryMicros,lastVx);
    while (endSentence!=lastVx)
    {
      Token* t=get(vertex_token,*(anagraph->getGraph()),endSentence);
#ifdef DEBUG_LP
      if (t!=0) {
        LDEBUG << "found endSentence at " << endSentence  << "(" 
               << Common::Misc::limastring2utf8stdstring(t->stringForm()) << ")";
      }
      else {
        LDEBUG << "found endSentence at " << endSentence;
      }
#endif
      if (t==0 || m_boundaryValues.find(t->stringForm())!=m_boundaryValues.end()) {
        sb->add(Segment("sentence",beginSentence,endSentence,anagraph));
        beginSentence=endSentence;
      }
      else {
#ifdef DEBUG_LP
        LDEBUG << " -> not kept (not in restricted values)";
#endif
      }
      endSentence=anagraph->nextMainPathVertex(endSentence,*m_microAccessor,m_boundaryMicros,lastVx);
    }
    // add last sentence (the one ending with lastVx)
    sb->add(Segment("sentence",beginSentence,endSentence,anagraph));
  }
  return SUCCESS_ID;
}


}

}

}
