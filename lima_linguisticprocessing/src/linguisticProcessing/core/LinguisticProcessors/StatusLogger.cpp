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
#ifdef WIN32
#define _WINSOCKAPI_
#endif


/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "StatusLogger.h"
#include "LinguisticMetaData.h"
#include "LimaStringText.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/misc/PortableGetTimeOfDay.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{

namespace LinguisticProcessing
{

SimpleFactory<MediaProcessUnit,StatusLogger> statusLoggerFactory(STATUSLOGGER_CLASSID);

StatusLogger::StatusLogger()
{}


StatusLogger::~StatusLogger()
{}


void StatusLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager*)

{
  ostringstream os;
  os << "/proc/";
#ifndef WIN32
  os << getpid();
#endif
  os << "/status";
  m_statusFile=os.str();
  std::string outputFile;
  try
  {
    outputFile=unitConfiguration.getParamsValueAtKey("outputFile");
  }
  catch (NoSuchParam& )
  {
    outputFile=string("status.log");
  }
  m_out= new ofstream(outputFile.c_str(), std::ofstream::binary);
  try
  {
    deque<string> tolog=unitConfiguration.getListsValueAtKey("toLog");
    m_toLog.insert(tolog.begin(),tolog.end());
  }
  catch (NoSuchList& )
  {
    m_toLog.insert(string("VmSize"));
  }
  m_predTime=TimeUtils::getCurrentTime();
}


LimaStatusCode StatusLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  uint64_t tmp=TimeUtils::getCurrentTime();
  // log time
  *m_out << TimeUtils::diffTime(m_predTime,tmp) << " ";
  const_cast<StatusLogger*>(this)->m_predTime=tmp;

  // log file and document name
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  
  try
  {
    const string& filename=metadata->getMetaData("FileName");
    *m_out << filename << " ";
  }
  catch (LinguisticProcessingException& ) {}
  try
  {
    string doc=metadata->getMetaData("DocumentName");
    *m_out << doc << " ";
  }
  catch (LinguisticProcessingException& ) {}

  ifstream statusIn(m_statusFile.c_str(),ios::in | std::ifstream::binary);
  string line;
  while (!statusIn.eof())
  {
    getline(statusIn,line);
    size_t index=line.find(":");
    string key=line.substr(0,index);
    if (m_toLog.find(key)!=m_toLog.end())
    {
      *m_out << line << " ";
    }
  }

  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));
  *m_out << "TextSize= " << originalText->size() << endl;
  TimeUtils::logElapsedTime("StatusLogger");
  return SUCCESS_ID;
}

}

}
