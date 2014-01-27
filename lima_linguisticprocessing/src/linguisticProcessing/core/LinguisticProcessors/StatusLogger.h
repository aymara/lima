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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGSTATUSLOGGER_H
#define LIMA_LINGUISTICPROCESSINGSTATUSLOGGER_H

#include "LinguisticProcessorsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <set>
#include <fstream>

#ifdef WIN32
#include <WinSock2.h>
#endif

namespace Lima
{

namespace LinguisticProcessing
{

#define STATUSLOGGER_CLASSID "StatusLogger"

/**
@author Benoit Mathieu
*/
class LIMA_LINGUISTICPROCESSORS_EXPORT StatusLogger : public MediaProcessUnit
{
public:

  StatusLogger();
  virtual ~StatusLogger();
  
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;
  
  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

private:

  std::ostream* m_out;
  std::set<std::string> m_toLog;
  std::string m_statusFile;
  timeval* m_predTime;

};

}

}

#endif
