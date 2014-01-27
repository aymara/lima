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
