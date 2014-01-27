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
/** @brief      debug only logger for syntactic analysis.
  *
  * @file       debugSyntacticAnalysisLogger.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  *             Copyright (c) 2004 by CEA
  * @version    $Id$
  */

#include "DebugSyntacticAnalysisLogger.h"
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

SimpleFactory<MediaProcessUnit,DebugSyntacticAnalysisLogger> debugSyntacticAnalysisLoggerFactory(DEBUGSYNTACTICANALYSISLOGGER_CLASSID);

DebugSyntacticAnalysisLogger::DebugSyntacticAnalysisLogger():
AbstractLinguisticLogger(".syntanal.txt"),
m_language(0),
m_displayChains(true),
m_displayDependencies(true),
m_displayRelationsOrderOfArguments(false),
m_displayRelationsDistanceOfArguments(false),
m_displayLemmas(true)
{}

DebugSyntacticAnalysisLogger::~DebugSyntacticAnalysisLogger()
{}

void DebugSyntacticAnalysisLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try
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
  }
  try
  {
    string boolValue=unitConfiguration.getParamsValueAtKey("displayDependencies");
    if (boolValue=="yes" || boolValue=="true" || boolValue=="1") {
      m_displayDependencies=true;
    }
    else {
      m_displayDependencies=false;
    }
  }
  catch (NoSuchParam& ) {
    // optional param, keep default value
  }

  try
  {
    string boolValue=unitConfiguration.getParamsValueAtKey("lemma");
    if (boolValue=="yes" || boolValue=="true" || boolValue=="1") {
      m_displayLemmas=true;
    }
    else {
      m_displayLemmas=false;
    }
  }
  catch (NoSuchParam& ) {
    // optional param, keep default value
  }

  try
  {
    string boolValue=unitConfiguration.getParamsValueAtKey("displayRelationsOrderOfArguments");
    if (boolValue=="yes" || boolValue=="true" || boolValue=="1") {
      m_displayRelationsOrderOfArguments=true;
    }
    else {
      m_displayRelationsOrderOfArguments=false;
    }
  }
  catch (NoSuchParam& ) {
    // optional param, keep default value
  }
  try
  {
    string boolValue=unitConfiguration.getParamsValueAtKey("displayRelationsDistanceOfArguments");
    if (boolValue=="yes" || boolValue=="true" || boolValue=="1") {
      m_displayRelationsDistanceOfArguments=true;
    }
    else {
      m_displayRelationsDistanceOfArguments=false;
    }
  }
  catch (NoSuchParam& ) {
    // optional param, keep default value
  }
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode DebugSyntacticAnalysisLogger::process(
  AnalysisContent& analysis) const
{
  SALOGINIT;
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SALOGINIT;
      LERROR << "no LinguisticMetaData ! abort" << LENDL;
      return MISSING_DATA;
  }

  std::ofstream ofs;
  if (!openLogFile(ofs,metadata->getMetaData("FileName"))) {
      SALOGINIT;
      LERROR << "DebugSyntacticAnalysisLogger::Cannot open log file" << LENDL;
      return CANNOT_OPEN_FILE_ERROR;
  }

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  //ofs << endl << "Printing syntactic analysis result :" << endl;
  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));
  //cout << endl;
  if (m_displayChains) {
    LDEBUG << "call of displayChains" << LENDL;
    SyntacticAnalysisTools::displayChains(tokenList, ofs);
  }
  if (m_displayDependencies) {
    LDEBUG << "call of displayDependancies" << LENDL;
    SyntacticAnalysisTools::displayDependancies(*syntacticData,
                                                m_language,ofs,m_displayLemmas);
  }
  if (m_displayRelationsOrderOfArguments) {
    LDEBUG << "call of displayRelationsOrderOfArguments" << LENDL;
    SyntacticAnalysisTools::displayRelationsOrderOfArguments(*syntacticData,
                                                             m_language,
                                                             ofs);
  }
  if (m_displayRelationsDistanceOfArguments) {
    LDEBUG << "call of displayRelationsDistanceOfArguments" << LENDL;
    SyntacticAnalysisTools::displayRelationsDistanceOfArguments(*syntacticData,
                                                                m_language,
                                                                ofs);
  }
  ofs.close();
  TimeUtils::logElapsedTime("DebugSyntacticAnalysisLogger");
  return SUCCESS_ID;
}

} // Data
} // LinguisticProcessing
} // Lima
