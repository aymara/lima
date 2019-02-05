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
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 ***********************************************************************/

#include "ExternalProcessUnit.h"

#include "LinguisticMetaData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"

//#include "boost/process.hpp"
#include <QtCore/QProcess>

#include <fstream>

//namespace bp = ::boost::process; 

using namespace std;

namespace Lima {
namespace LinguisticProcessing {

SimpleFactory<MediaProcessUnit,ExternalProcessUnit> ExternalProcessUnitFactory(EXTERNALPROCESSUNIT_CLASSID);

ExternalProcessUnit::ExternalProcessUnit():
MediaProcessUnit(),
m_dumper(),
m_loader(),
m_commandLine(),
m_inputSuffix(),
m_outputSuffix()
{
}

ExternalProcessUnit::~ExternalProcessUnit()
{
}


void ExternalProcessUnit::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOGINIT("LP::External");
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
  catch (InvalidConfiguration& ) {
    m_loader = 0;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LERROR << "Missing 'loader' parameter in ExternalProcessUnit group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }

  try {
    m_inputSuffix=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("inputSuffix").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_outputSuffix=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("outputSuffix").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }

  try {
    m_commandLine=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("command").c_str());
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
  LOGINIT("LP::External");
  LINFO << "ExternalProcessUnit: start";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  LimaStatusCode returnCode(SUCCESS_ID);

  // produce temporary file with the given dumper
  LDEBUG << "ExternalProcessUnit: write tmp file";
  returnCode=m_dumper->process(analysis);
  if (returnCode!=SUCCESS_ID) {
    LERROR << "ExternalProcessUnit: failed to dump data to temporary file";
    return returnCode;
  }

  QString fileName = QString::fromUtf8(metadata->getMetaData("FileName").c_str());
  QString inputFilename, outputFilename;
  // apply command line
  LDEBUG << "ExternalProcessUnit: apply external program";
  QString commandLine = m_commandLine;
  if (!m_inputSuffix.isEmpty())
  {
    inputFilename = fileName+ m_inputSuffix;
  }
  if (!m_outputSuffix.isEmpty())
  {
    outputFilename = fileName + m_outputSuffix;
  }
  commandLine = commandLine.arg(inputFilename, outputFilename);
  LDEBUG << "Launching " << commandLine;
  int processResult = QProcess::execute(commandLine);
  switch (processResult) {
    case -2 :
    LERROR << "ExternalProcessUnit: Was not able to start '" << commandLine << "'" ;
    return returnCode;
    case -1 :
    LERROR << "ExternalProcessUnit: '" << commandLine << "' crashed!";
    return returnCode;
    case 0 :
    break;
    default:
    LERROR << "ExternalProcessUnit: '" << commandLine << "' returned error status:" << processResult;
    return returnCode;
  }

  if (m_loader != 0) {
    // load results from the external program with the given loader
    LDEBUG << "ExternalProcessUnit: read results";
    returnCode=m_loader->process(analysis);
    if (returnCode!=SUCCESS_ID) {
      LERROR << "ExternalProcessUnit: failed to load data from temporary file";
      return returnCode;
    }
  }
  else {
    LWARN << "ExternalProcessUnit: no loader defined for the current external process unit";
  }
  
  TimeUtils::logElapsedTime("ExternalProcessUnit");
  return returnCode;
}


} // end namespace
} // end namespace
