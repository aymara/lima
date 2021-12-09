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
  * @brief  AbbreviationSplitAlternatives is the module which creates split alternatives
  *         for hyphen word tokens. Each token from the supplied tokens path is processed :
  *         o FullToken must be "AlphaHyphen" typed by Tokenizer.
  *         o If a token has a single word entry or an orthographic alternative
  *           it is not decomposed
  *         o Token is break at hyphen boundaries and a new alternative path is created
  *         o each FullToken of the new Path is searched into dictionnary as Simple Word
  *         o If special hyphen entry, no alternatives are searched,
  *           otherwise Accented alternatives are searched
  *         o Path is valid even if not all FullToken have entry into dictionary
  *         @br
  *         Modified @date Dec, 02 2002 by GC to handle splitting on t_alpha_possessive
  *
  * @file   AbbreviationSplitAlternatives.cpp
  * @author NAUTITIA jys
  * @author Gael de Chalendar
  * @author Copyright (c) 2002-2003 by CEA
  *
  * @date   created on Nov, 30 2002
  * @version    $Id$
  *
  */

#include "AbbreviationSplitAlternatives.h"
#include "MorphoSyntacticDataHandler.h"

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/time/timeUtilsController.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/FlatTokenizer/Tokenizer.h"

#include <boost/regex.hpp>

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::AnalysisDict;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,AbbreviationSplitAlternatives> abbreviationSplitAlternativesFactory(ABBREVIATIONSPLITALTERNATIVESFACTORY_CLASSID);

AbbreviationSplitAlternatives::AbbreviationSplitAlternatives() :
m_tokenizer(0),
m_dictionary(0),
m_abbreviations(),
m_language(),
m_confidentMode(true),
m_reader(0),
m_charSplitRegexp()
{
  // default split regexp: split on simple quote or UTF-8 right quotation mark
  LimaString quotes=Common::Misc::utf8stdstring2limastring("['’]");
  m_charSplitRegexp=QRegExp(quotes);

}

AbbreviationSplitAlternatives::~AbbreviationSplitAlternatives()
{
  if (m_reader) {
    delete m_reader;
  }
}


void AbbreviationSplitAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  MORPHOLOGINIT;
  m_language=manager->getInitializationParameters().media;
  try
  {
    string dico=unitConfiguration.getParamsValueAtKey("dictionary");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,dico);
    m_dictionary=static_cast<AnalysisDict::AbstractAnalysisDictionary*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in AbbreviationSplitAlternatives group for language " << (int) m_language;
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
    LERROR << "no param 'tokenizer' in AbbreviationSplitAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    deque<string> abbs=unitConfiguration.getListsValueAtKey("abbreviations");
    for (deque<string>::iterator it=abbs.begin();
         it!=abbs.end();
         it++)
    {
      m_abbreviations.push_back(Common::Misc::utf8stdstring2limastring(*it));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LERROR << "no list 'abbreviations' in AbbreviationSplitAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  FlatTokenizer::CharChart* charChart(0);
  try
  {
    string charchart=unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,charchart);
    charChart=static_cast<FlatTokenizer::CharChart*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'charChart' in AbbreviationSplitAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  try
  {
    string confident=unitConfiguration.getParamsValueAtKey("confidentMode");
    m_confidentMode=(confident=="true");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no param 'confidentMode' in AbbreviationSplitAlternatives group for language " << (int) m_language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }

  try
  {
    string charSplit=unitConfiguration.getParamsValueAtKey("charSplitRegexp");
    m_charSplitRegexp=QRegExp(Common::Misc::utf8stdstring2limastring(charSplit));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "no param 'confidentMode' in AbbreviationSplitAlternatives group for language " << (int) m_language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }

  FsaStringsPool* sp=&Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
  m_reader=new AlternativesReader(m_confidentMode,true,true,true,charChart,sp);

}

LimaStatusCode AbbreviationSplitAlternatives::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("AbbreviationSplitAlternatives");
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process AbbreviationSplitAlternatives";

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=tokenList->getGraph();

  VertexDataPropertyMap dataMap = get( vertex_data, *graph );
  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LDEBUG << "AbbreviationSplitAlternatives::process: Misssing AnnotationData. Create it";
    annotationData = new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  try
  {

    LinguisticGraphVertexIt it, it_end;
    boost::tie(it, it_end) = vertices(*graph);
    for (; it != it_end; it++)
    {
      MorphoSyntacticData* currentData = dataMap[*it];
      if (currentData == 0) continue;
      Token* currentToken= tokenMap[*it];

      /**
      * Le possessif prevaut sur le traitement des formes concatenees (it's).
      * En mode confiance, le 's est systematiquement traite comme un possessif lorsque
      * la forme n'a pas ete trouvee dans le dictionaire. En mode non confiance, on
      * laisse l'ambiguite
      */

      bool isSplitted=false;
      if (m_confidentMode)
      {
        // si la forme a deja ete trouvee dans le dictionnaire, il s'agit d'une
        // contraction connue, donc on ne fait rien
        if (currentData->size()>0) continue;
        // si il s'agit d'un possessif alors on traite le cas possessif.
        if (currentToken->status().isAlphaPossessive())
        {
          isSplitted=makePossessiveAlternativeFor(*it, graph, annotationData);
        }
        // On ne traite la forme comme une abbreviation, uniquement si elle n'a pas ete
        // traitee comme un possessif
        if ((!isSplitted) && currentToken->status().isAlphaConcatAbbrev())
        {
          isSplitted=makeConcatenatedAbbreviationSplitAlternativeFor(*it,graph, annotationData);
        }
      }
      else
      {
        // En mode non confiance, on effectue tous les traitements
        if (currentToken->status().isAlphaPossessive())
        {
          isSplitted=makePossessiveAlternativeFor(*it, graph, annotationData);
        }
        // On traite la forme comme une abbreviation, meme si elle a ete traitee comme un possessif
        if (currentToken->status().isAlphaConcatAbbrev())
        {
          isSplitted=makeConcatenatedAbbreviationSplitAlternativeFor(*it,graph, annotationData) || isSplitted;
        }
      }

      // si la forme a ete decoupee, alors on supprime le vertex d'origine
      if (isSplitted)
      {
        // unlink the previous vertex
        clear_vertex(*it,*graph);
      }

    }
  }
  catch (std::exception &exc)
  {
    MORPHOLOGINIT;
    LWARN << "Exception in AbbreviationSplitAlternatives : " << exc.what();
    return UNKNOWN_ERROR;
  }

  LINFO << "MorphologicalAnalysis: ending process AbbreviationSplitAlternatives";
  return SUCCESS_ID;
}


bool AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor(
  LinguisticGraphVertex splitted,
  LinguisticGraph* graph,
  AnnotationData* annotationData) const
{
  MORPHOLOGINIT;
  VertexTokenPropertyMap tokenMap = get( vertex_token,*graph );
  Token* ftok = tokenMap[splitted];
  const LimaString& ft = ftok->stringForm();
  LDEBUG << "AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor " << Common::Misc::limastring2utf8stdstring(ft);

  //int aposPos = ft.indexOf(Common::Misc::utf8stdstring2limastring("'"), 0);
  int aposPos = ft.indexOf(m_charSplitRegexp, 0);
  //LDEBUG << "AbbreviationSplitAlternatives: split chars found at " << aposPos;
  if (aposPos==-1 || aposPos==0) {
    return false;
  }
  LimaString beforeAbbrev(ft.left(aposPos-1));

  std::vector< LimaString >::const_iterator itAbb = m_abbreviations.begin();
  std::vector< LimaString >::const_iterator itAbb_end = m_abbreviations.end();
  LimaString abbrev;
  bool found = false;
  for (; itAbb != itAbb_end ; itAbb++)
  {
    abbrev = *itAbb;
    found = ft.endsWith(abbrev);
    if (found)
    {
      beforeAbbrev = ft.left(ft.size() - abbrev.size());
      break;
    }
  }
  if (!found) return false;

  // submit first string to Tokenizer
  LimaStringText* beforeAbbrevText=new LimaStringText(beforeAbbrev);
  AnalysisContent toTokenize;
  toTokenize.setData("Text",beforeAbbrevText);
  LimaStatusCode status=m_tokenizer->process(toTokenize);
  if (status != SUCCESS_ID) return false;
  AnalysisGraph* tokenizerList=static_cast<AnalysisGraph*>(toTokenize.getData("AnalysisGraph"));
  LinguisticGraph* tokGraph=tokenizerList->getGraph();

  // insert the first abreviated word
  uint64_t beginPos = ftok->position()-1;
  LinguisticGraphVertex firstToken=tokenizerList->firstVertex();
  {
    LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
    boost::tie(adjItr,adjItrEnd) = adjacent_vertices(firstToken,*tokGraph);
    if (adjItr==adjItrEnd)
    {
      MORPHOLOGINIT;
      LERROR << "AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor : no token forward !";
      throw LinguisticProcessingException("AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor : no token forward !");
    }
    firstToken=*adjItr;
  }

  FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
  Token* tokenizerToken = new Token(*(get(vertex_token,*tokGraph,firstToken)));
  MorphoSyntacticData* tokenizerData = new MorphoSyntacticData();
  tokenizerToken->setPosition(tokenizerToken->position() + beginPos);
  MorphoSyntacticDataHandler tokDataHandler(*tokenizerData,ABBREV_ALTERNATIVE);
  // read alternatives, but don't try concatenated
  m_reader->readAlternatives(
    *tokenizerToken,
    *m_dictionary,
    &tokDataHandler,
    0,
    &tokDataHandler);

  LinguisticGraphVertex beforeVertex = add_vertex(*graph);
  put(vertex_token,*graph,beforeVertex,tokenizerToken);
  put(vertex_data,*graph,beforeVertex,tokenizerData);

  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  annotationData->addMatching("AnalysisGraph", beforeVertex, "annot", agv);
  annotationData->annotate(agv, "AnalysisGraph", beforeVertex);


  // insert the second abreviated word
  StringsPoolIndex abbrevId=sp[abbrev];
  Token* newFT = new Token(abbrevId,abbrev,tokenizerToken->position()+tokenizerToken->length(),abbrev.size(),ftok->status());
  MorphoSyntacticData* newData = new MorphoSyntacticData();

  // retrieve ling infos for abbreviated word
  DictionaryEntry entry(m_dictionary->getEntry(newFT->form(),newFT->stringForm()));
  if (!entry.isEmpty())
  {
    MorphoSyntacticDataHandler newDataHandler(*newData,ABBREV_ALTERNATIVE);
    if (entry.hasLingInfos())
    {
      entry.parseLingInfos(&newDataHandler);
    }
    else
    {
    LERROR << "AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor: dictionary entry for abbreviated word " <<  Lima::Common::Misc::limastring2utf8stdstring(abbrev) << " has no linguistic info";
    }
  }
  else
  {
    LERROR << "AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor: Cannot find a dictionary entry for abbreviated word " <<  Lima::Common::Misc::limastring2utf8stdstring(abbrev);
  }
  if (newData->empty())
  {
    MORPHOLOGINIT;
    LERROR << "AbbreviationSplitAlternatives::makeConcatenatedAbbreviationSplitAlternativeFor Got empty morphosyntactic data. Abort.";
    delete newFT;
    delete newData;
    return false;
  }
//  LinguisticGraphVertex afterVertex = listIterator.createVertexFor(newFT);
  LinguisticGraphVertex afterVertex = add_vertex(*graph);
  put(vertex_token,*graph,afterVertex,newFT);
  VertexDataPropertyMap dataMap = get(vertex_data, *graph);
  dataMap[afterVertex] = newData;

  AnnotationGraphVertex agvafter =  annotationData->createAnnotationVertex();
  annotationData->addMatching("AnalysisGraph", afterVertex, "annot", agvafter);
  annotationData->annotate(agvafter, "AnalysisGraph", afterVertex);

  // links the first newly created vertex to the predecessors of the old vertex
  LinguisticGraphInEdgeIt itie, itie_end;
  boost::tie(itie, itie_end) = in_edges(splitted, *graph);
  for (; itie != itie_end; itie++)
  {
    add_edge(source(*itie,*graph), beforeVertex, *graph);
  }

  // links both newly created vertices
  add_edge(beforeVertex, afterVertex, *graph);

  // links the second newly created vertex to the succesors of the old vertex
  LinguisticGraphOutEdgeIt itoe, itoe_end;
  boost::tie(itoe, itoe_end) = out_edges(splitted, *graph);
  for (; itoe != itoe_end; itoe++)
  {
    add_edge(afterVertex, target(*itoe,*graph), *graph);
  }
  return true;
}

bool AbbreviationSplitAlternatives::makePossessiveAlternativeFor(
  LinguisticGraphVertex splitted,
  LinguisticGraph* graph,
  AnnotationData* annotationData) const
{
  MORPHOLOGINIT;
  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );
  Token* ftok = tokenMap[splitted];
  const LimaString& ft = ftok->stringForm();
  LDEBUG << "AbbreviationSplitAlternatives::makePossessiveAlternativeFor " << Common::Misc::limastring2utf8stdstring(ft);

  //int aposPos = ft.indexOf(LimaChar('\''), 0);
  int aposPos = ft.indexOf(m_charSplitRegexp, 0);
  if (aposPos==-1 || aposPos==0) return false;
  LimaString possessivedWord(ft.left(aposPos));
  LDEBUG << "AbbreviationSplitAlternatives::makePossessiveAlternativeFor possesive word: " << Common::Misc::limastring2utf8stdstring(possessivedWord);

  boost::regex pronounre("^(he|she|it|let)$", boost::regex::icase);
  if (boost::regex_match(Common::Misc::limastring2utf8stdstring(possessivedWord),pronounre))
  {
    return false;
  }

  // submit first string to Tokenizer
  LimaStringText* possessivedWordText=new LimaStringText(possessivedWord);
  AnalysisContent toTokenize;
  toTokenize.setData("Text",possessivedWordText);
  LimaStatusCode status=m_tokenizer->process(toTokenize);
  if (status != SUCCESS_ID)
  {
    LERROR << "AbbreviationSplitAlternatives::makePossessiveAlternativeFor: Failed to tokenize possesive word";
    return false;
  }
  AnalysisGraph* tokenizerList=static_cast<AnalysisGraph*>(toTokenize.getData("AnalysisGraph"));
  LinguisticGraph* tokGraph=tokenizerList->getGraph();

  // insert the first abreviated word
  uint64_t beginPos = ftok->position()-1;
  LinguisticGraphVertex firstToken=tokenizerList->firstVertex();
  {
    LinguisticGraphAdjacencyIt adjItr,adjItrEnd;
    boost::tie(adjItr,adjItrEnd) = adjacent_vertices(firstToken,*tokGraph);
    if (adjItr==adjItrEnd)
    {
      MORPHOLOGINIT;
      LERROR << "AbbreviationSplitAlternatives::makePossessiveAlternativeFor : no token forward !";
      throw LinguisticProcessingException("AbbreviationSplitAlternatives::makePossessiveAlternativeFor : no token forward !");
    }
    firstToken=*adjItr;
  }
  Token* tokenizerToken = new Token(*(get(vertex_token,*tokGraph,firstToken)));
  MorphoSyntacticData* tokenizerData = new MorphoSyntacticData();
  tokenizerToken->setPosition(tokenizerToken->position() + beginPos);
  tokenizerToken->status().setAlphaPossessive(true);
  MorphoSyntacticDataHandler tokDataHandler(*tokenizerData,ABBREV_ALTERNATIVE);
  m_reader->readAlternatives(
    *tokenizerToken,
    *m_dictionary,
    &tokDataHandler,  // linginfos
    0,                // Concat
    &tokDataHandler); // Accented

//  LinguisticGraphVertex possessivedVertex = listIterator.createVertexFor(tokenizerToken);
  LinguisticGraphVertex possessivedVertex = add_vertex(*graph);
  put(vertex_token,*graph,possessivedVertex,tokenizerToken);
  put(vertex_data,*graph,possessivedVertex,tokenizerData);

  AnnotationGraphVertex agvposs =  annotationData->createAnnotationVertex();
  annotationData->addMatching("AnalysisGraph", possessivedVertex, "annot", agvposs);
  annotationData->annotate(agvposs, "AnalysisGraph", possessivedVertex);

  // links the newly created vertex to the predecessors of the old vertex
  LinguisticGraphInEdgeIt itie, itie_end;
  boost::tie(itie, itie_end) = in_edges(splitted, *graph);
  for (; itie != itie_end; itie++)
  {
    add_edge(source(*itie,*graph), possessivedVertex, *graph);
  }

  // links the newly created vertex to the succesors of the old vertex
  LinguisticGraphOutEdgeIt itoe, itoe_end;
  boost::tie(itoe, itoe_end) = out_edges(splitted, *graph);
  for (; itoe != itoe_end; itoe++)
  {
    add_edge(possessivedVertex, target(*itoe,*graph), *graph);
  }

  return true;
}



} // closing namespace MorphologicAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
