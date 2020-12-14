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

#include "EnchantSpellingAlternatives.h"
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

#include <enchant/enchant++.h>

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

  SimpleFactory<MediaProcessUnit,EnchantSpellingAlternatives> enchantSpellingAlternativesFactory(ENCHANT_SPELLING_ALTERNATIVES_CLASSID);


class EnchantSpellingAlternativesPrivate
{
  friend class EnchantSpellingAlternatives;

public:
  EnchantSpellingAlternativesPrivate() {}
  virtual ~EnchantSpellingAlternativesPrivate() {delete m_enchantDictionary;}


  void setEnchantSpellingAlternatives(
    LinguisticAnalysisStructure::Token* token,
    LinguisticAnalysisStructure::MorphoSyntacticData* tokenData,
    FsaStringsPool& sp);

  AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
  MediaId m_language;
  enchant::Dict* m_enchantDictionary;
};


EnchantSpellingAlternatives::EnchantSpellingAlternatives() : m_d(new EnchantSpellingAlternativesPrivate())
{

}

EnchantSpellingAlternatives::~EnchantSpellingAlternatives()
{
  delete m_d;
}

void EnchantSpellingAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  m_d->m_language = manager->getInitializationParameters().media;
  try
  {
    m_d->m_enchantDictionary = enchant::Broker::instance()->request_dict(Common::MediaticData::MediaticData::changeable().getMediaId(m_d->m_language).substr(0,2));
  }
  catch (enchant::Exception& e)
  {
    LIMA_EXCEPTION( "Cannot get Enchant dictionary for language"
      << Common::MediaticData::MediaticData::changeable().getMediaId(m_d->m_language).c_str() );
  }
  try
  {
    std::string dico=unitConfiguration.getParamsValueAtKey("dictionary");
    AbstractResource* res= LinguisticResources::single().getResource(m_d->m_language,dico);
    m_d->m_dictionary=static_cast<AbstractAnalysisDictionary*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in EnchantSpellingAlternatives group for language " << (int) m_d->m_language;
    throw InvalidConfiguration("no param 'dictionary' in EnchantSpellingAlternatives group for language ");
  }
}


LimaStatusCode EnchantSpellingAlternatives::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  MORPHOLOGINIT;
  LINFO << "MorphologicalAnalysis: starting process EnchantSpellingAlternatives";

  FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();
  VertexDataPropertyMap dataMap=get(vertex_data,*g);
  VertexTokenPropertyMap tokenMap=get(vertex_token,*g);
  LinguisticGraphVertexIt it,itEnd;
  for (boost::tie(it,itEnd)=vertices(*g) ; it != itEnd ; it++)
  {
    LDEBUG << "EnchantSpellingAlternatives::process processing vertex " << *it;
    Token* currentToken=tokenMap[*it];
    MorphoSyntacticData* msd=dataMap[*it];

    if (currentToken!=0)
    {
      if (msd->empty())
      {
        m_d->setEnchantSpellingAlternatives(
          currentToken,
          msd,
          sp);
      }
    }
  }
  LINFO << "MorphologicalAnalysis: ending process EnchantSpellingAlternatives";
  return SUCCESS_ID;
}

void EnchantSpellingAlternativesPrivate::setEnchantSpellingAlternatives(
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
  std::vector<std::string> suggestions = m_enchantDictionary->suggest(tokenStr.toUtf8().constData());
  for (std::vector<std::string>::const_iterator it = suggestions.begin(); it != suggestions.end();it++)
  {
    LimaString correction = LimaString::fromUtf8((*it).c_str());
    // FIXME Conditions below could be process unit parameters
    if ( correction.size() > 1 && correction != tokenStr )
    {
      DictionaryEntry* entry = new DictionaryEntry(m_dictionary->getEntry(correction));
      MorphoSyntacticDataHandler lingInfosHandler(*tokenData, SPELLING_ALTERNATIVE);


      if (!entry->isEmpty())
      {
        LINFO << "EnchantSpellingAlternativesPrivate::setEnchantSpellingAlternatives correcting" << tokenStr << "into" << correction;
        // add orthographic alternative to Token;
        StringsPoolIndex idx=sp[correction];
        token->addOrthographicAlternatives(idx);

        if (entry->hasLingInfos())
        {
          entry->parseLingInfos(&lingInfosHandler);
        }
      }
      else
      {
        delete entry;
      }
    }
  }
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima



