/*
    Copyright 2002-2018 CEA LIST

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
  * @brief  HyphenWordAlternatives is the module which creates split alternatives
  *         for hyphen word tokens. Each token from the supplied tokens path is processed :
  *         o FullToken must be "AlphaHyphen" typed by Tokenizer.
  *         o If a token has a single word entry or an orthographic alternative
  *           it is not decomposed
  *         o Token is break at hyphen boundaries and a new alternative path is created
  *         o each FullToken of the new Path is searched into dictionnary as Simple Word
  *         o If special hyphen entry, no alternatives are searched,
  *           otherwise Accented alternatives are searched
  *         o Path is valid even if not all FullToken have entry into dictionary
  *         @b
  *         Modified @date Dec, 02 2002 by GC to handle splitting on t_alpha_possessive
  *
  * @file   HyphenWordAlternatives.cpp
  * @author NAUTITIA jys
  * @author Gael de Chalendar
  * @author Copyright (c) 2002-2003 by CEA
  *
  * @date   created on Nov, 30 2002
  * @version    $Id$
  *
  */

#include "HyphenWordAlternatives.h"
#include "MorphoSyntacticDataHandler.h"

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/time/timeUtilsController.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::AnalysisDict;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,HyphenWordAlternatives> hyphenwordAlternativesFactory(HYPHENWORDALTERNATIVESFACTORY_CLASSID);

HyphenWordAlternatives::HyphenWordAlternatives()
{}

HyphenWordAlternatives::~HyphenWordAlternatives()
{
  delete m_reader;
}

void HyphenWordAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  MORPHOLOGINIT;
  m_language = manager->getInitializationParameters().media;
  try
  {
    string dico=unitConfiguration.getParamsValueAtKey("dictionary");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,dico);
    m_dictionary=static_cast<AnalysisDict::AbstractAnalysisDictionary*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in HyphenWordAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    string charchart=unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,charchart);
    m_charChart=static_cast<FlatTokenizer::CharChart*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'charChart' in HyphenWordAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    string tok=unitConfiguration.getParamsValueAtKey("tokenizer");
    const MediaProcessUnit* res=manager->getObject(tok);
    m_tokenizer=static_cast<const FlatTokenizer::Tokenizer*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in HyphenWordAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    m_deleteHyphenWord=( unitConfiguration.getParamsValueAtKey("deleteHyphenWord") == "true");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no param 'deleteHyphenWord' in HyphenAlternatives group for language " << (int) m_language;
    LWARN << "use default value : true";
    m_deleteHyphenWord=true;
  }
  try
  {
    string confident=unitConfiguration.getParamsValueAtKey("confidentMode");
    m_confidentMode=(confident=="true");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no param 'confidentMode' in HyphenWordAlternatives group for language " << (int) m_language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }
  FsaStringsPool* sp=&Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
  m_reader=new AlternativesReader(m_confidentMode,true,true,true,m_charChart,sp);

  const auto &theMediaticData = static_cast<const Common::MediaticData::MediaticData&>(Common::MediaticData::MediaticData::single());
  m_engLanguageId = theMediaticData.getMediaId("eng");
}

LimaStatusCode HyphenWordAlternatives::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("HyphenWordAlternatives");
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process HyphenWordAlternatives";

  auto const& stopAnalyze = analysis.stopAnalyze();
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LDEBUG << "HyphenWordAlternatives::process: Misssing AnnotationData. Create it";
    annotationData = new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=tokenList->getGraph();

  VertexDataPropertyMap dataMap = get( vertex_data, *graph );
  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );

  try
  {
    LinguisticGraphVertexIt it, it_end;
    boost::tie(it, it_end) = vertices(*graph);
    for (; it != it_end; it++)
    {
      if (stopAnalyze)
		  {
			  LERROR << "Analyze too long. Stopped in HyphenWordAlternatives";
			  return TIME_OVERFLOW;
		  }
      MorphoSyntacticData* currentToken = dataMap[*it];
      Token* tok= tokenMap[*it];
      if (currentToken==0) continue;
      //<if a token has a linguistic data
      // it is not decomposed>
      if (currentToken->size() == 0)
      {
        if (tok->status().isAlphaHyphen() && isWorthSplitting(*it, graph))
        {
          makeHyphenSplitAlternativeFor(*it, graph, annotationData);
        }
      }
    }
  }
  catch (std::exception &exc)
  {
    MORPHOLOGINIT;
    LWARN << "Exception in HyphenWordAlternatives : " << exc.what();
    return UNKNOWN_ERROR;
  }

  LINFO << "MorphologicalAnalysis: ending process HyphenWordAlternatives";
  return SUCCESS_ID;
}

/*
 * Hyphenated words are often absent in the dictionary. This leads to inability to treat them properly
 * on the level of rules. It's worth to split hyphenated words if their parts would bring more information
 * in the later stages of the analysis.
 *
 * Verification is implemented for English only. Always true for other languages.
 *
 * It's considered that it makes sense to split English hypthenated words in following cases:
 * - its right part is capitalized
 * - its right part exists in the dictionary.
 *
 * Examples:
 *
 * April-June         (right part capitalized)
 * Ben-Elissar        (right part capitalized)
 * Singapore-listed   ("listed" exists in the dictionary)
 * record-breaking    ("breaking" exists in the dictionary)
 * EU-wide            ("wide" exists in the dictionary)
 */
bool HyphenWordAlternatives::isWorthSplitting(
  LinguisticGraphVertex splitted,
  LinguisticGraph* graph) const
{
  if (m_engLanguageId != m_language)
      return true;

  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );
  Token* currentToken = tokenMap[splitted];

  LimaString hyphenWord(currentToken->stringForm());

#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "isWorthSplitting for " << hyphenWord;
#endif

  // find last hyphen
  int pos = hyphenWord.lastIndexOf(LimaChar(L'-'));
  if (pos < 0 || (hyphenWord.length() - pos < 3)) {
#ifdef DEBUG_LP
    LDEBUG << "isWorthSplitting: pos = " << pos;
#endif
    return false;
  }

  LimaString rightPart = hyphenWord.right(hyphenWord.length() - pos - 1);

#ifdef DEBUG_LP
  LDEBUG << "isWorthSplitting: rightPart = " << rightPart;
#endif

  QChar firstChar = rightPart[0];
  if (firstChar.isLetter() && firstChar.isUpper()) {
#ifdef DEBUG_LP
    LDEBUG << "isWorthSplitting: first char is upper letter";
#endif
    return true;
  }

  DictionaryEntry dicoEntry(m_dictionary->getEntry(rightPart));
  if (dicoEntry.isEmpty()) {
#ifdef DEBUG_LP
    LDEBUG << "isWorthSplitting: dicoEntry is empty. Don't split.";
#endif
    return false;
  }

  return true;
}

void HyphenWordAlternatives::makeHyphenSplitAlternativeFor(
  LinguisticGraphVertex splitted,
  LinguisticGraph* graph,
  AnnotationData* annotationData) const
{
  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );
  VertexDataPropertyMap dataMap = get( vertex_data, *graph );
  Token* currentToken = tokenMap[splitted];

  // first, get a copy of token string
  LimaString hyphenWord(currentToken->stringForm());

  // first replace hyphens by spaces
  int pos = hyphenWord.indexOf(LimaChar(L'-'), 0);
  while (pos != -1)
  {
    hyphenWord[(int)pos] = LimaChar(L' ');
    pos = hyphenWord.indexOf(LimaChar(L'-'), pos+1);
  }
  // then submit string to Tokenizer
  AnalysisContent toTokenize;
  toTokenize.setData("Text",new LimaStringText(hyphenWord));
  LimaStatusCode status=m_tokenizer->process(toTokenize);
  if (status != SUCCESS_ID) return;
  AnalysisGraph* agTokenizer=static_cast<AnalysisGraph*>(toTokenize.getData("AnalysisGraph"));
  LinguisticGraph* tokgraph=agTokenizer->getGraph();

  // setup position field
  // insert each new FullToken into alternative path
  uint64_t beginPos = currentToken->position()-1;
  LinguisticGraphVertex previous = splitted;
  LinguisticGraphVertex currentVx=agTokenizer->firstVertex();
  // go one step forward on the new path
  {
    LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
    boost::tie(adjItr,adjItrEnd) = adjacent_vertices(currentVx,*tokgraph);
    if (adjItr==adjItrEnd)
    {
      MORPHOLOGINIT;
      LERROR << "HypenWordAlternatives : no token forward !";
      throw LinguisticProcessingException();
    }
    currentVx=*adjItr;
  }
  //    LinguisticGraphVertex lastVx=agTokenizer->lastVertex();
  VertexTokenPropertyMap tokTokenMap=get(vertex_token,*tokgraph);
  Token* tokenizerToken=tokTokenMap[currentVx];

  bool isFirst=true;

  while (tokenizerToken)
  {
    // prepare the new vertex
    Token* newFT=new Token(*tokenizerToken);
    newFT->status().setAlphaHyphen( true );
    MorphoSyntacticData* newData=new MorphoSyntacticData();
    LinguisticGraphVertex newVertex = add_vertex(*graph);

    AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
    annotationData->addMatching("AnalysisGraph", newVertex, "annot", agv);
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), newVertex);


    tokenMap[newVertex]=newFT;
    dataMap[newVertex]=newData;
    newFT-> setPosition(newFT->position() + beginPos);
    const LimaString& newTokenStr=newFT->stringForm();
    MorphoSyntacticDataHandler handler(*newData,HYPHEN_ALTERNATIVE);

    if (isFirst)
    {
      LimaString newTokHyphen(newTokenStr);
      newTokHyphen.append(LimaChar('-'));
      DictionaryEntry dicoEntry(m_dictionary->getEntry(newTokHyphen));
      if (!dicoEntry.isEmpty() && dicoEntry.hasLingInfos())
      {
        FsaStringsPool* sp=&Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
        Token* newFT2=new Token((*sp)[newTokHyphen],newTokHyphen,newFT->position(),newFT->length()+1);
        tokenMap[newVertex]=newFT2;
        delete newFT;
        newFT = newFT2;
        dicoEntry.parseLingInfos(&handler);
      }
      else
      {
        m_reader->readAlternatives(
          *newFT,
          *m_dictionary,
          &handler,
          0,
          &handler);
      }
    }
    else
    {
        m_reader->readAlternatives(
          *newFT,
          *m_dictionary,
          &handler,
          0,
          &handler);
    }

    // links the new vertex to its predecessor in the graph
    if (previous == splitted)
    {
      LinguisticGraphInEdgeIt ite, ite_end;
      boost::tie(ite, ite_end) = in_edges(splitted, *graph);
      for (; ite != ite_end; ite++)
      {
        add_edge(source(*ite,*graph), newVertex, *graph);
      }
    }
    else
    {
      add_edge(previous, newVertex, *graph);
    }
    previous = newVertex;
    // go one step forward on the new path
    LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
    boost::tie(adjItr,adjItrEnd) = adjacent_vertices(currentVx,*tokgraph);
    if (adjItr==adjItrEnd)
    {
      MORPHOLOGINIT;
      LERROR << "HypenWordAlternatives : no token forward !";
      throw LinguisticProcessingException();
    }
    currentVx=*adjItr;
    tokenizerToken=tokTokenMap[currentVx];
  }

  // links the last new vertex created to the successors of the splitted vertex
  LinguisticGraphOutEdgeIt ite, ite_end;
  boost::tie(ite, ite_end) = out_edges(splitted, *graph);
  for (; ite != ite_end; ite++)
  {
    add_edge(previous, target(*ite,*graph), *graph);
  }

  // if have to delete hyphen word, then clear it in the graph
  if (m_deleteHyphenWord)
  {
    clear_vertex(splitted,*graph);
  }
}

} // closing namespace MorphologicAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
