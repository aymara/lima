// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_TextSegmentation_SentenceBoundariesUpdater_H
#define LIMA_LINGUISTICPROCESSING_TextSegmentation_SentenceBoundariesUpdater_H

#include "TextSegmentationExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace TextSegmentation
{
namespace SentenceBoundariesUpdater
{

#define SENTENCEBOUNDARIESUPDATER_CLASSID "SentenceBoundariesUpdater"

class SentenceBoundariesUpdaterPrivate;

/** @brief This is a @ref MediaProcessUnit that does ...
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Input:
  * - Output:
  * - Preconditions:
  * - Effects:
  */
class LIMA_TEXTSEGMENTATION_EXPORT SentenceBoundariesUpdater : public MediaProcessUnit
{

public:
  SentenceBoundariesUpdater();
  virtual ~SentenceBoundariesUpdater();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  SentenceBoundariesUpdaterPrivate* m_d;
};

} // namespace SentenceBoundariesUpdater
} // namespace TextSegmentation
} // namespace LinguisticProcessing
} // namespace Lima

#endif
