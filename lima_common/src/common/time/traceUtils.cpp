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
#include "Windows.h"
#include <iomanip>


LARGE_INTEGER m_f;
static bool m_freqInit = false;

namespace 
{
  uint64_t _winTime()
  {
    LARGE_INTEGER i;
	if (m_freqInit == false)
	{
		QueryPerformanceFrequency(&m_f);
		m_freqInit = true;
	}
    QueryPerformanceCounter(&i);
    
    return (i.QuadPart * 1000000) / m_f.QuadPart; // microseconds   
  }
}
#else
#error no implementation for non-win32 systems
#endif


namespace Lima {

//**********************************************************************
//initialization of static members
//**********************************************************************
// uint64_t TimeUtils::currentTime={0,0};
std::map<std::string, TimeUtils::Data> TimeUtils::m_cumulatedTime = std::map<std::string, TimeUtils::Data>();
QMutex TimeUtils::m_mutex;


TimeUtils::TimeUtils() 
{
}
//**********************************************************************
// member functions
//**********************************************************************
uint64_t TimeUtils::getCurrentTime() { 
#ifdef WIN32
  return _winTime(); 
#else
#error no implementation for non-win32 systems
#endif
}
// FWI 03/11/24 nouvelle méthode pour remettre à zéro le cumul
void TimeUtils::restart( const std::string& taskCategory)
{
  QMutexLocker locker(&m_mutex);
#ifdef WIN32
  m_cumulatedTime[taskCategory].first = _winTime(); 
  //cout << "updateCurrentTime=" << m_cumulatedTime[taskCategory].first << ::std::endl;
#else
  m_cumulatedTime[taskCategory].first = QDateTime::currentMSecsSinceEpoch();
#endif
  m_cumulatedTime[taskCategory].second = 0;
  m_cumulatedTime[taskCategory].count = 0;
}

void TimeUtils::updateCurrentTime( const std::string& taskCategory ) {
  QMutexLocker locker(&m_mutex);
#ifdef WIN32
  m_cumulatedTime[taskCategory].first = _winTime();  
  //cout << "updateCurrentTime=" << m_cumulatedTime[taskCategory].first << ::std::endl;
#else
#error no implementation for non-win32 systems
#endif
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
#ifdef WIN32
  uint64_t newTime = _winTime();   
#else
#error no implementation for non-win32 systems
#endif
#ifdef WIN32
  uint64_t delta = newTime - m_cumulatedTime[taskCategory].first;
#else
#error no implementation for non-win32 systems
#endif
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
  LINFO << std::setfill('0') << std::setw(9) << m_cumulatedTime[taskCategory].second << " us"
    << " count : " << std::setfill('0') << std::setw(6) << m_cumulatedTime[taskCategory].count << ": " << mess;
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
