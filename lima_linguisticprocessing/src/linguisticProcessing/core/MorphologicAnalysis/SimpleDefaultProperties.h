// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

  std::vector<LinguisticCode> m_defaultProperties;
  std::shared_ptr<FlatTokenizer::CharChart> m_charChart;
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
