// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       applyRecognizer.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jan 14 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2020 by CEA LIST
 *
 ***********************************************************************/

#include "applyRecognizer.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/timeUtilsController.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

SimpleFactory<MediaProcessUnit,ApplyRecognizer> ApplyRecognizer(APPLYRECOGNIZER_CLASSID);

ApplyRecognizer::ApplyRecognizer():
MediaProcessUnit(),
m_recognizers(0),
m_useSentenceBounds(false),
m_sentenceBoundsData("SentenceBoundaries"),
m_updateGraph(false),
m_resolveOverlappingEntities(false),
m_overlappingEntitiesStrategy(IGNORE_SMALLEST),
m_testAllVertices(false),
m_stopAtFirstSuccess(true),
m_onlyOneSuccessPerType(false),
m_graphId("PosGraph"),
m_dataForStorage()
{
}

ApplyRecognizer::~ApplyRecognizer()
{
}

void ApplyRecognizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  APPRLOGINIT;
  MediaId language=manager->getInitializationParameters().media;
  try {
    // try to get a single automaton
    string automaton=unitConfiguration.getParamsValueAtKey("automaton");
    auto res = LinguisticResources::single().getResource(language,automaton);
    m_recognizers.push_back(std::dynamic_pointer_cast<Recognizer>(res));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    try {
    // try to get a list of automatons
      const deque<string>& automatonList=unitConfiguration.getListsValueAtKey("automatonList");
      for (auto automaton: automatonList)
      {
        auto res = LinguisticResources::single().getResource(language, automaton);
        m_recognizers.push_back(std::dynamic_pointer_cast<Recognizer>(res));
      }
    }
    catch (Common::XMLConfigurationFiles::NoSuchList& ) {
      LERROR << "No 'automaton' or 'automatonList' in ApplyRecognizer group for language "
             << (int)language << " !";
      throw InvalidConfiguration();
    }
  }

  try {
    m_useSentenceBounds=
      getBooleanParameter(unitConfiguration,"useSentenceBounds");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try
  {
    string sentenceBoundsData=unitConfiguration.getParamsValueAtKey("sentenceBoundsData");
    if (! sentenceBoundsData.empty()) {
      m_sentenceBoundsData=sentenceBoundsData;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional parameter: keep default value
  }

  try {
    m_updateGraph=
      getBooleanParameter(unitConfiguration,"updateGraph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_resolveOverlappingEntities=
      getBooleanParameter(unitConfiguration,"resolveOverlappingEntities");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    string overlappingEntitiesStrategy=
      unitConfiguration.getParamsValueAtKey("overlappingEntitiesStrategy");
    if (overlappingEntitiesStrategy=="IgnoreSmallest") {
      m_overlappingEntitiesStrategy=IGNORE_SMALLEST;
    }
    else if (overlappingEntitiesStrategy=="IgnoreFirst") {
      m_overlappingEntitiesStrategy=IGNORE_FIRST;
    }
    else if (overlappingEntitiesStrategy=="IgnoreSecond") {
      m_overlappingEntitiesStrategy=IGNORE_SECOND;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_testAllVertices=
      getBooleanParameter(unitConfiguration,"testAllVertices");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_stopAtFirstSuccess=
      getBooleanParameter(unitConfiguration,"stopAtFirstSuccess");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }


  try {
    m_onlyOneSuccessPerType=
      getBooleanParameter(unitConfiguration,"onlyOneSuccessPerType");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("applyOnGraph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional parameter: keep default value
  }

  try {
    m_dataForStorage=unitConfiguration.getParamsValueAtKey("storeInData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

}

bool ApplyRecognizer::
getBooleanParameter(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                    const std::string& param) const {
  string value=unitConfiguration.getParamsValueAtKey(param);
  if (value == "yes" ||
      value == "true" ||
      value == "1") {
    return true;
  }
  return false;
}

LimaStatusCode ApplyRecognizer::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("ApplyRecognizer");
  if (m_recognizers.empty()) {
    APPRLOGINIT;
    LDEBUG << "ApplyRecognizer: No recognizer to apply";
    return MISSING_DATA;
  }
#ifdef DEBUG_LP
  APPRLOGINIT;
  LINFO << "start process";
  LDEBUG << "  parameters are:";
  LDEBUG << "    - useSentenceBounds           :" << m_useSentenceBounds;
  LDEBUG << "    - updateGraph                 :" << m_updateGraph;
  LDEBUG << "    - resolveOverlappingEntities  :" << m_resolveOverlappingEntities;
  LDEBUG << "    - overlappingEntitiesStrategy :" << m_overlappingEntitiesStrategy;
  LDEBUG << "    - testAllVertices             :" << m_testAllVertices;
  LDEBUG << "    - stopAtFirstSuccess          :" << m_stopAtFirstSuccess;
  LDEBUG << "    - onlyOneSuccessPerType       :" << m_onlyOneSuccessPerType;
  LDEBUG << "    - graphId                     :" << m_graphId;
  LDEBUG << "    - dataForStorage              :" << m_dataForStorage;
#endif

  LimaStatusCode returnCode(SUCCESS_ID);

  auto recoData = std::dynamic_pointer_cast<RecognizerData>(analysis.getData("RecognizerData"));
  if (recoData == 0)
  {
    recoData = std::make_shared<RecognizerData>();
    analysis.setData("RecognizerData", recoData);
  }

  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData = std::make_shared<AnnotationData>();
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(
        annotationData.get(), "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  // data to possibly store the result (according to the actions)
  // assume all recognizers use the same entity type group (gloups)
  RecognizerResultData* resultData=
    new RecognizerResultData(m_graphId);
  recoData->setResultData(resultData);

  if (m_useSentenceBounds) {
    for (auto reco: m_recognizers) {
      returnCode = processOnEachSentence(analysis, reco.get(), recoData.get());
    }
  }
  else {
    for (auto reco: m_recognizers) {
      returnCode = processOnWholeText(analysis, reco.get(), recoData.get());
    }
  }

  if (m_updateGraph) {
//     LDEBUG << "";
    recoData->removeVertices(analysis);
    recoData->clearVerticesToRemove();
    recoData->removeEdges(analysis);
    recoData->clearEdgesToRemove();
  }

  if (! m_dataForStorage.empty()) {
    analysis.setData(m_dataForStorage,resultData);
  }
  else {
    // result data stored in recoData and resultData are same pointer
    recoData->deleteResultData();
    resultData=0;
  }

  // remove recognizer data (used only internally to this process unit)
  analysis.removeData("RecognizerData");

  return returnCode;
}

LimaStatusCode ApplyRecognizer::
processOnEachSentence(AnalysisContent& analysis,
                      Recognizer* reco,
                      RecognizerData* recoData) const
{
  APPRLOGINIT;

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData(recoData->getGraphId()));
  if (nullptr==anagraph)
  {
    LERROR << "graph with id '"<< recoData->getGraphId() <<"' is not available";
    return MISSING_DATA;
  }

  // get sentence bounds
  auto sb=std::dynamic_pointer_cast<SegmentationData>(analysis.getData(m_sentenceBoundsData));
  if (nullptr==sb)
  {
    LERROR << "no sentence bounds "<< m_sentenceBoundsData << " defined ! abort";
    return MISSING_DATA;
  }

  std::vector<RecognizerMatch> seRecognizerResult;
  // SegmentationData::const_iterator boundItr=sb->begin();
  std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
  // ??OME2 while (boundItr!=sb->end())
  while (boundItr!=(sb->getSegments()).end())
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    //LDEBUG << "ApplyRecognizer: analyze sentence from vertex " << beginSentence << " to vertex " << endSentence << QTENDL;

    seRecognizerResult.clear();
    reco->apply(*anagraph,beginSentence,
                        endSentence,analysis,seRecognizerResult);

    //remove overlapping entities
    if (m_resolveOverlappingEntities)
    {
      reco->resolveOverlappingEntities(seRecognizerResult,
                                       m_overlappingEntitiesStrategy);
    }

    boundItr++;
    recoData->nextSentence();
  }

  return SUCCESS_ID;
}

LimaStatusCode ApplyRecognizer::
processOnWholeText(AnalysisContent& analysis,
                   Recognizer* reco,
                   RecognizerData* recoData ) const
{
  // APPRLOGINIT;
  // LDEBUG << "apply recognizer on whole text";

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData(recoData->getGraphId()));
  if (nullptr == anagraph)
  {
    APPRLOGINIT;
    LERROR << "graph with id '"<< recoData->getGraphId() <<"' is not available";
    return MISSING_DATA;
  }

//   auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));

  std::vector<RecognizerMatch> seRecognizerResult;

  reco->apply(*anagraph,
              anagraph->firstVertex(),
              anagraph->lastVertex(),
              analysis,seRecognizerResult,
              m_testAllVertices,m_stopAtFirstSuccess,m_onlyOneSuccessPerType);

  //remove overlapping entities
  if (m_resolveOverlappingEntities)
  {
    reco->resolveOverlappingEntities(seRecognizerResult,
                                             m_overlappingEntitiesStrategy);
  }

  return SUCCESS_ID;
}

} // end namespace
} // end namespace
} // end namespace
