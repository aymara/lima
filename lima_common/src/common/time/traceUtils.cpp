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

#ifdef ANTINNO_SPECIFIC
// FWI 28/10/2015 modifs pour utiliser une horloge plus précise (en us au lieu de ms) sous windows
// + ajout d'un compteur

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
#endif


namespace Lima {

//**********************************************************************
//initialization of static members
//**********************************************************************
// uint64_t TimeUtils::currentTime={0,0};
#ifdef ANTINNO_SPECIFIC
std::map<std::string, TimeUtils::Data> TimeUtils::m_cumulatedTime = std::map<std::string, TimeUtils::Data>();
#else
std::map<std::string , std::pair<uint64_t,uint64_t> > TimeUtils::m_cumulatedTime =
  std::map<std::string , std::pair<uint64_t,uint64_t> >();
#endif
QMutex TimeUtils::m_mutex;


#ifdef ANTINNO_SPECIFIC
TimeUtils::TimeUtils() 
{
}
#endif
//**********************************************************************
// member functions
//**********************************************************************
uint64_t TimeUtils::getCurrentTime() { 
#ifdef ANTINNO_SPECIFIC
#ifdef WIN32
  return _winTime(); 
#else
#error no implementation for non-win32 systems
#endif
#else
  return QDateTime::currentMSecsSinceEpoch();
#endif
}
#ifdef ANTINNO_SPECIFIC
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
#endif

void TimeUtils::updateCurrentTime( const std::string& taskCategory ) {
  QMutexLocker locker(&m_mutex);
#ifdef ANTINNO_SPECIFIC
#ifdef WIN32
  m_cumulatedTime[taskCategory].first = _winTime();  
  //cout << "updateCurrentTime=" << m_cumulatedTime[taskCategory].first << ::std::endl;
#else
#error no implementation for non-win32 systems
#endif
#else
  m_cumulatedTime[taskCategory].first = QDateTime::currentMSecsSinceEpoch();
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
#ifdef ANTINNO_SPECIFIC
#ifdef WIN32
  uint64_t newTime = _winTime();   
#else
#error no implementation for non-win32 systems
#endif
#else
  uint64_t newTime = QDateTime::currentMSecsSinceEpoch();
#endif
#ifdef ANTINNO_SPECIFIC
#ifdef WIN32
  uint64_t delta = newTime - m_cumulatedTime[taskCategory].first;
#else
#error no implementation for non-win32 systems
#endif
#else
  uint64_t delta = diffTime(m_cumulatedTime[taskCategory].first,newTime);
#endif
  m_cumulatedTime[taskCategory].second += delta;
  m_cumulatedTime[taskCategory].first = newTime;
#ifdef ANTINNO_SPECIFIC
  ++m_cumulatedTime[taskCategory].count;
  #endif
  return delta;
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logElapsedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
#ifdef ANTINNO_SPECIFIC
  LINFO << mess << "(" << taskCategory << "): " << TimeUtils::elapsedTime(taskCategory) << " us";
#else
  LINFO << mess << "(" << taskCategory << "): " << TimeUtils::elapsedTime(taskCategory) << " ms";
#endif
}

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logCumulatedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
#ifdef ANTINNO_SPECIFIC
  LINFO << std::setfill('0') << std::setw(9) << m_cumulatedTime[taskCategory].second << " us"
    << " count : " << std::setfill('0') << std::setw(6) << m_cumulatedTime[taskCategory].count << ": " << mess;
#else
  LINFO << mess << ": " << m_cumulatedTime[taskCategory].second << " ms";
#endif
}

void TimeUtils::logAllCumulatedTime(const std::string& mess) {
  TIMELOGINIT;
  LINFO << mess << ": ";

#ifdef ANTINNO_SPECIFIC
  for( std::map<std::string, Data>::const_iterator it = m_cumulatedTime.begin() ; 
	   it != m_cumulatedTime.end() ; it++ ) {
	LINFO << it->first << ":" << it->second.second << " us" << " count: " << it->second.count;
#else
  for( std::map<std::string , std::pair<uint64_t,uint64_t> >::const_iterator it = m_cumulatedTime.begin() ; 
	   it != m_cumulatedTime.end() ; it++ ) {
	LINFO << it->first << ":" << it->second.second << " ms" ;
#endif
  }
}


} // end namespace
