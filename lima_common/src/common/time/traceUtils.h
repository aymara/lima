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
 * @file       traceUtils.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Mar  5 2004
 * @version    $Id: traceUtils.h,v 1.3 2005/10/20 11:37:31 gael Exp $
 * copyright   Copyright (C) 2004-2012 by CEA LIST
 * Project     Linguistic Analysis
 *
 * @brief      contains functions to trace some infos for debug
 *
 *
 ***********************************************************************/

#ifndef LIMA_TRACEUTILS_H
#define LIMA_TRACEUTILS_H

#include "common/LimaCommon.h"

#include <QtCore/QMutex>

#include <string>


namespace Lima {
  
  #define TIMELOGINIT LOGINIT("TimeInfo")
  /**
   * This class contains utility functions to log informations about time,
   * such as logging time elapsed since a specific moment. It uses the logging facility:
   * set the TimeInfo category to INFO to view the time logs.
   *
   * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
   *
   */
  class LIMA_TIME_EXPORT TimeUtils
  {
  public:
    TimeUtils() {}
    ~TimeUtils() {}
    
    /**
     * store current time for new elapsed time computation
     * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
     *
     */
    //   static void updateCurrentTime( const std::string& taskCategory = std::string("") );
    static void updateCurrentTime( const std::string& taskCategory = std::string("") );
    
    //   static void setCurrentTime(uint64_t time);
    static void setCurrentTime(uint64_t time, const std::string& taskCategory = std::string(""));
    
    /**
     * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
     * @return the number of microseconds since last UpdateCurrentTime
     */
    // static uint64_t elapsedTime();
    static uint64_t elapsedTime( const std::string& taskCategory = std::string("") );
    
    /**
     * return the difference between two times in microseconds
     */
    static uint64_t diffTime(const uint64_t& begin,
                             const uint64_t& end);
    
    /**
     * log the number of microseconds since last UpdateCurrentTime
     * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
     */
    //   void TimeUtils::logElapsedTime(const std::string& mess) {
      static void logElapsedTime(const std::string& mess,
                                 const std::string& taskCategory = std::string("") );
      
      /**
       * log the number of microseconds cumulated for the category
       * sum of elapsed time between consecutive call of UpdateCurrent() and elapsedTime()
       * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
       */
      static void logCumulatedTime(const std::string& mess,
                                   const std::string& taskCategory = std::string(""));
      /**
       * log the number of microseconds cumulated for all categories
       * @note this class is deprecated to count the time passed in a specific class. Use @ref TimeUtilsController instead.
       */
      static void logAllCumulatedTime(const std::string& mess);
      /**
       * get the current time (real current time, not last time
       * stored)
       *
       */
      static uint64_t getCurrentTime();
      
    private:
      /** last current time stored */
      //   static uint64_t currentTime;
      static std::map<std::string , std::pair<uint64_t,uint64_t> > m_cumulatedTime;
      static QMutex m_mutex;
    };
    
  } // end namespace
  
  #endif
  
