// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_Tagger_RnnNER_H
#define LIMA_LINGUISTICPROCESSING_Tagger_RnnNER_H

#include "RnnNERExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnNER
{

#define RNNNER_CLASSID "RnnNER"

class CharChart;

class RnnNERPrivate;
#define RNNTAGGER_CLASSID "RnnNER"

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

/**
 * @class BadFileException
 * @brief Class exception
 */
    class BadFileException : public std::logic_error {
    public:
        explicit BadFileException(const std::string& s= "") :
                std::logic_error(s){}
        ~BadFileException() noexcept override = default;
    };

class LIMA_RNNNER_EXPORT RnnNER : public MediaProcessUnit
{
public:
    RnnNER();
    ~RnnNER() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

private:
    RnnNERPrivate* m_d;
};

} // namespace Lima

#endif
