// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       traceUtils.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Mar  5 2004
 * @version    $Id: traceUtils.cpp,v 1.3 2005/10/20 11:37:31 gael Exp $
 * copyright   Copyright (C) 2004-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "traceUtils.h"
#include <QDateTime>

namespace Lima {

//**********************************************************************
//initialization of static members
//**********************************************************************
// uint64_t TimeUtils::currentTime={0,0};
std::map<std::string , std::pair<uint64_t,uint64_t> > TimeUtils::m_cumulatedTime =
  std::map<std::string , std::pair<uint64_t,uint64_t> >();
QMutex TimeUtils::m_mutex;

//**********************************************************************
// member functions
//**********************************************************************
uint64_t TimeUtils::getCurrentTime() { 
  return QDateTime::currentMSecsSinceEpoch();
}

void TimeUtils::updateCurrentTime( const std::string& taskCategory ) {
  QMutexLocker locker(&m_mutex);
  m_cumulatedTime[taskCategory].first = QDateTime::currentMSecsSinceEpoch();
}

// void TimeUtils::updateCurrentTime() {
//   boost::mutex::scoped_lock(m_mutex);
//   gettimeofday(&currentTime,0);
// }

void TimeUtils::setCurrentTime(uint64_t time, const std::string& taskCategory) {
  QMutexLocker locker(&m_mutex);
  m_cumulatedTime[taskCategory].first=time;
}

uint64_t TimeUtils::diffTime(const uint64_t& begin,
                                  const uint64_t& end) {
  return end - begin;
}

uint64_t TimeUtils::elapsedTime(const std::string& taskCategory) {
  uint64_t newTime = QDateTime::currentMSecsSinceEpoch();
  uint64_t delta = diffTime(m_cumulatedTime[taskCategory].first,newTime);
  m_cumulatedTime[taskCategory].second += delta;
  m_cumulatedTime[taskCategory].first = newTime;
  return delta;
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logElapsedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
  LINFO << mess << "(" << taskCategory << "): " << TimeUtils::elapsedTime(taskCategory) << " ms";
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logCumulatedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
  LINFO << mess << ": " << m_cumulatedTime[taskCategory].second << " ms";
}

void TimeUtils::logAllCumulatedTime(const std::string& mess) {
  TIMELOGINIT;
  LINFO << mess << ": ";
  for( std::map<std::string , std::pair<uint64_t,uint64_t> >::const_iterator it = m_cumulatedTime.begin() ; 
	   it != m_cumulatedTime.end() ; it++ ) {
	LINFO << it->first << ":" << it->second.second << " ms" ;
  }
}


} // end namespace
