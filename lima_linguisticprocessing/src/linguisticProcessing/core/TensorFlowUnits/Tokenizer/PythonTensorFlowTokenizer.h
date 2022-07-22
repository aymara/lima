// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 24-JUL-2002
//
// PythonTensorFlowTokenizer is the main program of PythonTensorFlowTokenizer stuff.

#ifndef LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_Tokenizer_H
#define LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_Tokenizer_H

#include "TensorFlowTokenizerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Tokenizer
{

#define PYTHONTENSORFLOWTOKENIZER_CLASSID "PythonTensorFlowTokenizer"

class CharChart;

class PythonTokenizerPrivate;

/** @brief This is a @ref MediaProcessUnit that is usually the first element of the pipeline. It cuts the input text into tokens
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Input: an AnalysisContent and the following parameters in the configuration file:
  *   - charChart: the name of a group in the Resources module. This defines a resource of class TensorFlowTokenizerCharChart with a parameter named charFile given the path to the chars chart file to use, relative to the main resources folder
  *   - automatonFile: the path to the tokenization automaton file to use, relative to the main resources folder
  * - Output: an AnalysisContent
  * - Preconditions: the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"
  * - Effects: the AnalysisContent will contain an AnalysisData of type AnalysisGraph named "AnalysisGraph" which is a linear graph (a string) containing one vertex for each detected token.
  */
class LIMA_TENSORFLOWTOKENIZER_EXPORT PythonTensorFlowTokenizer : public MediaProcessUnit
{

public:
  PythonTensorFlowTokenizer();
  virtual ~PythonTensorFlowTokenizer();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  private:

    PythonTokenizerPrivate* m_d;
};

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima

#endif
