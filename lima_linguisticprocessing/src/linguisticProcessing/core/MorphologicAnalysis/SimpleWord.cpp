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

// NAUTITIA
//
// jys 8-OCT-2002
//
// SimpleWord is the implementation of the 1st module of
// Morphological Analysis. Each token from the main tokens
// path is searched into the specified dictionary.


#include "SimpleWord.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntry.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/misc/fsaStringsPool.h"
#include "MorphoSyntacticDataHandler.h"
#include "ConcatenatedDataHandler.h"
#include "AccentedConcatenatedDataHandler.h"
#include "SequenceEntryHandler.h"

#include <iostream>

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::AnalysisDict;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,SimpleWord> SimpleWordFactory(SIMPLEWORD_CLASSID);


SimpleWord::SimpleWord() :
    m_reader(0)
{}

SimpleWord::~SimpleWord()
{
  delete m_reader;
}

void SimpleWord::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  MediaId language = manager->getInitializationParameters().media;
  m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);
  string dico;
  try
  {
    dico=unitConfiguration.getParamsValueAtKey("dictionary");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in SimpleWord group for language " << (int) language;
    throw InvalidConfiguration();
  }

  AbstractResource* res=LinguisticResources::single().getResource(language,dico);
  m_dictionary=static_cast<AbstractAnalysisDictionary*>(res);

  try
  {
    string confident=unitConfiguration.getParamsValueAtKey("confidentMode");
#ifdef DEBUG_LP
    LDEBUG << "SimpleWord set confident mode to:" << confident;
#endif
    m_confidentMode=(confident=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "no param 'confidentMode' in SimpleWord group for language " << (int) language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }

  // initialize dictionary reader

  try
  {
    string chart=unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res= LinguisticResources::single().getResource(language,chart);
    m_charChart=static_cast<FlatTokenizer::CharChart*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'charChart' in SimpleWord group for language " << (int) language;
    throw InvalidConfiguration();
  }

  m_reader=new AlternativesReader(m_confidentMode,true,true,true,m_charChart,&Common::MediaticData::MediaticData::changeable().stringsPool(language));

  try
  {
    string concat=unitConfiguration.getParamsValueAtKey("parseConcatenated");
    m_parseConcatenated=(concat=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "no param 'parseConcatenated' in SimpleWord group for language " << (int) language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }

}


LimaStatusCode SimpleWord::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SimpleWord");
  MORPHOLOGINIT;
  LINFO << "starting process SimpleWord";

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));


  LinguisticGraph* g=tokenList->getGraph();
  LinguisticGraphVertexIt it,itEnd;
  VertexTokenPropertyMap tokenMap=get(vertex_token,*g);
  VertexDataPropertyMap dataMap=get(vertex_data,*g);
  boost::tie(it,itEnd)=vertices(*g);
  for (;it!=itEnd;it++)
  {
    Token* currentToken=tokenMap[*it];
    if (currentToken!=0)
    {
#ifdef DEBUG_LP
      LDEBUG << "SimpleWord for token" <<  currentToken->stringForm();
#endif
      // Init handlers
      MorphoSyntacticData* msd=dataMap[*it];
      AbstractDictionaryEntryHandler* lingInfoHandler=new MorphoSyntacticDataHandler(*msd,SIMPLE_WORD);
      ConcatenatedDataHandler* concatHandler=0;
      AccentedConcatenatedDataHandler* accentedConcatHandler=0;
      AbstractDictionaryEntryHandler* accentedHandler=lingInfoHandler;

      if (m_parseConcatenated)
      {
        concatHandler=new ConcatenatedDataHandler(g,currentToken,SIMPLE_WORD,m_sp);
        accentedConcatHandler=new AccentedConcatenatedDataHandler(
          g,
          currentToken->stringForm(),
          currentToken->position(),
          currentToken->status(),
          SIMPLE_WORD,
          m_sp,
          m_charChart);
        SequenceEntryHandler* seh=new SequenceEntryHandler();
        seh->addHandler(lingInfoHandler);
        seh->addHandler(accentedConcatHandler);
        accentedHandler=seh;
      }

      // parse data
      DictionaryEntry entry(m_dictionary->getEntry(currentToken->form(),
                                                   currentToken->stringForm()));
      m_reader->readAlternatives(*currentToken,
                                 *m_dictionary,
                                 lingInfoHandler,
                                 concatHandler,
                                 accentedHandler);

      // finalize
      if (concatHandler && !concatHandler->getConcatVertices().empty())
      {
        linkConcatVertices(g,*it,concatHandler->getConcatVertices());
        if (msd->empty())
        {
          clear_vertex(*it,*g);
        }
      }
      if (accentedConcatHandler && !accentedConcatHandler->getConcatVertices().empty())
      {
        linkConcatVertices(g,*it,accentedConcatHandler->getConcatVertices());
        if (msd->empty())
        {
          clear_vertex(*it,*g);
        }
      }
      if (m_parseConcatenated)
      {
        delete concatHandler;
        delete accentedConcatHandler;
        delete accentedHandler;
      }
      delete lingInfoHandler;
    }
  }

  auto annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LINFO << "SimpleWord::process no annotation data, creating and populating it";
    annotationData = new AnnotationData();
    analysis.setData("AnnotationData", annotationData);
  }
  tokenList->populateAnnotationGraph(annotationData, "AnalysisGraph");
  if (static_cast<AnalysisGraph*>(analysis.getData("PosGraph")) != 0)
  {
    static_cast<AnalysisGraph*>(analysis.getData("PosGraph"))->populateAnnotationGraph(
      annotationData,
      "PosGraph");
  }

#ifdef DEBUG_LP
  LDEBUG << "ending process SimpleWord";
#endif
  return SUCCESS_ID;
}

void SimpleWord::linkConcatVertices(
  LinguisticGraph* graph,
  LinguisticGraphVertex srcToken,
  const std::vector<std::vector<LinguisticGraphVertex> >& concats) const
{
  LinguisticGraphInEdgeIt ieItr,ieItrEnd;
  for (boost::tie(ieItr,ieItrEnd) = in_edges(srcToken,*graph);
       ieItr!=ieItrEnd;
       ieItr++)
  {
     LinguisticGraphVertex pred=source(*ieItr,*graph);
     LinguisticGraphOutEdgeIt oeItr,oeItrEnd;
     for (boost::tie(oeItr,oeItrEnd) = out_edges(srcToken,*graph);
          oeItr!=oeItrEnd;
          oeItr++)
     {
       LinguisticGraphVertex next=target(*oeItr,*graph);
       for (std::vector<std::vector<LinguisticGraphVertex> >::const_iterator concatItr=concats.begin();
            concatItr!=concats.end();
            concatItr++)
       {
         add_edge(pred,concatItr->front(),*graph);
         add_edge(concatItr->back(),next,*graph);
       }
     }
  }
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
