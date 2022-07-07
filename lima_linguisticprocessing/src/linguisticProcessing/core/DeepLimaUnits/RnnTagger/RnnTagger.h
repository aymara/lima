// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_Tagger_RnnTagger_H
#define LIMA_LINGUISTICPROCESSING_Tagger_RnnTagger_H

#include "RnnTaggerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTagger
{

#define RNNTagger_CLASSID "RnnTagger"

class CharChart;

class RnnTaggerPrivate;
#define RNNTAGGER_CLASSID "RnnTagger"

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
class LIMA_RNNTAGGER_EXPORT RnnTagger : public MediaProcessUnit
{
public:
    RnnTagger();
    ~RnnTagger() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

private:
    RnnTaggerPrivate* m_d;
};

} // namespace Lima

#endif
