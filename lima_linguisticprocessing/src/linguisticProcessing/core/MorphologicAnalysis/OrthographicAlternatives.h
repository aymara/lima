// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 25-NOV-2002
//
// OrthographicAlternatives is the module which creates alternatives
// Each token from the supplied tokens path is processed.
// There are 2 modes :
// o confident mode : only tokens unknown into dictionnary as simple word
//   are processed
// o unconfident mode : all tokens are processed.

#ifndef OrthographicAlternatives_H
#define OrthographicAlternatives_H

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

#define ORTHOGRAPHALTERNATIVES_CLASSID "OrthographicAlternatives"

class OrthographicAlternatives : public MediaProcessUnit {

public:
    OrthographicAlternatives();
    virtual ~OrthographicAlternatives();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;

  static void setOrthographicAlternatives(
    LinguisticAnalysisStructure::Token* token,
    LinguisticAnalysisStructure::MorphoSyntacticData* tokenData,
    AnalysisDict::AbstractAnalysisDictionary* dictionary,
    FlatTokenizer::CharChart* charChart,
    FsaStringsPool& sp);
    
  static void createAlternative(
    LinguisticAnalysisStructure::Token* srcToken,
    LinguisticAnalysisStructure::MorphoSyntacticData* tokenData,
    LimaString& str,
    AnalysisDict::AbstractAnalysisDictionary* dictionary,
    FsaStringsPool& sp);
    
        
protected:

private:
    
    AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
    FlatTokenizer::CharChart* m_charChart;
    bool m_confidentMode;
    MediaId m_language;
    
    
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima


#endif  
