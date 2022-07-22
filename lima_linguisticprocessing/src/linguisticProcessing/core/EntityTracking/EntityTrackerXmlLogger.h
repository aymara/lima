// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef ENTITYTRACKERXMLLOGGER_H
#define ENTITYTRACKERXMLLOGGER_H

#include "EntityTrackingExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{

/**
@author Zied Boulila
*/
#define ENTITYTRACKERXMLLOGGER_CLASSID "EntityTrackerXmlLogger"

class LIMA_ENTITYTRACKING_EXPORT EntityTrackerXmlLogger : public AbstractLinguisticLogger
{
public:
  EntityTrackerXmlLogger();

  virtual ~EntityTrackerXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  bool m_compactFormat; // compact format is the same as the RecognizerResultLogger
  MediaId m_language;
  std::string m_graph;
};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif
