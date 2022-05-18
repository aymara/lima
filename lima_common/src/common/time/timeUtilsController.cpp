// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       timeUtilsController.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Mar  5 2004
 * @version    $Id: timeUtilsController.cpp,v 1.3 2005/10/20 11:37:31 gael Exp $
 * copyright   Copyright (C) 2004-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "timeUtilsController.h"
#include "traceUtils.h"

namespace Lima {

TimeUtilsController::TimeUtilsController(const std::string& topic, bool logElapsedTime)
    : m_topic(topic), m_logElapsedTime(logElapsedTime)
{
  TimeUtils::updateCurrentTime( topic );
}

TimeUtilsController::~TimeUtilsController() {
  uint64_t delta = TimeUtils::elapsedTime(m_topic);
  if (m_logElapsedTime) {
    TIMELOGINIT;
    LINFO << m_topic << " (  ): " << delta << " ms";
  }
}
  

} // end namespace
