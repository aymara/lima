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
 * @file       SegmentationDataXmlLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
 * 
 * @brief      a logger for segmentation data
 * 
 ***********************************************************************/

#ifndef SEGMENTATIONDATAXMLLOGGER_H
#define SEGMENTATIONDATAXMLLOGGER_H

#include "TextSegmentationExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

namespace Lima {
namespace LinguisticProcessing {

#define SEGMENTATIONDATAXMLLOGGER_CLASSID "SegmentationDataXmlLogger"

class LIMA_TEXTSEGMENTATION_EXPORT SegmentationDataXmlLogger : public AbstractLinguisticLogger
{
 public:
  SegmentationDataXmlLogger(); 
  virtual ~SegmentationDataXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  std::string m_data;
};

} // end namespace
} // end namespace

#endif
