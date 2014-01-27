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
