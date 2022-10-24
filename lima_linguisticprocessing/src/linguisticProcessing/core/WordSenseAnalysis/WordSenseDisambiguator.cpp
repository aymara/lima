// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        WordSenseDisambiguator.cpp
  * @author      Claire Mouton (Claire.Mouton@cea.fr)

  *              Copyright (c) 2010 by CEA
  * @date        Created on Aug, 17 2010
  *
  */

#include "WordSenseDisambiguator.h"
#include <boost/algorithm/string.hpp>
//#include "boost/graph/adjacency_list.hpp"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/misc/stringspool.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include <stack>
#include <iostream>
#include <math.h>
#include <fstream>





using namespace std;
//using namespace boost;
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;


namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
SimpleFactory<MediaProcessUnit,WordSenseDisambiguator> wordSenseDisambiguationFactory(WORDSENSEDISAMBIGUATIONPU_CLASSID);


void WordSenseDisambiguator::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOGINIT("WordSenseDisambiguator");
  m_language=manager->getInitializationParameters().media;

  try
  {
      string mode = unitConfiguration.getParamsValueAtKey("mode");
      LDEBUG << "Reading mode : " << mode;
      if (mode.compare("b_most_frequent")==0)
      {
    m_mode = B_MOST_FREQUENT;
      }
      else if (mode.compare("b_Romanseval_most_frequent")==0)
      {
    m_mode = B_ROMANSEVAL_MOST_FREQUENT;
      }
      else if (mode.compare("b_Jaws_most_frequent")==0)
      {
    m_mode = B_JAWS_MOST_FREQUENT;
      }
      else if (mode.compare("s_Wsi_mrd")==0)
      {
    m_mode = S_WSI_MRD;
      }
      else if (mode.compare("s_Wsi_Dempster_Schaffer")==0)
      {
    m_mode = S_WSI_DS;
      }
      else
      {
    m_mode = S_UNKNOWN;
      }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'mode' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_mode = S_UNKNOWN;
    LERROR << "Mode is set to UNKNOWN by default.";
  }
  LDEBUG << "Mode is "<< m_mode << " - "  << mode();
  try
  {
      string mapping = unitConfiguration.getParamsValueAtKey("mapping");
      if (mapping.compare("m_Romanseval_senses"))
      {
    m_mappingMode = M_ROMANSEVAL_SENSES;
      }
      else if (mapping.compare("m_Jaws_senses"))
      {
    m_mappingMode = M_JAWS_SENSES;
      }
      else
      {
    m_mappingMode = M_UNKNOWN;
      }
      try
      {
  string mappingFile = unitConfiguration.getParamsValueAtKey("mappingFile");
  loadMapping(mappingFile);
      }
      catch (Common::XMLConfigurationFiles::NoSuchParam& )
      {
  LERROR << "No 'mappingFile' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
  LERROR << "Mapping will not be performed.";
      }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'mapping' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_mappingMode = M_UNKNOWN;
    LERROR << "Scope is set to UNKNOWN by default.";
  }

  string dictionaryPath = "";
  try {
    dictionaryPath=unitConfiguration.getParamsValueAtKey("dictionaryFile");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'dictionaryFile' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    dictionaryPath = "words.ids";
    LERROR << "DictionaryFile is set to 'words.ids' by default.";
  }
  initDictionaries(dictionaryPath);



  try {
    m_sensesPath=unitConfiguration.getParamsValueAtKey("sensesPath");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No 'sensesPath' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_sensesPath = "clusterDir";
    LERROR << "SensesPath is set to 'clusterDir' by default.";
  }

  LDEBUG << "SensesPath config ok " ;


  if (mode()== S_WSI_MRD || mode()== S_WSI_DS) {
    try {
      deque<string> tmpDeque = unitConfiguration.getListsValueAtKey("NounContextList");
      for (deque<string>::const_iterator it = tmpDeque.begin(); it!=tmpDeque.end(); it++)
      {
        m_contextList["N"].insert(tmpDeque.begin(), tmpDeque.end());
      }
      tmpDeque.clear();
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      LWARN << "No 'NounContextList' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
      LWARN << "Default list for NounContext is set to : SUJ_V, COD_V, COMPDUNOM, COMPDUNOM.reverse, ADJPRENSUB.reverse, SUBADJPOST.rverse, window5" ;
    }


    LDEBUG << "ContextLists config ok " ;

    try {
      m_knnDir=unitConfiguration.getParamsValueAtKey("knnDir");
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      LERROR << "No 'knnDir' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    m_knnDir = "knnall";
      LERROR << "KnnDir is set to 'knnall' by default.";
    }

    LDEBUG << "KnnDir config ok " ;

    try
    {
      const map<string,string>& knnSearchConfig=unitConfiguration.getMapAtKey("knnsearchConfig");
      m_searcher = new KnnSearcher(knnSearchConfig);
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      LERROR << "No 'knnsearchConfig' defined in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    }

    LDEBUG << "KnnSearchConfig config ok " ;
  } // end mode == S_WSI_XX

  cerr << m_language << endl;
  const Common::PropertyCode::PropertyManager& macroManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO");

  m_macroAccessor=&macroManager.getPropertyAccessor();

  m_L_NC = macroManager.getPropertyValue("NC");
  m_L_NP = macroManager.getPropertyValue("NP");
  m_L_V = macroManager.getPropertyValue("V");
  m_L_ADJ = macroManager.getPropertyValue("ADJ");
  m_L_ADV = macroManager.getPropertyValue("ADV");
}


void WordSenseDisambiguator::initDictionaries(const string& dictionaryPath)
{
  LOGINIT("WordSenseDisambiguator");
  LINFO << "Loading dictionaries from " << dictionaryPath << ".";

  ifstream is(dictionaryPath.c_str(), std::ifstream::binary);
  if ( !is.good() ) {
      LERROR << "File " << dictionaryPath << " not read" ;
      if ( is.eof() ) {
  LERROR << "(reason is eof)" ;
      } else if ( is.fail() ) {
  LERROR << "(reason is fail)" ;
      } else if ( is.bad() ) {
  LERROR << "(reason is bad)" ;
      } else {
  LERROR << "(reason unknown)" ;
      }
      LERROR << ". ";
      return;
  }
  string s;
  while (getline(is, s)) {
    vector<string> strs;
    boost::split( strs, s, boost::is_any_of(" ") );
    stringstream ss;
    ss << strs.at(1);
    uint64_t id;
    ss>> id;
    m_lemma2Index[strs.at(0)] = id;
    m_index2Lemma[id] = strs.at(0);
  }
  is.close();
  LINFO << "Dictionaries loaded from " << dictionaryPath << ".";

}



void WordSenseDisambiguator::loadMapping(const string& mappingPath)
{
  LOGINIT("WordSenseDisambiguator");
  ifstream is(mappingPath.c_str(), std::ifstream::binary);
  if ( !is.good() ) {
      LERROR << "File " << mappingPath << " not read" ;
      if ( is.eof() ) {
  LERROR << "(reason is eof)" ;
      } else if ( is.fail() ) {
  LERROR << "(reason is fail)" ;
      } else if ( is.bad() ) {
  LERROR << "(reason is bad)" ;
      } else {
  LERROR << "(reason unknown)" ;
      }
      LERROR << ". ";
      return;
  }
  string s;
  while (getline(is, s)) {

  }
  is.close();
  LINFO << "Mapping loaded from " << mappingPath << ".";

}

/**
 *
 * @param analysis
 * @return
 */
LimaStatusCode WordSenseDisambiguator::process(
  AnalysisContent& analysis) const
{
  LOGINIT("WordSenseDisambiguator");
  TimeUtils::updateCurrentTime();
  LINFO << "start WordSenseDisambiguator";




  // create syntacticData
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
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds computed on graph '" << sb->getGraphId() << "'. WordSenseDisambiguator needs " <<
    "sentence bounds on PosGraph";
    return INVALID_CONFIGURATION;
  }
  //auto syntacticData = std::dynamic_pointer_cast<SyntacticData>(analysis.getData("SyntacticData"));
  if (sb==0)
  {
    LERROR << "no syntactic data ! abort";
    return MISSING_DATA;
  }


  /** Access to or creation of an annotation graph */
  auto annotationData = std::dynamic_pointer_cast<AnnotationData>(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData = std::make_shared<AnnotationData>();
    /** Creates a node in the annotation graph for each node of the
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData.get(), "AnalysisGraph");
    }
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph")) != 0)
    {

      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"))->populateAnnotationGraph(annotationData.get(), "PosGraph");
    }

    analysis.setData("AnnotationData",annotationData);
  }


  /**
   * To be able to dump the content of an annotation, a function pointer with a precise signature has to be givent to
   * the annotation graph. See @ref{WordSenseAnnotation.h} for the details of the DumpWordSense() function
   */
  if (annotationData->dumpFunction("WordSense") == 0)
  {
    annotationData->dumpFunction("WordSense", new DumpWordSense());
  }

  const FsaStringsPool& stringspool= Common::MediaticData::MediaticData::single().stringsPool(m_language);

  LinguisticGraph* graph=anagraph->getGraph();
  set< LinguisticGraphVertex > alreadyProcessedVertices;



  map<string, WordUnit> referenceWords;
  vector<TargetWordWithContext> targetWords;


  //LinguisticGraphVertex beginSentence=sb->getStartVertex();
  // for each sentence
  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
       boundItr!=(sb->getSegments()).end();
       boundItr++)
  {
    LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
    LinguisticGraphVertex endSentence=boundItr->getLastVertex();
    LINFO << "analyze sentence from vertex " << beginSentence << " to vertex " << endSentence;

    // Parse Sentence
    // for each word in the sentence
    LinguisticGraphVertex v=beginSentence;
    vector<set<uint64_t> >lemmasBuffer;
    while (v!=endSentence)
    {
  //LDEBUG << "Processing vertex : " << v ;
      /*
      if (alreadyProcessedVertices.find(v) != alreadyProcessedVertices.end())
      {
  LinguisticGraphOutEdgeIt ite, ite_end;
  boost::tie(ite, ite_end)=boost::out_edges(v, *graph);
  v=target(*ite, *graph);
        continue;
      }*/

      // if v is empty, continue
      MorphoSyntacticData* data = get(vertex_data,*graph,v);
      if (data == 0
    || data->empty() )
      {
  LinguisticGraphOutEdgeIt ite, ite_end;
  boost::tie(ite, ite_end)=boost::out_edges(v, *graph);
  v=target(*ite, *graph);
  continue;
      }
      set<string> lemmas;
      getLemmas(data, stringspool, lemmas);
      set<uint64_t> lemmasIds;
      for (set<string>::const_iterator itLemmas = lemmas.begin(); itLemmas != lemmas.end(); itLemmas++)
      {
  // Store preview window contexts
  LDEBUG << "Storing preview window contexts... ";
  if ( data->firstValue(*m_macroAccessor) == m_L_NC
    || data->firstValue(*m_macroAccessor) == m_L_NP
    || data->firstValue(*m_macroAccessor) == m_L_V
    || data->firstValue(*m_macroAccessor) == m_L_ADJ
    || data->firstValue(*m_macroAccessor) == m_L_ADV
  )
  {
    //LDEBUG << "lemma : "<< *itLemmas ;
    lemmasIds.insert(lemma2Index(*itLemmas));
  }

  // Load matching Reference Words
  if(data->firstValue(*m_macroAccessor) != m_L_NC
  && data->firstValue(*m_macroAccessor) != m_L_NP)
  {
    // Don't process nouns
    continue;
  }

  WordUnit wu(m_searcher, m_lemma2Index, m_index2Lemma, *itLemmas, mode(), m_sensesPath);
  /*
  LDEBUG << "test size : " << wu.wordSensesUnits().size();
    if (wu.wordSensesUnits().size()>0) {
    LDEBUG << "test begint it : " << wu.wordSensesUnits().begin()->senseId();
    LDEBUG << "test begint it : " << (++wu.wordSensesUnits().begin())->senseId();
    }
  for(set<WordSenseUnit>::iterator itSenses = wu.wordSensesUnits().begin();
           itSenses!= wu.wordSensesUnits().end();
           itSenses++ ) {

  LDEBUG << "Checking 4 Sense id "<< *itLemmas << " : " << itSenses->senseId()
    << " : " << itSenses->senseTag()
    << " : " << itSenses->parentLemmaId();
  }*/

  referenceWords[*itLemmas] = wu;


  LDEBUG << "Added wordunit : " << *itLemmas << " : " << wu ;



      }
      lemmasBuffer.push_back(lemmasIds);

      // Load matching Target Words
      //LDEBUG << "Loading matching target words... ";
      map<string, set<uint64_t> >context = map<string, set<uint64_t> >();

      // Get syntactic contexts
      //int nbContxts = getContext(syntacticData, v, graph, stringspool, context);
      //LDEBUG << "Nb contexts : " << nbContxts << " vs. " << context.size();

      // Add prestored preview window context
      addPreviewWindowContext(lemmasBuffer, context);


      // Store context into targetWords
       /* print debug */
       /*
      LINFO << "Storing current context into targetWords... ";
      for (set<uint64_t>::iterator itLemmaId = lemmasIds.begin();
        itLemmaId != lemmasIds.end();
        itLemmaId++)
      {
  cerr << "Adding "<< *itLemmaId << endl;
  for (SemanticContext::iterator itCtxt = context.begin();
               itCtxt!= context.end();
               itCtxt++)
  {
    LDEBUG << itCtxt->first << " : " ;
    for (set<uint64_t>::iterator itValues = itCtxt->second.begin();
            itValues!= itCtxt->second.end();
            itValues++)
    {
      LDEBUG << *itValues ;
    }
  }
      }
      */
      /* end print debug */

      targetWords.push_back(TargetWordWithContext(lemmas, v, context));


      // Add postview window contexts
      //LDEBUG << "Adding postview window contexts... ";
      addPostviewWindowContext(lemmasIds, targetWords);


      // Prepare Next

      alreadyProcessedVertices.insert(v);
      LinguisticGraphOutEdgeIt ite, ite_end;
      boost::tie(ite, ite_end)=boost::out_edges(v, *graph);
      v=target(*ite, *graph);
    } //end for each word in sentence


    // Disambiguate Target Words (separate step due to window contexts addition)
    for (vector<TargetWordWithContext>::const_iterator  itTargets = targetWords.begin();
                 itTargets != targetWords.end();
                 itTargets++)
    {
      for (set<string>::const_iterator itLemmas = itTargets->lemmas.begin();
               itLemmas!= itTargets->lemmas.end();
               itLemmas++)
      {
  //* debug printing
  LDEBUG << "Context of " << *itLemmas << " : ";
  for (SemanticContext::const_iterator itContext = itTargets->context.begin();
               itContext != itTargets->context.end();
               itContext++)
  {
    LDEBUG << "Rel " << itContext->first << " : ";
    for (set<uint64_t>::iterator itContextValue = itContext->second.begin();
               itContextValue != itContext->second.end();
             itContextValue++)
    {
      if(m_index2Lemma.find(*itContextValue)!=m_index2Lemma.end())
      {
        LDEBUG << "Contextual value : " << index2Lemma(*itContextValue) ;
      }
    }

  }
  //* end debug printing

  // Instanciate annotation
  WordSenseAnnotation wsa (mode(), mapping(), itTargets->vertex);

  // Disambiguate
  LDEBUG << *itLemmas ;
  if (referenceWords.find(*itLemmas) != referenceWords.end())
  {
    LDEBUG << "Reference word found for " << *itLemmas << " and mode is " << mode();
    bool disambOk=false;
    switch (mode())
    {
      case B_MOST_FREQUENT:
      case B_ROMANSEVAL_MOST_FREQUENT:
      case B_JAWS_MOST_FREQUENT:
        LDEBUG << "Disambiguation processing : MOST_FREQUENT";
        disambOk=wsa.disambiguate(referenceWords[*itLemmas]);
        break;
      case S_WSI_MRD:
        LDEBUG << "Disambiguation processing : WSI_MRD";
        try
        {
          disambOk=wsa.disambiguate(m_searcher, referenceWords[*itLemmas], itTargets->context, 0.95, 'A');
        }
        catch (std::exception &e)
        {
          LDEBUG << "LPException";
          LDEBUG << e.what();
        }
        break;
      default:
        LWARN << "No Disambiguation processing. Bad configuration";
        break;
    }
    if (disambOk)
    {
      LINFO << "write word sense annotations for "<< *itLemmas <<" on graph";
      wsa.writeAnnotation(annotationData.get());
    }
    else
    {
      LWARN << *itLemmas << " was not disambiguated (still ambiguous).";
    }
  }
  else
  {
    LWARN << *itLemmas << " was not disambiguated (no referenceWord).";
  }
      } // end ambiguous lemmas
    } // end ambiguous words

    // Release target words
    targetWords.clear();
    beginSentence=endSentence;
  } // end for each sentence in Doc

  // Release Reference Words
  referenceWords.clear();

  TimeUtils::logElapsedTime("WordSense");
  return SUCCESS_ID;
}

int WordSenseDisambiguator::addPostviewWindowContext(const set<uint64_t>& lemmasIds,
                                                     vector<TargetWordWithContext>& targetWordsWithContext) const
{
  int cntPostContext = 0;
  int maxPostContext = 20;
  if (lemmasIds.size()>0)
  {
    for (vector<TargetWordWithContext>::reverse_iterator itStoredContext = targetWordsWithContext.rbegin()+1;
                itStoredContext != targetWordsWithContext.rend();
                itStoredContext++)
    {
      if (cntPostContext >= maxPostContext)
      {
  break;
      }
      if (cntPostContext < 5)
      {
  if (contextList("N").find("window5") != contextList("N").end())
  {
    itStoredContext->context["window5"].insert(lemmasIds.begin(), lemmasIds.end());
  }
  if (cntPostContext < 10)
  {
    if (contextList("N").find("window10") != contextList("N").end())
    {
      itStoredContext->context["window10"].insert(lemmasIds.begin(), lemmasIds.end());
    }
    if (cntPostContext < 20)
    {
      if (contextList("N").find("window20") != contextList("N").end())
      {
        itStoredContext->context["window20"].insert(lemmasIds.begin(), lemmasIds.end());
      }
    }
  }
      }
      cntPostContext++;
    }
  }
  return targetWordsWithContext.size();
}


int WordSenseDisambiguator::addPreviewWindowContext(vector<set<uint64_t> >& previewWindow, map<string, set<uint64_t> >& context) const
{
  int cnt = 0;
  int max = 20;
  cerr << "previewWindow.size() " << previewWindow.size() << endl;
  for (vector<set<uint64_t> >::reverse_iterator itWindow = previewWindow.rbegin()+1;
               itWindow != previewWindow.rend();
               itWindow++)
  {
    if (cnt>=max)
    {
      break;
    }
    if (cnt < 5)
    {
      if (contextList("N").find("window5") != contextList("N").end())
      {
  context["window5"].insert(itWindow->begin(), itWindow->end());
      }
      if (cnt < 10)
      {
  if (contextList("N").find("window10") != contextList("N").end())
  {
    context["window10"].insert(itWindow->begin(), itWindow->end());
  }
  if (cnt < 20)
  {
    if (contextList("N").find("window20") != contextList("N").end())
    {
      context["window20"].insert(itWindow->begin(), itWindow->end());
    }
  }
      }
    }
    cnt++;
  }
  return context.size();
}

int WordSenseDisambiguator::getContext(SyntacticData* syntacticData,
                                       LinguisticGraphVertex& v,
                                       LinguisticGraph* graph,
                                       const FsaStringsPool& stringspool,
                                       map<string, set<uint64_t> >& context) const
{
  // get Dependency graph and relations
  EdgeDepRelTypePropertyMap map = get(edge_deprel_type, *(syntacticData-> dependencyGraph()));
  DependencyGraphVertex dv = syntacticData-> depVertexForTokenVertex(v);

  // out edges
  DependencyGraphOutEdgeIt it_out, it_out_end;
  boost::tie(it_out, it_out_end) = out_edges(dv, *(syntacticData-> dependencyGraph()));
  for (; it_out != it_out_end; it_out++)
  {
    LinguisticGraphVertex targetV = target(*it_out, *(syntacticData-> dependencyGraph()));
    set<string> targetLemmas;
    MorphoSyntacticData* targetData = get(vertex_data,*graph,targetV);
    getLemmas(targetData, stringspool, targetLemmas);
    for (set<string>::iterator itLemmas = targetLemmas.begin(); itLemmas != targetLemmas.end(); itLemmas++)
    {
      string relation = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(map[*it_out]);
      if (contextList("N").find(relation) != contextList("N").end())
      {
  context[relation].insert(lemma2Index(*itLemmas));
      }
    }
  }

  // in edges
  DependencyGraphInEdgeIt it_in, it_in_end;
  boost::tie(it_in, it_in_end) = in_edges(dv, *(syntacticData-> dependencyGraph()));
  for (; it_in != it_in_end; it_in++)
  {
    LinguisticGraphVertex sourceV = source(*it_in, *(syntacticData-> dependencyGraph()));
    set<string> sourceLemmas;
    MorphoSyntacticData* sourceData = get(vertex_data,*graph,sourceV);
    getLemmas(sourceData, stringspool, sourceLemmas);
    for (set<string>::iterator itLemmas = sourceLemmas.begin(); itLemmas != sourceLemmas.end(); itLemmas++)
    {
      string relation = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(map[*it_in])+".reverse";
      if (contextList("N").find(relation) != contextList("N").end())
      {
  context[relation].insert(lemma2Index(*itLemmas));
      }
    }
  }
  return context.size();
}


int WordSenseDisambiguator::getLemmas (MorphoSyntacticData* data,
                                       const FsaStringsPool& stringspool,
                                       set<string>& lemmas) const
{
  std::set<StringsPoolIndex> forms=data->allLemma();
  for (std::set<StringsPoolIndex>::const_iterator formItr=forms.begin();
    formItr!=forms.end();
    formItr++)
  {
    lemmas.insert(Common::Misc::limastring2utf8stdstring(stringspool[*formItr]));
  }
  return lemmas.size();
}


} // closing namespace WordSenseDisambiguation
} // closing namespace LinguisticProcessing
} // closing namespace Lima
