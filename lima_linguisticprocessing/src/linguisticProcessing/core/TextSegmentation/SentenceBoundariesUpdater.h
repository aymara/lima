/*
    Copyright 2002-2020 CEA LIST

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
