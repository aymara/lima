// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 25-NOV-2002
//
// OrthographicAlternatives is the module which creates alternatives
// for given tokens. It "unmarks" the string then searchs into dictionnary
// with this new entry for new reaccented words.
// Each token from the supplied tokens path is processed.
// There are 2 modes :
// o confident mode : only tokens unknown into dictionnary as simple word
//   are processed
// o unconfident mode : all tokens are processed.

#include "OrthographicAlternatives.h"

// #include "common/linguisticData/linguisticData.h"
#include "common/misc/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::AnalysisDict;
using namespace Lima::LinguisticProcessing::FlatTokenizer;
using namespace Lima::Common::LinguisticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<LinguisticProcessUnit,OrthographicAlternatives> orthographicAlternativeFactory(ORTHOGRAPHALTERNATIVES_CLASSID);

OrthographicAlternatives::OrthographicAlternatives()
{}

OrthographicAlternatives::~OrthographicAlternatives()
{}

void OrthographicAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  m_language = manager->getInitializationParameters().language;
  try
  {
    string dico=unitConfiguration.getParamsValueAtKey("dictionary");
    AbstractResource* res= LinguisticResources::single().getResource(m_language,dico);
    m_dictionary=static_cast<AbstractAnalysisDictionary*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in OrthographicAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  try
  {
    string dico=unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res= LinguisticResources::single().getResource(m_language,dico);
    m_charChart=static_cast<CharChart*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'charChart' in OrthographicAlternatives group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  try
  {
    string confident=unitConfiguration.getParamsValueAtKey("confidentMode");
    m_confidentMode=(confident=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "no param 'confidentMode' in OrthographicAlternatives group for language " << (int) m_language;
    LWARN << "use default value : 'true'";
    m_confidentMode=true;
  }

}


LimaStatusCode OrthographicAlternatives::process(
  AnalysisContent& analysis) const
{

  TimeUtils::updateCurrentTime();
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process OrthographicAlternatives";

  StringsPool& sp=Common::LinguisticData::LinguisticData::changeable().stringsPool(m_language);
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();
  LinguisticGraphVertexIt it,itEnd;
  VertexDataPropertyMap dataMap=get(vertex_data,*g);
  VertexTokenPropertyMap tokenMap=get(vertex_token,*g);
  boost::tie(it,itEnd)=vertices(*g);
  for (;it!=itEnd;it++)
  {
    LDEBUG << "processing vertex " << *it;
    MorphoSyntacticData* currentTokenData=dataMap[*it];
    Token* tok=tokenMap[*it];
    if (currentTokenData!=0)
    {

      // if in confidentMode and token has already ling infos, skip
      if ( m_confidentMode && (currentTokenData->size()>0) ) continue;

      // set orthographic alternatives given by dictionary
      // using the alternatives directly given by the morphosyntactic data
      {
        LDEBUG << "processing alternatives from dico";
        DictionaryEntry* entry=tok->dictionaryEntry();
        entry->reset();
        if (entry->hasAccented()) {
          LimaString oa = entry->nextAccented();
          while ( oa.size() > 0 )
          {
            createAlternative(tok,currentTokenData,oa,m_dictionary,sp);
            oa = entry->nextAccented();
          }
        }
      }

      // if in confidentMode and token has already ling infos, skip
      if (m_confidentMode && (currentTokenData->size() > 0) ) continue;

      // if no ling infos, then lower and unmark string
      LDEBUG << "set unmark alternatives";
      setOrthographicAlternatives(
        tok,
        currentTokenData,
        m_dictionary,
        m_charChart,
        sp);
    }
  }
  LINFO << "MorphologicalAnalysis: ending process OrthographicAlternatives";
  TimeUtils::logElapsedTime("OrthographicAlternatives");
  return SUCCESS_ID;
}

void OrthographicAlternatives::setOrthographicAlternatives(
  Token* token,
  MorphoSyntacticData* tokenData,
  AnalysisDict::AbstractAnalysisDictionary* dictionary,
  CharChart* charChart,
  StringsPool& sp)
{
  // try to find simple Uncapitalization
  MORPHOLOGINIT;
  const LimaString& tokenStr=token->stringForm();
  LimaString lowerWord = charChart->toLower(tokenStr);
  if (!(lowerWord == "") && !(lowerWord == tokenStr) )
  {
    LDEBUG << "createAlternative for lowerWord " << lowerWord;
    createAlternative(token,tokenData,lowerWord,dictionary,sp);
  }
  if (tokenData->size()>0)
  {
    return;
  }

  // desaccent token string
  LimaString unmarked=charChart->unmark(tokenStr);
  if (!(unmarked=="") && !(unmarked==tokenStr))
  {
    LDEBUG << "createAlternative for unmarked " << unmarked;
    createAlternative(token,tokenData,unmarked,dictionary,sp);
  }

}


void OrthographicAlternatives::createAlternative(
  Token* srcToken,
  MorphoSyntacticData* tokenData,
  LimaString& str,
  AnalysisDict::AbstractAnalysisDictionary* dictionary,
  StringsPool& sp)
{
  MORPHOLOGINIT;
  LDEBUG << "OrthographicAlternatives::createAlternative" << str;
  DictionaryEntry* dicoEntry = new DictionaryEntry(dictionary->getEntry(str));
  if (!dicoEntry->isEmpty())
  {
    // add orthographic alternative to Token;
    StringsPoolIndex infl=sp[str];
    Token* altToken=new Token(infl,str,srcToken->position(),srcToken->length(),new TStatus(*(srcToken->status())));
    altToken->setDictionaryEntry(dicoEntry);
    srcToken->addOrthographicAlternative(altToken);
  
    tokenData->appendLingInfo(infl,dicoEntry,ORTHOGRAPHIC_ALTERNATIVE,sp);

    // if entry has other accented forms,
    // keep them ("PARIS" -> "paris" -> "Paris")
    if (dicoEntry->hasAccented())
    {
      dicoEntry->reset();
      Lima::LimaString alternativeStr = dicoEntry->nextAccented();
      while (alternativeStr.size() != 0)
      {
        // give it its simple word entry into dictionary
        DictionaryEntry* altDicoEntry = new DictionaryEntry(dictionary->getEntry(alternativeStr));
        StringsPoolIndex infl2=sp[alternativeStr];
        tokenData->appendLingInfo(infl2,altDicoEntry,ORTHOGRAPHIC_ALTERNATIVE,sp);
        
        // add orthographic alternative to Token
        Token* altToken2=new Token(infl2,alternativeStr,srcToken->position(),srcToken->length(),new TStatus(*(srcToken->status())));
        altToken2->setDictionaryEntry(altDicoEntry);
        srcToken->addOrthographicAlternative(altToken2);
        
        alternativeStr = dicoEntry->nextAccented();
      }
    }
  } else {
    delete dicoEntry;
  }
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
