// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_Lemmatizer_RnnTokensAnalyzer_H
#define LIMA_LINGUISTICPROCESSING_Lemmatizer_RnnTokensAnalyzer_H

#include "RnnTokensAnalyzerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTokensAnalyzer
{

class CharChart;

class RnnTokensAnalyzerPrivate;

#define RNNTOKENSANALYZER_CLASSID "RnnTokensAnalyzer"

/**
  * @brief This is a @ref MediaProcessUnit which executes deeplima RNN-based PoS tagger and lemmatizer.
  *
  * The text is supposed to be already tokenized.
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Output:
  * - Preconditions:
  * - Effects:
  * @author Tristan Derouet
*/
class LIMA_RNNTOKENSANALYZER_EXPORT RnnTokensAnalyzer : public MediaProcessUnit
{
    public:
    RnnTokensAnalyzer();
    ~RnnTokensAnalyzer() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

    private:
        RnnTokensAnalyzerPrivate* m_d;
};

} // namespace Lima

#endif
