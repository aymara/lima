// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_GRAPHSENTENCEBOUNDSFINDER_H
#define LIMA_LINGUISTICPROCESSING_GRAPHSENTENCEBOUNDSFINDER_H

#include "TextSegmentationExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

#include <deque>

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

#define PARAGRAPHBOUNDARIESFINDER_CLASSID "ParagraphBoundariesFinder"

/**
@author Benoit Mathieu
*/
class LIMA_TEXTSEGMENTATION_EXPORT ParagraphBoundariesFinder : public MediaProcessUnit
{
public:
  ParagraphBoundariesFinder();

  virtual ~ParagraphBoundariesFinder();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  std::list<LinguisticCode> m_stopCategories;
//   const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  std::string m_graph;
  LimaString m_paragraphSeparator;
  std::string m_dataName;
};

}

}

}

#endif
