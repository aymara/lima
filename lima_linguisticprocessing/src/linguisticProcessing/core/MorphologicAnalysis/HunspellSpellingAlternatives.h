/*
    Copyright 2015 CEA LIST

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

#ifndef HUNSPELL_SPELLING_ALTERNATIVES_H
#define HUNSPELL_SPELLING_ALTERNATIVES_H

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

#define HUNSPELL_SPELLING_ALTERNATIVES_CLASSID "HunspellSpellingAlternatives"
class HunspellSpellingAlternativesPrivate;
class HunspellSpellingAlternatives : public MediaProcessUnit {

public:
    HunspellSpellingAlternatives();
    virtual ~HunspellSpellingAlternatives();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  HunspellSpellingAlternativesPrivate* m_d;
  
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima


#endif  // HUNSPELL_SPELLING_ALTERNATIVES_H
