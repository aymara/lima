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
#include "PortableGetTimeOfDay.h"

namespace Lima {

//**********************************************************************
//initialization of static members
//**********************************************************************
// timeval TimeUtils::currentTime={0,0};
std::map<std::string , std::pair<timeval,uint64_t> > TimeUtils::m_cumulatedTime =
  std::map<std::string , std::pair<timeval,uint64_t> >();
boost::mutex TimeUtils::m_mutex;

//**********************************************************************
// member functions
//**********************************************************************
timeval TimeUtils::getCurrentTime() { 
  timeval current;
  gettimeofday(&current,0);
  return current;
}

void TimeUtils::updateCurrentTime( const std::string& taskCategory ) {
  boost::mutex::scoped_lock(m_mutex);
  gettimeofday(&(m_cumulatedTime[taskCategory].first),0);
}

// void TimeUtils::updateCurrentTime() {
//   boost::mutex::scoped_lock(m_mutex);
//   gettimeofday(&currentTime,0);
// }

void TimeUtils::setCurrentTime(timeval time, const std::string& taskCategory) {
  boost::mutex::scoped_lock(m_mutex);
  m_cumulatedTime[taskCategory].first=time;
}

// void TimeUtils::setCurrentTime(timeval time) {
//   boost::mutex::scoped_lock(m_mutex);
//   currentTime=time;
// }


uint64_t TimeUtils::diffTime(const timeval& begin,
                                  const timeval& end) {
  uint64_t 
    microsecondsElapsed=((end.tv_sec*1000000+end.tv_usec) - 
                         (begin.tv_sec*1000000+begin.tv_usec));
  return microsecondsElapsed; // return microseconds
}

uint64_t TimeUtils::elapsedTime(const std::string& taskCategory) {
  timeval newTime;
  gettimeofday(&newTime,0);
  uint64_t delta = diffTime(m_cumulatedTime[taskCategory].first,newTime);
  m_cumulatedTime[taskCategory].second += delta;
  m_cumulatedTime[taskCategory].first = newTime;
  return delta;
}

// uint64_t TimeUtils::elapsedTime() {
//   timeval newTime;
//   gettimeofday(&newTime,0);
//   return diffTime(currentTime,newTime);
// }

/**
 * log the number of microseconds since last UpdateCurrentTime
 */
void TimeUtils::logElapsedTime(const std::string& mess,
                               const std::string& taskCategory) {
  TIMELOGINIT;
  LINFO << mess << "(" << taskCategory << "): " << TimeUtils::elapsedTime(taskCategory) << " ms";
}

//   void TimeUtils::logElapsedTime(const std::string& mess) {
//   TIMELOGINIT;
//   LINFO << mess << ": " << elapsedTime() << " ms";
// }

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
  for( std::map<std::string , std::pair<timeval,uint64_t> >::const_iterator it = m_cumulatedTime.begin() ; 
	   it != m_cumulatedTime.end() ; it++ ) {
	LINFO << it->first << ":" << it->second.second << " ms" ;
  }
}


} // end namespace
