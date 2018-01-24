/*
    Copyright 2002-2013 CEA LIST

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

namespace Lima
{

namespace LinguisticProcessing
{

namespace LinguisticAnalysisStructure
{

#define SENTENCEBOUNDARIESFINDER_CLASSID "SentenceBoundariesFinder"

/**
@author Benoit Mathieu
*/
class LIMA_TEXTSEGMENTATION_EXPORT SentenceBoundariesFinder : public MediaProcessUnit
{
public:
  SentenceBoundariesFinder();

  virtual ~SentenceBoundariesFinder();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  std::string m_graph;
  std::set<LimaString> m_boundaryValues;
  std::list<LinguisticCode> m_boundaryMicros;
};

}

}

}

#endif
