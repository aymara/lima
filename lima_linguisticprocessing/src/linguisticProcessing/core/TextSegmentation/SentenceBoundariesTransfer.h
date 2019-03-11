/*
    Copyright 2019 CEA LIST

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
#ifndef LIMA_LINGUISTICPROCESSING_GRAPHSENTENCEBOUNDSTRANSFER_H
#define LIMA_LINGUISTICPROCESSING_GRAPHSENTENCEBOUNDSTRANSFER_H

#include "TextSegmentationExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

#include <deque>

namespace Lima
{

namespace LinguisticProcessing
{

namespace LinguisticAnalysisStructure
{

#define SENTENCEBOUNDARIESTRANSFER_CLASSID "SentenceBoundariesTransfer"

class SentenceBoundariesTransferPrivate;

/**
 * @brief The pipeline unit transfers sentence boundaries found on the
 * AnalysisGraph to tho PosGraph as requested by following pipeline units
 *
 * @author Gaël de Chalendar <gael.de-chalendar@cea.fr>
*/
class LIMA_TEXTSEGMENTATION_EXPORT SentenceBoundariesTransfer : public MediaProcessUnit
{
friend class SentenceBoundariesTransferPrivate;

public:
  SentenceBoundariesTransfer();

  virtual ~SentenceBoundariesTransfer();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  SentenceBoundariesTransferPrivate* m_d;
};

}

}

}

#endif
