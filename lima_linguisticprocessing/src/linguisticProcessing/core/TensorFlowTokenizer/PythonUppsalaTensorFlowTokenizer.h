/*
    Copyright 2002-2019 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_PythonUppsalaTensorFlowTokenizer_Tokenizer_H
#define LIMA_LINGUISTICPROCESSING_PythonUppsalaTensorFlowTokenizer_Tokenizer_H

#include "TensorFlowTokenizerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowTokenizer
{

#define PYTHONUPPSALATENSORFLOWTOKENIZER_CLASSID "PythonUppsalaTensorFlowTokenizer"

class CharChart;

class PythonUppsalaTokenizerPrivate;

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
class LIMA_TENSORFLOWTOKENIZER_EXPORT PythonUppsalaTensorFlowTokenizer : public MediaProcessUnit
{

public:
  PythonUppsalaTensorFlowTokenizer();
  virtual ~PythonUppsalaTensorFlowTokenizer();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  private:

    PythonUppsalaTokenizerPrivate* m_d;
};

} // namespace TensorFlowTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
