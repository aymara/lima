// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
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
  m_forbidBoundaryValues(),
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
    deque<string> exclBoundariesRestrictions=unitConfiguration.getListsValueAtKey("forbid-values");
    for (deque<string>::const_iterator it=exclBoundariesRestrictions.begin(),it_end=exclBoundariesRestrictions.end();
    it!=it_end; it++)
    {
#ifdef DEBUG_LP
      LDEBUG << "init(): add filter for forbidden value " << *it;
#endif
      m_forbidBoundaryValues.insert(Common::Misc::utf8stdstring2limastring(*it));
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
      if (micro == L_NONE)
      {
        LERROR << "init(): cannot find linguistic code for micro " << *it;
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "init(): add filter for micro " << micro;
#endif
        m_boundaryMicros.push_back(micro);
      }
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
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
  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData(m_graph));
  if (anagraph==0)
  {
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

  if (m_boundaryValues.empty() && m_forbidBoundaryValues.empty())
  {
    while (beginSentence!=lastVx)
    {
      LinguisticGraphVertex endSentence=anagraph->nextMainPathVertex(beginSentence,*m_microAccessor,m_boundaryMicros,lastVx);
      if (endSentence == lastVx)
      {
        set<LinguisticGraphVertex> prevVx = getPrecedingNodes<set<LinguisticGraphVertex>>(*anagraph, lastVx);
        /*if (prevVx.size() != 1)
        {
          throw LimaException("Many paths lead to the last vertex of the text");
        }
        endSentence = *prevVx.begin();
        */
        if (beginSentence != endSentence && prevVx.end() == prevVx.find(beginSentence))
        {
          sb->add(Segment("sentence",beginSentence,endSentence,anagraph.get()));
        }
        break;
      }
#ifdef DEBUG_LP
      LDEBUG << "found endSentence at " << endSentence;
#endif
      sb->add(Segment("sentence",beginSentence,endSentence,anagraph.get()));
      beginSentence=endSentence;
    }
  }
  else
  {
    // Apply restriction on values for sentence boundaries
    // cannot set endSentence from beginSentence inside the loop, because we have to continue
    // moving forward even if there is no match (with restricted values)
    LinguisticGraphVertex endSentence=anagraph->nextMainPathVertex(beginSentence,*m_microAccessor,m_boundaryMicros,lastVx);
    while (endSentence!=lastVx)
    {
      Token* t=get(vertex_token,*(anagraph->getGraph()),endSentence);
#ifdef DEBUG_LP
      if (t!=0)
      {
          LDEBUG << "found endSentence at " << endSentence  << "("
               << Common::Misc::limastring2utf8stdstring(t->stringForm()) << ")";
      }
      else
      {
          LDEBUG << "found endSentence at " << endSentence;
      }
#endif

      if (t==0 || (!m_forbidBoundaryValues.empty() && m_forbidBoundaryValues.find(t->stringForm())!=m_forbidBoundaryValues.end()) )
      {
#ifdef DEBUG_LP
          LDEBUG << " -> not kept (bcz in forbidden values)";
#endif
      }
      else if (t==0 || (!m_boundaryValues.empty() && m_boundaryValues.find(t->stringForm())==m_boundaryValues.end()) )
      {
#ifdef DEBUG_LP
          LDEBUG << " -> not kept (bcz not in restricted values)";
#endif
      }
      else
      {
#ifdef DEBUG_LP
          LDEBUG << "add sentence " << beginSentence << "-" << endSentence;
#endif
          sb->add(Segment("sentence",beginSentence,endSentence,anagraph.get()));
          beginSentence=endSentence;
      }
      endSentence=anagraph->nextMainPathVertex(endSentence,*m_microAccessor,m_boundaryMicros,lastVx);
    }
    // add last sentence (the one ending with lastVx)
    if(beginSentence<endSentence) {
#ifdef DEBUG_LP
        LDEBUG << "add last sentence " << beginSentence << "-" << endSentence;
#endif
      sb->add(Segment("sentence",beginSentence,endSentence,anagraph.get()));
    }
  }
  return SUCCESS_ID;
}


}

}

}
