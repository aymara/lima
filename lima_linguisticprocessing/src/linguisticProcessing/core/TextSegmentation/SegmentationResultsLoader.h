/*
    Copyright 2002-2022 CEA LIST

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
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Mon Jan 17 2011
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SEGMENTATIONRESULTSLOADER_H
#define LIMA_LINGUISTICPROCESSING_SEGMENTATIONRESULTSLOADER_H

#include "TextSegmentationExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"

namespace Lima {
namespace LinguisticProcessing {

#define SEGMENTATIONRESULTSLOADER_CLASSID "SegmentationResultsLoader"

class SegmentationResultsLoaderPrivate;
/*
 * @brief A class to load results from text segmentation
 */
class LIMA_TEXTSEGMENTATION_EXPORT SegmentationResultsLoader : public AnalysisLoader
{
public:
  SegmentationResultsLoader();

  virtual ~SegmentationResultsLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  SegmentationResultsLoaderPrivate* m_d;
};

} // end namespace
} // end namespace

#endif
