// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SyntacticAnalyzer-simplify.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on Mar, 15 2005
  *
  */

#include "SyntacticAnalyzer-simplify.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/ChainIdStruct.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "SimplificationData.h"
#include "common/time/timeUtilsController.h"

#include <boost/graph/reverse_graph.hpp>

using namespace std;
using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{
#define SASLOGINIT  LOGINIT("LP::SyntacticAnalysis::Simplify")

SimpleFactory<MediaProcessUnit,SyntacticAnalyzerSimplify> syntacticAnalyzerSimplifyFactory(SYNTACTICANALYZERSIMPLIFY_CLASSID);


SyntacticAnalyzerSimplify::SyntacticAnalyzerSimplify() :
    m_language(),
    m_recognizer(0)
{}

void SyntacticAnalyzerSimplify::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SASLOGINIT;
  LINFO << "SyntacticAnalyzerSimplify::init";
  m_language=manager->getInitializationParameters().media;
  std::string rules=unitConfiguration.getParamsValueAtKey("simplifyAutomaton");
  m_recognizer = static_cast<Automaton::Recognizer*>(LinguisticResources::single().getResource(m_language,rules));
}

LimaStatusCode SyntacticAnalyzerSimplify::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SyntacticAnalysis");
  SASLOGINIT;
  LINFO << "SyntacticAnalyzerSimplify::process";

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"));
  if (anagraph==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  auto sb = std::dynamic_pointer_cast<SegmentationData>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no sentence bounds ! abort";
    return MISSING_DATA;
  }

  if (analysis.getData("SyntacticData")==0)
  {
    auto syntacticData = std::make_shared<SyntacticData>(anagraph.get(), nullptr);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData", syntacticData);
  }
  
  auto simplificationData = std::dynamic_pointer_cast<SimplificationData>(analysis.getData("SimplificationData"));
  if (simplificationData==0)
  {
    simplificationData = std::make_shared<SimplificationData>(anagraph.get());
    analysis.setData("SimplificationData", simplificationData);
  }
  
  // ??OME2 SegmentationData::const_iterator boundItr, boundItr_end;
  //boundItr = sb->begin(); boundItr_end = sb->end();
  std::vector<Segment>::const_iterator boundItr, boundItr_end;
  boundItr = (sb->getSegments()).begin(); boundItr_end = (sb->getSegments()).end();
  for (; boundItr != boundItr_end; boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    LDEBUG << "simplify sentence from vertex " << beginSentence 
           << " to vertex " << endSentence;

    do 
    {
      LDEBUG << "Applying automaton on sentence from " << beginSentence << " to " << endSentence;
      simplificationData->simplificationDone(false);
      simplificationData->sentence(beginSentence);
      std::vector<Automaton::RecognizerMatch> result;
      m_recognizer->apply(*anagraph,
                          beginSentence, 
                          endSentence,
                          analysis,
                          result,
                          true, // test all vertices=true
                          true, // stop rules search on a node at first success
                          true, // only one success per type
                          true  // stop exploration at first success 
                         ); 
    } while (simplificationData->simplificationDone());

  }

  LINFO << "end syntactic analysis - subsentences simplification";
  return SUCCESS_ID;
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
