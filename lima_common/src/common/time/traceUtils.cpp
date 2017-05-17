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
 * @file       traceUtils.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Mar  5 2004
 * @version    $Id: traceUtils.cpp,v 1.3 2005/10/20 11:37:31 gael Exp $
 * copyright   Copyright (C) 2004-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "traceUtils.h"
#include <QDateTime>

#ifdef WIN32
#include <Windows.h>
#else // linux
#include <time.h>
#endif
#ifdef ANTINNO_SPECIFIC
#include <iomanip>
#endif

namespace Lima {

//**********************************************************************
//initialization of static members
//**********************************************************************
// uint64_t TimeUtils::currentTime={0,0};
std::map<std::string, TimeUtils::Data> TimeUtils::m_cumulatedTime = std::map<std::string, TimeUtils::Data>();
QMutex TimeUtils::m_mutex;

//**********************************************************************
// member functions
//**********************************************************************
TimeUtils::TimeUtils() 
{
}

uint64_t TimeUtils::getCurrentTime() { 
  // This function is platform dependent
#ifdef WIN32
  static LARGE_INTEGER frequency;
  if (frequency.QuadPart == 0) {
    QueryPerformanceFrequency(&frequency);
  }
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return 1000000 * now.QuadPart / frequency.QuadPart; // microseconds
#else // linux
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return 1000000 * now.tv_sec + now.tv_nsec / 1000; // microseconds
#endif
}

// FWI 03/11/24 nouvelle méthode pour remettre à zéro le cumul
void TimeUtils::restart( const std::string& taskCategory)
{
  QMutexLocker locker(&m_mutex);
  m_cumulatedTime[taskCategory].first = getCurrentTime();
  m_cumulatedTime[taskCategory].second = 0;
  m_cumulatedTime[taskCategory].count = 0;
}

void TimeUtils::updateCurrentTime( const std::string& taskCategory ) {
  QMutexLocker locker(&m_mutex);
  m_cumulatedTime[taskCategory].first = getCurrentTime();
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
  uint64_t newTime = getCurrentTime();
  uint64_t delta = diffTime(m_cumulatedTime[taskCategory].first,newTime);
  m_cumulatedTime[taskCategory].second += delta;
  m_cumulatedTime[taskCategory].first = newTime;
  ++m_cumulatedTime[taskCategory].count;
  return delta;
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logElapsedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
  LINFO << mess << "(" << taskCategory << "): " << TimeUtils::elapsedTime(taskCategory) << " us";
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logCumulatedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
#ifdef ANTINNO_SPECIFIC
  ::std::ostringstream oss;
  auto const duration = m_cumulatedTime[taskCategory].second;
  if (duration != 0)
    oss << std::setfill(' ') << std::setw(9) << duration;
  else
    oss << "         ";
  oss << " us ";
  auto const count = m_cumulatedTime[taskCategory].count;
  if (count != 0)
    oss << std::setfill(' ') << std::setw(6) << count;
  else
    oss << "      ";
  oss  << " occurences  " << mess;
  LINFO << oss.str();
#else
  LINFO << m_cumulatedTime[taskCategory].second << " us"
  << " count: " << m_cumulatedTime[taskCategory].count << ": " << mess;
#endif
}

void TimeUtils::logAllCumulatedTime(const std::string& mess) {
  TIMELOGINIT;
  LINFO << mess << ": ";

  for( std::map<std::string, Data>::const_iterator it = m_cumulatedTime.begin() ; 
	   it != m_cumulatedTime.end() ; it++ ) {
	  LINFO << it->first << ":" << it->second.second << " us" << " count: " << it->second.count;
  }
}


} // end namespace
