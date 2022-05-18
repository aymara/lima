// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_CppUppsalaTensorFlowTokenizer_Tokenizer_H
#define LIMA_LINGUISTICPROCESSING_CppUppsalaTensorFlowTokenizer_Tokenizer_H

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

#define CPPUPPSALATENSORFLOWTOKENIZER_CLASSID "CppUppsalaTensorFlowTokenizer"

class CharChart;

class CppUppsalaTokenizerPrivate;

/** @brief This is a @ref MediaProcessUnit that is usually the first element of the pipeline. It cuts the input text into tokens
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Output:          an AnalysisContent
  * - Preconditions:   the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"
  * - Effects:         the AnalysisContent will contain an AnalysisData of type AnalysisGraph
  *                    named "AnalysisGraph" which is a linear graph (a string) containing one vertex
  *                    for each detected token.
  */
class LIMA_TENSORFLOWTOKENIZER_EXPORT CppUppsalaTensorFlowTokenizer : public MediaProcessUnit
{
public:
  CppUppsalaTensorFlowTokenizer();
  virtual ~CppUppsalaTensorFlowTokenizer();

  void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  CppUppsalaTokenizerPrivate* m_d;
};

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima

#endif
