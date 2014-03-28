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
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  EnchantSpellingAlternativesPrivate* m_d;
  
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima


#endif  // ENCHANT_SPELLING_ALTERNATIVES_H
