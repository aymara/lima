// Copyright 2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
