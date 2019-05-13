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
/************************************************************************
 *
 * @file       SegmentFeaturesDumper.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
 * 
 * @brief      a dumper that outputs tab-separated word features, one word per line
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTFEATURESDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTFEATURESDUMPER_H

#include "TextSegmentationExport.h"
#include "SegmentFeatureExtractor.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define SEGMENTFEATURESDUMPER_CLASSID "SegmentFeaturesDumper"

class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeaturesDumper : public AbstractTextualAnalysisDumper
{
public:
  SegmentFeaturesDumper();
  virtual ~SegmentFeaturesDumper();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override
  ;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
private:
  std::string m_graph; /*< name of the graph */
  std::string m_data; /*< name of the segmentation data used */
  std::string m_sep; /*< separator between features */
  std::string m_sepReplace; /*< character to replace separator in feature strings */
  SegmentFeatures* m_features; /* list of features */
};


}
}
}

#endif
