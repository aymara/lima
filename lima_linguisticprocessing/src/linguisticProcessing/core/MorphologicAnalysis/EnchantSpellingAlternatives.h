// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ENCHANT_SPELLING_ALTERNATIVES_H
#define ENCHANT_SPELLING_ALTERNATIVES_H

#include "common/Data/LimaString.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{
  class CharChart;
}
namespace LinguisticAnalysisStructure
{
  class MorphoSyntacticData;
}
namespace AnalysisDict
{
  class AbstractAnalysisDictionary;
}
namespace MorphologicAnalysis
{

#define ENCHANT_SPELLING_ALTERNATIVES_CLASSID "EnchantSpellingAlternatives"
class EnchantSpellingAlternativesPrivate;
class EnchantSpellingAlternatives : public MediaProcessUnit {

public:
    EnchantSpellingAlternatives();
    virtual ~EnchantSpellingAlternatives();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

private:
  EnchantSpellingAlternativesPrivate* m_d;
  
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima


#endif  // ENCHANT_SPELLING_ALTERNATIVES_H
