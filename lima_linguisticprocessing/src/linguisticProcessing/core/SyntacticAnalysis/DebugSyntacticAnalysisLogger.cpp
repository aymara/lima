// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SALOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  auto tokenList = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));
  if (tokenList == 0) {
      SALOGINIT;
      LERROR << "no AnalysisGraph ! abort";
      return MISSING_DATA;
  }
  auto syntacticData = std::dynamic_pointer_cast<SyntacticData>(analysis.getData("SyntacticData"));
  if (syntacticData == 0) {
      SALOGINIT;
      LERROR << "no SyntacticData ! abort";
      return MISSING_DATA;
  }

  std::ofstream ofs;
  if (!openLogFile(ofs,metadata->getMetaData("FileName"))) {
      SALOGINIT;
      LERROR << "DebugSyntacticAnalysisLogger::Cannot open log file";
      return CANNOT_OPEN_FILE_ERROR;
  }
  //ofs << endl << "Printing syntactic analysis result :" << endl;

  //cout << endl;
  if (m_displayChains) {
    LDEBUG << "call of displayChains";
    SyntacticAnalysisTools::displayChains(tokenList.get(), ofs);
  }
  if (m_displayDependencies) {
    LDEBUG << "call of displayDependancies";
    SyntacticAnalysisTools::displayDependancies(*syntacticData,
                                                m_language,ofs,m_displayLemmas);
  }
  if (m_displayRelationsOrderOfArguments) {
    LDEBUG << "call of displayRelationsOrderOfArguments";
    SyntacticAnalysisTools::displayRelationsOrderOfArguments(*syntacticData,
                                                             m_language,
                                                             ofs);
  }
  if (m_displayRelationsDistanceOfArguments) {
    LDEBUG << "call of displayRelationsDistanceOfArguments";
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
