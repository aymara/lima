// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
