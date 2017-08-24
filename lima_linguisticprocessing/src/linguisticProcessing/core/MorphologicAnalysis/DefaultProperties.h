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
// jys 22-OCT-2002
//
// DefaultProperties is the implementation of the last module of
// Morphological Analysis. Each token from the main tokens 
// path and alla recursive alternatives which remains without 
// linguistic properties receives
// properties by default found into dictionnary using Tokenizer
// status names as keys.
//<if token has any linguistic properties, directly or by its 
// orthographic alternatives, default properties are not applied>

#ifndef DefaultProperties_H
#define DefaultProperties_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace FlatTokenizer {
class CharChart;
}
namespace AnalysisDict
{
class AbstractAnalysisDictionary;
}
namespace MorphologicAnalysis {

#define DEFAULTPROPERTIES_CLASSID "DefaultProperties"

class LIMA_MORPHOLOGICANALYSIS_EXPORT DefaultProperties : public MediaProcessUnit {

public:
    DefaultProperties();
    virtual ~DefaultProperties();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:
    
    FlatTokenizer::CharChart* m_charChart;
    std::set<LimaString> m_skipUnmarkStatus;
    MediaId m_language;
    std::map<LimaString,std::vector<LinguisticCode> > m_defaults;
    
    void readDefaultsFromFile(const std::string& filename);
};


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif  
