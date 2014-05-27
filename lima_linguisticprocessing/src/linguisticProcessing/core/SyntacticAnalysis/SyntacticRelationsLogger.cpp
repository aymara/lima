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
#include "SyntacticRelationsLogger.h"
// #include "common/linguisticData/linguisticData.h"
// #include "common/misc/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticAnalysisTools.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

SimpleFactory<MediaProcessUnit,SyntacticRelationsLogger> syntacticRelationsLoggerLoggerFactory(SYNTACTICRELATIONSLOGGER_CLASSID);

SyntacticRelationsLogger::SyntacticRelationsLogger():
AbstractTextualAnalysisDumper(),
m_language(0)
{}

SyntacticRelationsLogger::~SyntacticRelationsLogger()
{}

void SyntacticRelationsLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  //Parameters settings example
  /*try
  {
    string boolValue=unitConfiguration.getParamsValueAtKey("displayChains");
    if (boolValue=="yes" || boolValue=="true" || boolValue=="1") {
      m_displayChains=true;
    }
    else {
      m_displayChains=false;
    }
  }
  catch (NoSuchParam& ) {
    // optional param, keep default value
  }   */
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode SyntacticRelationsLogger::process(
  AnalysisContent& analysis) const
{
  SALOGINIT;
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SALOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  DumperStream* dstream=initialize(analysis);
  std::ostream& outputStream=dstream->out();
  /*std::ofstream ofs;
  if (!openLogFile(ofs,metadata->getMetaData("FileName"))) {
      SALOGINIT;
      LERROR << "SyntacticRelationsLogger::Cannot open log file";
      return CANNOT_OPEN_FILE_ERROR;
  }*/

//   AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  //ofs << endl << "Printing syntactic analysis result :" << endl;
  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));
  LDEBUG << "call of displayRelationsXMLFormat";
  SyntacticAnalysisTools::displayRelationsXMLFormat(*syntacticData,m_language,outputStream);
  delete dstream;
  TimeUtils::logElapsedTime("SyntacticRelationsLogger");
  return SUCCESS_ID;
}

} // Data
} // LinguisticProcessing
} // Lima
