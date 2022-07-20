// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
