// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_LangDetector_H
#define LIMA_LINGUISTICPROCESSING_LangDetector_H

#include "LangDetectorExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima::LinguisticProcessing::LDetector
{

#define LANGDETECTOR_CLASSID "LangDetector"

class CharChart;

class LangDetectorCore;

/** @brief This is a @ref MediaProcessUnit that is usually the first element of the pipeline. It cuts the input text into tokens
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Input: an AnalysisContent and the following parameters in the configuration file:
  *   - charChart: the name of a group in the Resources module. This defines a resource of class FlatTokenizerCharChart with a parameter named charFile given the path to the chars chart file to use, relative to the main resources folder
  *   - automatonFile: the path to the tokenization automaton file to use, relative to the main resources folder
  * - Output: an AnalysisContent
  * - Preconditions: the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"
  * - Effects: the AnalysisContent will contain an AnalysisData of type AnalysisGraph named "AnalysisGraph" which is a linear graph (a string) containing one vertex for each detected token.
  */
class LIMA_LANGDETECTOR_EXPORT LangDetector : public MediaProcessUnit
{

public:
  LangDetector();
  virtual ~LangDetector();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  private:
    LangDetectorCore* m_d;
};

} // namespace Lima

#endif
