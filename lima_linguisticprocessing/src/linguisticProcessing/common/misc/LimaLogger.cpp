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
 * @file       limaLogger.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Feb  2 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "limaLogger.h"
#include <log4cxx/propertyconfigurator.h>

namespace Lima {

// initialization of static members 

log4cxx::LoggerPtr LimaLogger::m_currentLogger;

LimaLevelLogger LimaLogger::m_debugLogger(::log4cxx::Level::DEBUG);
LimaLevelLogger LimaLogger::m_warnLogger(::log4cxx::Level::WARN);
LimaLevelLogger LimaLogger::m_infoLogger(::log4cxx::Level::INFO);
LimaLevelLogger LimaLogger::m_errorLogger(::log4cxx::Level::ERROR);
LimaLevelLogger LimaLogger::m_fatalLogger(::log4cxx::Level::FATAL);

// configure function
void LimaLogger::configure(const std::string filename) {
  log4cxx::PropertyConfigurator::configure(filename);
}

} // end namespace
