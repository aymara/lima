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

#include "SpecificEntitiesRecognizer.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/misc/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/SentenceBounds.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntities
{

SimpleFactory<LinguisticProcessUnit,SpecificEntitiesRecognizer> specificEntitiesRecognizer(SPECIFICENTITIESRECOGNIZER_CLASSID);

SpecificEntitiesRecognizer::SpecificEntitiesRecognizer():
LinguisticProcessUnit(),
m_recognizer(0),
m_useSentenceBounds(true),
m_useDicoWords(false)
{}


SpecificEntitiesRecognizer::~SpecificEntitiesRecognizer()
{}

void SpecificEntitiesRecognizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SELOGINIT;
  MediaId language=manager->getInitializationParameters().language;
  try
  {
    string automaton=unitConfiguration.getParamsValueAtKey("automaton");
    AbstractResource* res=LinguisticResources::single().getResource(language,automaton);
    m_recognizer=static_cast<Automaton::Recognizer*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No param 'automaton' in SpecificEntitiesRecognizer group for language "
           << (int)language << " !" << LENDL;
    throw InvalidConfiguration();
  }

  try {
    string useDicoWords=unitConfiguration.getParamsValueAtKey("useDicoWords");
    if (useDicoWords=="yes" ||
        useDicoWords=="true" ||
        useDicoWords=="1") {
      m_useDicoWords=true;
    }
    else {
      m_useDicoWords=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try
  {
    string useSentenceBounds=unitConfiguration.getParamsValueAtKey("useSentenceBounds");
    if (useSentenceBounds=="yes" ||
        useSentenceBounds=="true" ||
        useSentenceBounds=="1") {
      m_useSentenceBounds=true;
    }
    else {
      m_useSentenceBounds=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional parameter: keep default value
  }
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No 'graph' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using PosGraph" << LENDL;
    m_graph=string("PosGraph");
  }

}

LimaStatusCode SpecificEntitiesRecognizer::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  SELOGINIT;
  LINFO << "start process" << LENDL;

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    if (static_cast<AnalysisGraph*>(analysis.getData("PosGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("PosGraph"))->populateAnnotationGraph(annotationData, "PosGraph");
    }

    analysis.setData("AnnotationData",annotationData);
  }
  if (m_annotationData->dumpFunction("SpecificEntity") == 0)
  {
    m_annotationData->dumpFunction("SpecificEntity", new DumpSpecificEntityAnnotation());
  }
//   if (analysis.getData("SyntacticData")==0)
//   {
//     SyntacticAnalysis::SyntacticData* syntacticData=new SyntacticAnalysis::SyntacticData(static_cast<AnalysisGraph*>(analysis.getData(m_graph)),0);
//     syntacticData->setupDependencyGraph();
//     analysis.setData("SyntacticData",syntacticData);
//   }

  AnalysisGraphId* gid(new AnalysisGraphId(m_graph));
  analysis.setData("GraphId",gid);

//   SpecificEntityFound* seFound(new SpecificEntityFound(false));
//   analysis.setData("SEFound",seFound);

  LimaStatusCode returnCode;

  // process text
//   if (m_useSentenceBounds) {
//     returnCode=processOnEachSentence(analysis);
//   }
//   else {
//     returnCode=processOnWholeText(analysis);
//   }

  // initialize the vertices to clear
  std::set<LinguisticGraphVertex> verticesToRemove;

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));

  LinguisticGraph* graph=anagraph->getGraph();
  std::queue<LinguisticGraphVertex> toVisit;
  VertexTokenPropertyMap tokenMap=get(vertex_token,*graph);
  VertexDataPropertyMap DataMap=get(vertex_data,*graph);
  std::set<LinguisticGraphVertex> visited;

  try
  {
    toVisit.push(anagraph->firstVertex());
    visited.insert(anagraph->firstVertex());

    while (!toVisit.empty())
    {
      std::set<LinguisticGraphVertex> addedVertices;
      LinguisticGraphVertex currentVertex=toVisit.front();
      toVisit.pop();

      Token* currentToken=tokenMap[currentVertex];
      if (currentToken)
      {
        // process current Token
        bool specificEntityFound(false);
        RecognizerMatch se(anagraph);
        specificEntityFound= findSEFromRecognizer(currentVertex,
            anagraph,
            analysis,
            se);

//         if (specificEntityFound) {
//           LDEBUG << "found specific entity for vertex "
//               << currentVertex << "("
//               << currentToken->stringForm() << ")" << LENDL;
/*          updateGraph(anagraph->getGraph(),se,verticesToRemove,
                      addedVertices,annotationData);*/
//         }
//         else {
//           LDEBUG << "no specific entity found for vertex "
//               << currentVertex << "("
//               << currentToken->stringForm() << ")" << LENDL;
//         }
      }
      std::set<LinguisticGraphVertex>::const_iterator addedIt, addedIt_end;
      addedIt = addedVertices.begin(); addedIt_end = addedVertices.end();
      for (; addedIt != addedIt_end; addedIt++)
      {
        if (visited.find(*addedIt) == visited.end())
        {
          toVisit.push(*addedIt);
          visited.insert(*addedIt);
        }
      }

      // go one step forward on the new path
      LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
      boost::tie(adjItr,adjItrEnd) = adjacent_vertices(currentVertex,*graph);
      for (;adjItr!=adjItrEnd;adjItr++)
      {
        if (addedVertices.find(*adjItr)==addedVertices.end() &&
            visited.find(*adjItr)==visited.end())
        {
          toVisit.push(*adjItr);
          visited.insert(*adjItr);
        }
      }
    }

    // at end, remove vertices
//     removeVertices(,*graph);
  }
  catch (std::exception &exc)
  {
    SELOGINIT;
    LWARN << "Exception in Specific Entities : " << exc.what() << LENDL;
//     throw;
    return UNKNOWN_ERROR;
  }

  return SUCCESS_ID;

  TimeUtils::logElapsedTime("SpecificEntitiesRecognizer");
  return returnCode;
}

LimaStatusCode SpecificEntitiesRecognizer::
processOnEachSentence(AnalysisContent& analysis) const
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesRecognizer::processOnEachSentence" << LENDL;

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (anagraph==0) {
    SELOGINIT;
    LERROR << "SpecificEntitiesRecognizer::processOnEachSentence: no graph '" << m_graph << "' available !!" << LENDL;
    return MISSING_DATA;
  }

  // get sentence bounds
  SentenceBounds* sb=static_cast<SentenceBounds*>(analysis.getData("SentenceBounds"));
  if (sb==0)
  {
    SELOGINIT;
    LERROR << "SpecificEntitiesRecognizer::processOnEachSentence: no sentence bounds defined ! abort" << LENDL;
    return MISSING_DATA;
  }
  if (sb->graphId() != m_graph) {
    SELOGINIT;
    LERROR << "SpecificEntitiesRecognizer::processOnEachSentence: SentenceBounds are computed on graph '" << sb->graphId() << "'" << LENDL;
    LERROR << "can't compute specificEntities on graph '" << m_graph << "' !" << LENDL;
    return INVALID_CONFIGURATION;
  }

  // resize data to the number of sentences
  std::vector< Automaton::RecognizerMatch > seRecognizerResult;
  LinguisticGraphVertex beginSentence=sb->getStartVertex();
  SentenceBounds::const_iterator boundItr=sb->begin();
  while (boundItr!=sb->end())
  {
    LinguisticGraphVertex endSentence=*boundItr;
//     LDEBUG << "analyze sentence from vertex " << beginSentence << " to vertex " << endSentence << LENDL;

    seRecognizerResult.clear();
    m_recognizer->apply(
      *anagraph,
      beginSentence,
      endSentence,
      analysis,
      seRecognizerResult
/*                     bool testAllVertices=false,
                     bool stopAtFirstSuccess=true,
                     bool onlyOneSuccessPerType=false,
                     bool returnAtFirstSuccess=false,
                     bool applySameRuleWhileSuccess=false */
    );

    beginSentence=endSentence;
    boundItr++;
  }

  return SUCCESS_ID;
}

LimaStatusCode SpecificEntitiesRecognizer::
processOnWholeText(AnalysisContent& analysis) const
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesRecognizer::processOnWholeText" << LENDL;

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (anagraph == 0) {
    SELOGINIT;
    LERROR << "no graph '" << m_graph << "' available !!" << LENDL;
    return MISSING_DATA;
  }

  // resize data to 1 : whole text taken as one
  std::vector< Automaton::RecognizerMatch > seRecognizerResult;

  m_recognizer->apply(*anagraph,
                      anagraph->firstVertex(),
                      anagraph->lastVertex(),
                      analysis,
                      seRecognizerResult

//                      bool testAllVertices=false,
//                      bool stopAtFirstSuccess=true,
//                      bool onlyOneSuccessPerType=false,
//                      bool returnAtFirstSuccess=false,
//                      bool applySameRuleWhileSuccess=false) const;

  );

//   SpecificEntityFound* seFound = static_cast<SpecificEntityFound*>(analysis.getData("SEFound"));
//   do
//   {
//     LDEBUG << "Applying automaton" << LENDL;
//     seFound->setFound(false);
//     m_recognizer->apply(*anagraph,
//                          anagraph->firstVertex(),
//                          anagraph->lastVertex(),
//                          analysis,
//                          seRecognizerResult,
//                          false, // test all vertices=true
//                          true, // stop rules search on a node at first success
//                          true, // only one success per type
//                          true  // stop exploration at first success
//                        );
//   } while (seFound->getFound());

  return SUCCESS_ID;
}

bool SpecificEntitiesRecognizer::findSEFromRecognizer(
    LinguisticGraphVertex& currentVertex,
    AnalysisGraph* anagraph,
    AnalysisContent& analysis,
    RecognizerMatch& se) const
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesRecognizer::findSEFromRecognizer " << currentVertex << LENDL;

  // search in recognizer if vertex matches one trigger
  std::vector<RecognizerMatch> result;

  if (m_recognizer->testOnVertex(
      *anagraph,
      currentVertex,
      anagraph->firstVertex(),
      anagraph->lastVertex(),
      analysis,result
/*                          bool stopAtFirstSuccess=true,
                            bool onlyOneSuccessPerType=false,
                            bool applySameRuleWhileSuccess=false */
    ))
  {
    se=result.front();
    return true;
  }
  return false;
}


} // end namespace
} // end namespace
} // end namespace
