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
 * @file       CRFSegmentLoader.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Mar 24 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 * 
 * @brief a class to load results from text segmentation, in CRF output format
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_CRFSEGMENTLOADER_H
#define LIMA_LINGUISTICPROCESSING_CRFSEGMENTLOADER_H

#include "TextSegmentationExport.h"
#include "SegmentationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <iostream>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {

#define CRFSEGMENTLOADER_CLASSID "CRFSegmentLoader"

/*
 * @brief this is the abstract class for analysis loaders, that read
 * informations from external files to insert them in the analysis
 * data
 */
class LIMA_TEXTSEGMENTATION_EXPORT CRFSegmentLoader : public AnalysisLoader
{
public:
  CRFSegmentLoader();

  virtual ~CRFSegmentLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;

private:
  std::string m_dataName;
  std::string m_fromDataName;
};

} // end namespace
} // end namespace

#endif
