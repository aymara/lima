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
 * @file       ExternalProcessUnit.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 *
 ***********************************************************************/

#include "ExternalProcessUnit.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"

//#include "boost/process.hpp"
#include <QtCore/QProcess>

#include <fstream>

//namespace bp = ::boost::process; 

using namespace std;

namespace Lima {
namespace LinguisticProcessing {

#define DEFAULT_TEMPFILE "/tmp/lp_external_tmp"

SimpleFactory<MediaProcessUnit,ExternalProcessUnit> ExternalProcessUnitFactory(EXTERNALPROCESSUNIT_CLASSID);

ExternalProcessUnit::ExternalProcessUnit():
MediaProcessUnit(),
m_dumper(),
m_loader(),
m_commandLine(),
m_useTemporaryFile(true),
m_cleanTemporaryFile(true),
m_tmpFileName(DEFAULT_TEMPFILE),
m_handler(0),
m_out(0)
{
}

ExternalProcessUnit::~ExternalProcessUnit()
{
}

//************************************************************************
bool 
getBooleanParameter(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                    const std::string& param) {
  string value=unitConfiguration.getParamsValueAtKey(param);
  if (value == "yes" ||
    value == "true" ||
    value == "1") {
    return true;
  }
  return false;
}
                   

void ExternalProcessUnit::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOGINIT("LP:External");
  LDEBUG << "Initialization";

  MediaId language=manager->getInitializationParameters().media;
  try {
    string dumperName=unitConfiguration.getParamsValueAtKey("dumper");
    // create the dumper
    m_dumper=manager->getObject(dumperName);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LERROR << "Missing 'dumper' parameter in ExternalProcessUnit group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }

  try {
    string loaderName=unitConfiguration.getParamsValueAtKey("loader");
    // create the loader
    m_loader=manager->getObject(loaderName);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LERROR << "Missing 'loader' parameter in ExternalProcessUnit group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }

  try {
    m_useTemporaryFile=getBooleanParameter(unitConfiguration,"useTmpFile");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_cleanTemporaryFile=getBooleanParameter(unitConfiguration,"cleanTmpFile");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_tmpFileName=unitConfiguration.getParamsValueAtKey("tmpFileName");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_commandLine=unitConfiguration.getParamsValueAtKey("command");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LERROR << "Missing 'command' parameter in ExternalProcessUnit group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }
}

LimaStatusCode ExternalProcessUnit::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  LOGINIT("LP:External");
  LINFO << "ExternalProcessUnit: start";

  LimaStatusCode returnCode(SUCCESS_ID);

  // produce temporary file with the given dumper
  LDEBUG << "ExternalProcessUnit: write tmp file";
  returnCode=m_dumper->process(analysis);
  if (returnCode!=SUCCESS_ID) {
    LERROR << "ExternalProcessUnit: failed to dump data to temporary file";
    return returnCode;
  }

  // apply command line
  LDEBUG << "ExternalProcessUnit: apply external program";
  QProcess::execute(m_commandLine.c_str());

  // load results from the external program with the given loader
  LDEBUG << "ExternalProcessUnit: read results";
  returnCode=m_loader->process(analysis);
  if (returnCode!=SUCCESS_ID) {
    LERROR << "ExternalProcessUnit: failed to load data from temporary file";
    return returnCode;
  }
  
  TimeUtils::logElapsedTime("ExternalProcessUnit");
  return returnCode;
}


} // end namespace
} // end namespace
