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

#include "HunspellSpellingAlternatives.h"
#include "MorphoSyntacticDataHandler.h"

#include "common/time/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"

#include <hunspell/hunspell.hxx>

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::AnalysisDict;
using namespace Lima::LinguisticProcessing::FlatTokenizer;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

  SimpleFactory<MediaProcessUnit,HunspellSpellingAlternatives> hunspellSpellingAlternativesFactory(HUNSPELL_SPELLING_ALTERNATIVES_CLASSID);


class HunspellSpellingAlternativesPrivate
{
  friend class HunspellSpellingAlternatives;

public:
  HunspellSpellingAlternativesPrivate() : m_hunspell(0) {}
  virtual ~HunspellSpellingAlternativesPrivate() {delete m_hunspell;}
  
  
  void setHunspellSpellingAlternatives(
    LinguisticAnalysisStructure::Token* token,
    LinguisticAnalysisStructure::MorphoSyntacticData* tokenData,
    FsaStringsPool& sp);
  
  AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
  MediaId m_language;
  Hunspell* m_hunspell;
};


HunspellSpellingAlternatives::HunspellSpellingAlternatives() : m_d(new HunspellSpellingAlternativesPrivate())
{
  
}

HunspellSpellingAlternatives::~HunspellSpellingAlternatives()
{
  delete m_d;
}

void HunspellSpellingAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  LDEBUG << "HunspellSpellingAlternatives::init";  std::string spellDico;
  m_d->m_language = manager->getInitializationParameters().media;

  try
  {
    // try to get a specific spellchecking dictionary name from the config file
    spellDico = unitConfiguration.getParamsValueAtKey("spellcheckDictionary");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'spellcheckDictionary' in HunspellSpellingAlternatives group for language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }
  LDEBUG << "HunspellSpellingAlternatives::init requesting Hunspell spellcheck dictionary" << Common::MediaticData::MediaticData::changeable().getResourcesPath()+"/Spellchecking/" << spellDico;
  if (m_d->m_hunspell != 0) delete  m_d->m_hunspell;
  m_d->m_hunspell = new Hunspell( (Common::MediaticData::MediaticData::changeable().getResourcesPath()+"/Spellchecking/"+spellDico+".aff").c_str(),
    (Common::MediaticData::MediaticData::changeable().getResourcesPath()+"/Spellchecking/"+spellDico+".dic").c_str() );
  try
  {
    std::string dico=unitConfiguration.getParamsValueAtKey("dictionary");
    AbstractResource* res= LinguisticResources::single().getResource(m_d->m_language,dico);
    m_d->m_dictionary=static_cast<AbstractAnalysisDictionary*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in HunspellSpellingAlternatives group for language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }
}


LimaStatusCode HunspellSpellingAlternatives::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process HunspellSpellingAlternatives";
  
  FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();
  VertexDataPropertyMap dataMap=get(vertex_data,*g);
  VertexTokenPropertyMap tokenMap=get(vertex_token,*g);
  LinguisticGraphVertexIt it,itEnd;
  for (boost::tie(it,itEnd)=vertices(*g) ; it != itEnd ; it++)
  {
    LDEBUG << "HunspellSpellingAlternatives::process processing vertex " << *it;
    Token* currentToken=tokenMap[*it];
    MorphoSyntacticData* msd=dataMap[*it];
    
    if (currentToken!=0)
    {
      if (msd->empty())
      {
        m_d->setHunspellSpellingAlternatives(
          currentToken,
          msd,
          sp);
      }
    }
  }
  LINFO << "MorphologicalAnalysis: ending process HunspellSpellingAlternatives";
  return SUCCESS_ID;
}

void HunspellSpellingAlternativesPrivate::setHunspellSpellingAlternatives(
  Token* token,
  MorphoSyntacticData* tokenData,
  FsaStringsPool& sp)
{
  // try to find simple Uncapitalization
  MORPHOLOGINIT;
  // FIXME Conditions below could be process unit parameters
  const LimaString& tokenStr=token->stringForm();
  if (token->status().getAlphaCapital() == T_CAPITAL
    || token->status().getAlphaCapital() == T_CAPITAL_1ST
    || token->status().getAlphaCapital() == T_CAPITAL_SMALL
    || token->status().isAlphaConcatAbbrev()
    || token->status().isAlphaHyphen()
    || token->status().isAlphaPossessive()
    || tokenStr.toUpper() == tokenStr)
  {
    return;
  }
  char **suggestions;
  int suggestResult = m_hunspell->suggest(&suggestions, tokenStr.toUtf8().constData());
  for (int i = 0; i < suggestResult;    i++)
  {
    LimaString correction = LimaString::fromUtf8(suggestions[i]);
    // FIXME Conditions below could be process unit parameters
    if ( correction.size() > 1 && correction != tokenStr )
    {
      LDEBUG << "HunspellSpellingAlternativesPrivate::setHunspellSpellingAlternatives trying to correct" << tokenStr << "into" << correction;
      DictionaryEntry entry (m_dictionary->getEntry(correction));
      MorphoSyntacticDataHandler lingInfosHandler(*tokenData, SPELLING_ALTERNATIVE);
      
      
//       if (!entry.isEmpty())
      {
        LINFO << "HunspellSpellingAlternativesPrivate::setHunspellSpellingAlternatives correcting" << tokenStr << "into" << correction;
        // add orthographic alternative to Token;
        StringsPoolIndex idx=sp[correction];
        token->addOrthographicAlternatives(idx);
        
        if (entry.hasLingInfos())
        {
          entry.parseLingInfos(&lingInfosHandler);
        }
      } 
//       else 
//       {
//         LDEBUG << "HunspellSpellingAlternativesPrivate::setHunspellSpellingAlternatives correction" << correction << "not found in the dictionary";
//         delete entry;
//       }
    }
  }
  m_hunspell->free_list(&suggestions, suggestResult);
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima



