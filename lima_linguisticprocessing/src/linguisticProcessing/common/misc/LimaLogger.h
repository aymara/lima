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
 * @file       limaLogger.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jan 28 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     s2common
 * 
 * @brief      lima logger (wrapper around log4cxx)
 * 
 * 
 ***********************************************************************/

#ifndef LIMALOGGER_H
#define LIMALOGGER_H

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/level.h>
#include <log4cxx/helpers/exception.h>

namespace Lima {

#define LOGINIT(X) LimaLogger::setLogger((X));

#define LDEBUG if (LimaLogger::getLogger()->isDebugEnabled()) LimaLogger::getDebugLogger()
#define LINFO if (LimaLogger::getLogger()->isInfoEnabled()) LimaLogger::getInfoLogger()
#define LWARN if (LimaLogger::getLogger()->isWarnEnabled()) LimaLogger::getWarnLogger()
#define LERROR if (LimaLogger::getLogger()->isErrorEnabled()) LimaLogger::getErrorLogger()
#define LFATAL if (LimaLogger::getLogger()->isFatalEnabled()) LimaLogger::getFatalLogger()
#define LENDL LimaLevelLogger::ENDLINE

class LimaLevelLogger;

class LimaLogger {
 public:
  static void configure(const std::string filename);
  static void setLogger(const std::string& loggerName) {
    m_currentLogger=log4cxx::Logger::getLogger(loggerName);
  }
  static log4cxx::LoggerPtr& getLogger() { return m_currentLogger; }
  static LimaLevelLogger& getDebugLogger() { return m_debugLogger; }
  static LimaLevelLogger& getWarnLogger() { return m_warnLogger; }
  static LimaLevelLogger& getInfoLogger() { return m_infoLogger; }
  static LimaLevelLogger& getErrorLogger() { return m_errorLogger; }
  static LimaLevelLogger& getFatalLogger() { return m_fatalLogger; }

 private:
  LimaLogger() {}
  ~LimaLogger() {}
  static log4cxx::LoggerPtr m_currentLogger;
  static LimaLevelLogger m_debugLogger;
  static LimaLevelLogger m_warnLogger;
  static LimaLevelLogger m_infoLogger;
  static LimaLevelLogger m_errorLogger;
  static LimaLevelLogger m_fatalLogger;
};

class LimaLevelLogger {
public:
  LimaLevelLogger(log4cxx::LevelPtr level):
    m_buffer(),m_level(level) {};
  ~LimaLevelLogger() {};

  typedef enum { ENDLINE } EndLine;

  LimaLevelLogger& operator << (const EndLine& endline) {
    LimaLogger::getLogger()
      ->forcedLog(m_level, m_buffer.str(),__FILE__,__LINE__); 
    // reinit buffer
    m_buffer.str("");
    return *this;
  }

  template<typename T>
    LimaLevelLogger& operator << (const T& t) {
    m_buffer << t; 
    return *this;
  }

private:
  ::log4cxx::StringBuffer m_buffer; 
  ::log4cxx::LevelPtr m_level;
};

} // end namespace

#endif
