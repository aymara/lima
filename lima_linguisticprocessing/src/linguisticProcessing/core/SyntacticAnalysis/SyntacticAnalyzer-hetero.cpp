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
/**
 * @brief      process unit to extract heterosyntagmatic dependency relations
 *
 * @file       SyntacticAnalyzer-hetero.cpp
 * @author     besancon (besanconr@zoe.cea.fr) 

 *             Copyright (C) 2004 by CEA LIST
 * @date       Tue Oct  5 2004
 * @version    $Id$
 */

#include "SyntacticAnalyzer-hetero.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "SimplificationData.h"

#define SAHLOGINIT LOGINIT("LP::SyntacticAnalysis::Hetero")

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

SimpleFactory<MediaProcessUnit,SyntacticAnalyzerDepsHetero>
syntacticAnalyzerDepsHeteroFactory(SYNTACTICANALYZERDEPSHETERO_CLASSID);

//***********************************************************************
// constructors
//***********************************************************************
SyntacticAnalyzerDepsHetero::SyntacticAnalyzerDepsHetero():
m_language(),
m_recognizer(0),
m_unfold(true),
m_linkSubSentences(true),
m_applySameRuleWhileSuccess(false)
{
}

SyntacticAnalyzerDepsHetero::SyntacticAnalyzerDepsHetero(const SyntacticAnalyzerDepsHetero& s):
MediaProcessUnit(s),
m_language(s.m_language),
m_recognizer(s.m_recognizer),
m_unfold(s.m_unfold),
m_linkSubSentences(s.m_linkSubSentences),
m_applySameRuleWhileSuccess(s.m_applySameRuleWhileSuccess)
{
}

//***********************************************************************
// destructor
//***********************************************************************
SyntacticAnalyzerDepsHetero::~SyntacticAnalyzerDepsHetero()
{
}

//***********************************************************************
// assignment operator
//***********************************************************************
SyntacticAnalyzerDepsHetero& SyntacticAnalyzerDepsHetero::operator=(const SyntacticAnalyzerDepsHetero& s)
{
  if (this != &s) {
    m_language=s.m_language;
    m_recognizer=s.m_recognizer;
  }
  return *this;
}

//***********************************************************************
// initialization
//***********************************************************************
void SyntacticAnalyzerDepsHetero::init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
  
{
  m_language=manager->getInitializationParameters().media;
  try {
    std::string rules=unitConfiguration.getParamsValueAtKey("rules");
    m_recognizer = static_cast<Automaton::Recognizer*>(LinguisticResources::single().getResource(m_language,rules));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SAHLOGINIT;
    LERROR << "no parameter 'rules' in SyntacticAnalyzerDepsHetero group for language "
           << (int) m_language;
  }
  try {
    std::map<std::string,std::string>&  rulesMap=unitConfiguration.getMapAtKey("subSentencesRules");
    std::map<std::string,std::string>::const_iterator itm, itm_end;
    itm = rulesMap.begin(); itm_end = rulesMap.end();
    for (; itm != itm_end; itm++)
    {
      SAHLOGINIT;
      LINFO << "Loading subsentences recognizer " << (*itm).second << " for type " << (*itm).first;
      m_subSentRecognizers.insert(std::make_pair(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId((*itm).first), static_cast<Automaton::Recognizer*>(LinguisticResources::single().getResource(m_language,(*itm).second))));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchMap& )
  {
    SAHLOGINIT;
    LERROR << "no parameters map 'subSentencesRules' in SyntacticAnalyzerDepsHetero group for language "
      << (int) m_language;
  }

  try
  {
    std::string unfold=unitConfiguration.getParamsValueAtKey("unfold");
    if (unfold == "true") m_unfold = true;
    else m_unfold = false;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SAHLOGINIT;
    LINFO << "no parameter 'unfold' in SyntacticAnalyzerDepsHetero group for language "
      << (int) m_language << " ; using default : true";
  }
  if (m_unfold)
  {
    try {
      std::string linkSubSentences=unitConfiguration.getParamsValueAtKey("linkSubSentences");
      if (linkSubSentences == "true") m_linkSubSentences = true;
      else m_linkSubSentences = false;
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      SAHLOGINIT;
      LINFO << "no parameter 'linkSubSentences' in SyntacticAnalyzerDepsHetero group for language "
        << (int) m_language << " ; using default : true";
    }
  }

  try {
    std::string val=unitConfiguration.getParamsValueAtKey("applySameRuleWhileSuccess");
    if (val == "true" || val == "yes") {
      m_applySameRuleWhileSuccess=true;
    }
    else {
      m_applySameRuleWhileSuccess=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value
}

//***********************************************************************
// main process function
//***********************************************************************
LimaStatusCode SyntacticAnalyzerDepsHetero::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SyntacticAnalysis");
  SAHLOGINIT;
  LINFO << "start heterosyntagmatic dependence relations search";

  AnalysisGraph* posgraph=
    static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  LinguisticGraph* graph = posgraph->getGraph();

  SegmentationData* sb=
    static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no sentence bounds ! abort";
    return MISSING_DATA;
  }
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds have been computed on " << sb->getGraphId() << " !";
    LERROR << "SyntacticAnalyzer-deps needs SentenceBounds on PosGraph";
    return INVALID_CONFIGURATION;
  }

  SimplificationData* simplificationData =
    static_cast<SimplificationData*>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    simplificationData=new SimplificationData(posgraph);
    analysis.setData("SimplificationData",simplificationData);
  }


  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
       boundItr!=(sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    LDEBUG << "analyze sentence from vertex " << beginSentence
           << " to vertex " << endSentence;

    std::vector<Automaton::RecognizerMatch> result;
    m_recognizer->apply(*posgraph,
                        beginSentence,
                        endSentence,
                        analysis,
                        result,
                        true,  // test all vertices=true
                        false, // stop at first success=false
                        false,  // only one success per type=true
                        false, // return at first success=false
                        m_applySameRuleWhileSuccess // depends on config file
                        );

    /** Handles subSentences found during simplification
      * For each subsentence (in reverse order of their finding)
      *   - hetero analysis on the subsentence alone ;
      *   - put back the subsentence in the graph ;
      *   - hetero analysis initialized on the subsentence bounds but
      *     allowed to search up to the full sentence bounds in order
      *     to link the subsentence to the full sentence
      */
    std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > >::reverse_iterator subSentRIt, subSentRIt_end;
    subSentRIt = simplificationData->simplifications(beginSentence).rbegin();
    subSentRIt_end = simplificationData->simplifications(beginSentence).rend();
    for (; subSentRIt != subSentRIt_end; subSentRIt++)
    {
      LinguisticGraphEdge first;
      LinguisticGraphEdge last;
      Common::MediaticData::SyntacticRelationId subSentType;
      boost::tie(first, last, subSentType) = *subSentRIt;
      std::vector<Automaton::RecognizerMatch> subSentResult;
      LDEBUG << "Hetero rules on a subsentence: " << boost::target(first,*graph) << " / " << boost::source(last,*graph);

      m_recognizer->apply(*posgraph,
                          boost::target(first,*graph),
                          boost::source(last,*graph),
                          analysis,
                          subSentResult,
                          true,  // test all vertices=true
                          false, // stop at first success=false
                          true,  // only one success per type=true
                          false, // return at first success=false
                          false  //applySameRuleWhileSuccess=false
                          );
      if (m_unfold)
      {
        LDEBUG << "Unfolding";
        LDEBUG << "Remove edge " << boost::source(first,*graph) << " -> " << boost::target(last,*graph);
        boost::remove_edge(boost::source(first,*graph),
                          boost::target(last,*graph),
                          *graph);
        if (!boost::edge(boost::source(first,*graph),
                        boost::target(first,*graph),
                        *graph).second)
        {
          LDEBUG << "Add edge " << boost::source(first,*graph) << " -> " << boost::target(first,*graph);
          boost::add_edge(boost::source(first,*graph),
                      boost::target(first,*graph),
                      *graph);
        }
        if (!boost::edge(boost::source(last,*graph),
                        boost::target(last,*graph),
                        *graph).second)
        {
          LDEBUG << "Add edge " << boost::source(last,*graph) << " -> " << boost::target(last,*graph);
          boost::add_edge(boost::source(last,*graph),
                      boost::target(last,*graph),
                      *graph);
        }
        if (m_linkSubSentences)
        {
          if (m_subSentRecognizers.find(subSentType) == m_subSentRecognizers.end())
          {
            LWARN << "No recognizer for subsentence type " << subSentType;
          }
          else
          {
            LDEBUG << "Link subsentence to main one " << boost::target(first,*graph) << "/"
                << boost::source(last,*graph) << "/" << beginSentence << "/"
                << endSentence;
            Automaton::Recognizer* subSentRecognizer = (*(m_subSentRecognizers.find(subSentType))).second;
            subSentRecognizer->apply(*posgraph,
                                     boost::target(first,*graph),
                                     boost::source(last,*graph),
                                     beginSentence,
                                     endSentence,
                                     analysis,
                                     subSentResult,
                                     true, // test all vertices=true
                                     true, // stop at first success=false
                                     true, // only one success per type=true
                                     true, // return at first success=true
                                     false //applySameRuleWhileSuccess=false
                                     );
          }
        }
      }
    }
    /// Recopy dependencies found on defining coord strings (coming from outside or pointing out of
    /// the string) on defined ones
    recopyCoordinationStrings(analysis);
  }
  return SUCCESS_ID;
}

LimaStatusCode SyntacticAnalyzerDepsHetero::recopyCoordinationStrings(AnalysisContent& analysis) const
{
  SAHLOGINIT;
  LDEBUG << "SyntacticAnalyzerDepsHetero::recopyCoordinationStrings";

  // Initialisation
  SyntacticData* syntacticData = static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    LERROR << "no syntacticData ! abort";
    return MISSING_DATA;
  }
  
  SimplificationData* simplificationData = static_cast<SimplificationData*>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    LERROR << "no simplificationData ! abort";
    return MISSING_DATA;
  }

  DependencyGraph *depGraph = syntacticData-> dependencyGraph();
  EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *depGraph);
  
  // for each string definition d
  for (std::map< std::string, std::vector< LinguisticGraphVertex > >::const_iterator dit = simplificationData->coordStringsDefs().begin(); dit != simplificationData->coordStringsDefs().end(); dit++)
  {
    if (simplificationData->coordStrings().find((*dit).first) == simplificationData->coordStrings().end())
    {
      // no same string for definition
      LDEBUG << "SyntacticAnalyzerDepsHetero::recopyCoordinationStrings no same string for definition " << (*dit).first;
      continue;
    }
    const std::vector< LinguisticGraphVertex >& stringDef = (*dit).second;
    const std::vector< std::vector< LinguisticGraphVertex > >& sameStrings = (*simplificationData->coordStrings().find((*dit).first)).second;
    LDEBUG << "SyntacticAnalyzerDepsHetero::recopyCoordinationStrings there is " << sameStrings.size() << " same strings for " << (*dit).first;
    // for each same string s of d
    for (std::vector< std::vector< LinguisticGraphVertex > >::const_iterator ssit = sameStrings.begin(); ssit != sameStrings.end(); ssit++)
    {
      const std::vector< LinguisticGraphVertex >& sameString = *ssit;
      std::vector< LinguisticGraphVertex >::const_iterator stringDefIt = stringDef.begin();
      std::vector< LinguisticGraphVertex >::const_iterator sameStringIt = sameString.begin();
      // for each twin vertices vs and vd in s and d
      for (; stringDefIt != stringDef.end(); stringDefIt++, sameStringIt++)
      {
        LinguisticGraphVertex stringDefVx = *stringDefIt;
        LinguisticGraphVertex sameStringVx = *sameStringIt;
        DependencyGraphVertex depStringDefVx = syntacticData->depVertexForTokenVertex(stringDefVx);
        DependencyGraphVertex depSameStringVx = syntacticData->depVertexForTokenVertex(sameStringVx);

        // if vd has outgoing edges from outside of d that vs doest not have (same source and
        // same type), create this edge for vs
        DependencyGraphOutEdgeIt depIt, depIt_end;
        boost::tie(depIt, depIt_end) = out_edges(depStringDefVx, *depGraph); 
        for (; depIt != depIt_end; depIt++)
        {
          DependencyGraphVertex depTargV = target(*depIt, *depGraph);
          // target must be outside current coord definition string
          if (std::find(stringDef.begin(), stringDef.end(), depTargV) != stringDef.end()) continue;
          std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relTypeMap[*depIt]);
          std::pair< LinguisticGraphEdge, bool > sameStringEdge = boost::edge(depSameStringVx, depTargV, *depGraph);
          if (!sameStringEdge.second)
          {
            LDEBUG << "SyntacticAnalyzerDepsHetero::recopyCoordinationStrings create edge " << depSameStringVx << " -("<<relationName<<")-> " << depTargV;
            std::pair<DependencyGraphEdge,bool> newSameStringEdge = boost::add_edge(depSameStringVx, depTargV, *depGraph);
            if (newSameStringEdge.second)
            {
              relTypeMap[newSameStringEdge.first] = relTypeMap[*depIt];
            }
          }
        }
        // same thing for incoming edges
        DependencyGraphInEdgeIt depInIt, depInIt_end;
        boost::tie(depInIt, depInIt_end) = in_edges(depStringDefVx, *depGraph);
        for (; depInIt != depInIt_end; depInIt++)
        {
          DependencyGraphVertex depSrcV = source(*depInIt, *depGraph);
          // target must be outside current coord definition string
          if (std::find(stringDef.begin(), stringDef.end(), depSrcV) != stringDef.end()) continue;
          std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relTypeMap[*depIt]);
          std::pair< LinguisticGraphEdge, bool > sameStringEdge = boost::edge(depSameStringVx, depSrcV, *depGraph);
          if (!sameStringEdge.second)
          {
            std::pair<DependencyGraphEdge,bool> newSameStringEdge = boost::add_edge(depSameStringVx, depSrcV, *depGraph);
            if (newSameStringEdge.second)
            {
              LDEBUG << "SyntacticAnalyzerDepsHetero::recopyCoordinationStrings create edge " << depSameStringVx << " <-("<<relationName<<")- " << depSrcV;
              relTypeMap[newSameStringEdge.first] = relTypeMap[*depInIt];
            }
          }
        }
      }
    }
  }
  return SUCCESS_ID;
}


} // end namespace
} // end namespace
} // end namespace
