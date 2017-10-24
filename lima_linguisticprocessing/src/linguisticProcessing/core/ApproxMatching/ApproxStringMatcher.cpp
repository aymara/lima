/*
    Copyright 2002-2017 CEA LIST

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

#include "ApproxStringMatcher.h"

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
#include "common/misc/AbstractAccessIterators.h"
#include "common/Data/strwstrtools.h"
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

SimpleFactory<MediaProcessUnit,ApproxStringMatcher> ApproxStringMatcherFactory(APPROX_STRING_MATCHER_CLASSID);

ApproxStringMatcher::ApproxStringMatcher() :
    m_nbMaxError(0)
{}

ApproxStringMatcher::~ApproxStringMatcher()
{
  // delete m_reader;
}

void ApproxStringMatcher::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  MediaId language = manager->getInitializationParameters().media;
  // m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);
  string dico;
  try
  {
    dico=unitConfiguration.getParamsValueAtKey("dictionary");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }

  AbstractResource* res=LinguisticResources::single().getResource(language,dico);
  AbstractAccessResource* lexicon = lexicon=static_cast<AbstractAccessResource*>(res);
  m_lexicon = lexicon->getAccessByString();
}


LimaStatusCode ApproxStringMatcher::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("ApproxStringMatcher");
  MORPHOLOGINIT;
  LINFO << "starting process ApproxStringMatcher";

  // Initalize list of suggestions, ordered by number of errors
  std::multimap<int,Suggestion> result;
  
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));

  LinguisticGraph* g=tokenList->getGraph();
  matchExactTokenAndFollowers(*g, result);

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LINFO << "ApproxStringMatcher::process no annotation data, creating and populating it";
    annotationData=new AnnotationData();
    analysis.setData("AnnotationData",annotationData);
  }
  tokenList->populateAnnotationGraph(annotationData, "AnalysisGraph");
  
#ifdef DEBUG_LP
  LDEBUG << "ending process ApproxStringMatcher";
#endif
  return SUCCESS_ID;
}

LimaStatusCode ApproxStringMatcher::matchExactTokenAndFollowers(LinguisticGraph& g, std::multimap<int,Suggestion>& result) const
{
  MORPHOLOGINIT;
  VertexTokenPropertyMap tokenMap=get(vertex_token,g);
  // VertexDataPropertyMap dataMap=get(vertex_data,g);
  LinguisticGraphVertexIt it,itEnd;
  boost::tie(it,itEnd)=vertices(g);
  for (;it!=itEnd;it++)
  {
    Token* currentToken=tokenMap[*it];
    if (currentToken!=0)
    {
      Lima:LimaString form = currentToken->stringForm();
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchTokenAndFollowers() for token" << Lima::Common::Misc::limastring2utf8stdstring(form);
#endif
      int position = currentToken->position();
      // currentToken->status()?
      typedef Lima::Common::AccessSuperWordIterator WIt;
      std::pair<WIt,WIt>  wordsIt = m_lexicon->getSuperWords(form);
      for( ; wordsIt.first != wordsIt.second ; wordsIt.first++ )
      {
#ifdef DEBUG_LP
      Lima::LimaString word = *(wordsIt.first);
      LDEBUG << "ApproxStringMatcher::matchTokenAndFollowers() from lexicon:" <<
      Lima::Common::Misc::limastring2utf8stdstring(word);
#endif
      }
    }
  }
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
