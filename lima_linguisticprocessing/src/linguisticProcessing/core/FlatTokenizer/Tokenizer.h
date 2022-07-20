// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 24-JUL-2002
//
// Tokenizer is the main program of Tokenizer stuff.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Tokenizer_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_Tokenizer_H

#include "FlatTokenizerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

#define FLATTOKENIZER_CLASSID "FlatTokenizer"

class CharChart;

class TokenizerPrivate;

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
class LIMA_FLATTOKENIZER_EXPORT Tokenizer : public MediaProcessUnit
{

public:
  Tokenizer();
  virtual ~Tokenizer();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  const CharChart* charChart() const;
  CharChart* charChart();
  void setCharChart(CharChart* charChart);

  private:

    TokenizerPrivate* m_d;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
