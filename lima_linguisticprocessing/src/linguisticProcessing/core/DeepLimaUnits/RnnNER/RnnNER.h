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


/** @brief This is a @ref MediaProcessUnit which detects named entities
*
* As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
*
* IOPES:
* - Output:
* - Preconditions:
* - Effects:
*
*/
class LIMA_RNNNER_EXPORT RnnNER : public MediaProcessUnit
{
public:
    RnnNER();
    virtual ~RnnNER() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

private:
    RnnNERPrivate* m_d;
};

} // namespace Lima

#endif
