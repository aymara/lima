// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    
    std::shared_ptr<FlatTokenizer::CharChart> m_charChart;
    std::set<LimaString> m_skipUnmarkStatus;
    MediaId m_language;
    std::map<LimaString,std::vector<LinguisticCode> > m_defaults;
    
    void readDefaultsFromFile(const std::string& filename);
};


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif  
