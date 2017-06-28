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

//
// DefaultProperties is the implementation of the last module of
// Morphological Analysis. Each token from the main tokens
// path and alla recursive alternatives which remains without
// linguistic properties receives
// properties by default found into dictionnary using Tokenizer
// status names as keys.
//<if token has any linguistic properties, directly or by its
// orthographic alternatives, default properties are not applied>

#ifndef LIMA_LINGUISTICPROCESSING_SIMPLEDEFAULTPROPERTIES_H
#define LIMA_LINGUISTICPROCESSING_SIMPLEDEFAULTPROPERTIES_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer {
class CharChart;
}
namespace MorphologicAnalysis
{

#define SIMPLEDEFAULTPROPERTIES_CLASSID "SimpleDefaultProperties"

class LIMA_MORPHOLOGICANALYSIS_EXPORT SimpleDefaultProperties : public MediaProcessUnit
{

public:
  SimpleDefaultProperties();
  virtual ~SimpleDefaultProperties();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;

protected:

private:

  std::vector<LinguisticCode> m_defaultProperties;
  FlatTokenizer::CharChart* m_charChart;
  MediaId m_language;

  // Each token of the specified path without dictionnary entry is
  // searched into the specified dictionary.
  void affectPropertiesOnePath(
    Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph& tokenList) const;

};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
