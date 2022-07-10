// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_Tokenizer_RnnTokenizer_H
#define LIMA_LINGUISTICPROCESSING_Tokenizer_RnnTokenizer_H

#include "RnnTokenizerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace DeepLimaUnits
{
namespace RnnTokenizer
{

#define RNNTOKENIZER_CLASSID "RnnTokenizer"

class CharChart;

class RnnTokenizerPrivate;

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
class LIMA_RNNTOKENIZER_EXPORT RnnTokenizer : public MediaProcessUnit
{
public:
  RnnTokenizer();
  virtual ~RnnTokenizer();

  void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  RnnTokenizerPrivate* m_d;
};

} // namespace RnnTokenizer
} // namespace DeepLimaUnits
} // namespace LinguisticProcessing
} // namespace Lima

#endif
