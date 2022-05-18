// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       AbstractLinguisticLogger.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Aug  3 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     LinguisticProcessing
 * 
 ***********************************************************************/

#include "AbstractLinguisticLogger.h"
#include <fstream>

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace LinguisticProcessing {

AbstractLinguisticLogger::
AbstractLinguisticLogger(const std::string& defaultSuffix):
m_outputSuffix(defaultSuffix),
m_append(false)
{}

AbstractLinguisticLogger::~AbstractLinguisticLogger()
{}

void AbstractLinguisticLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* /*manager*/)

{
  try
  {
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& ) {} // ignored : keep default suffix

  try
  {
    string val=unitConfiguration.getParamsValueAtKey("append");
    if (val=="true" || val=="yes" || val=="1") {
      m_append=true;
    }
    else {
      m_append=false;
    }
  }
  catch (NoSuchParam& ) {} // keep default value
}

bool AbstractLinguisticLogger::
openLogFile(std::ofstream& output,
            const std::string& sourceFile) const
{
  string outputFile=sourceFile + m_outputSuffix;
  if (m_append) {
    output.open(outputFile.c_str(),ios::app | std::ofstream::binary);
  }
  else {
    output.open(outputFile.c_str(), std::ofstream::binary);
  }
  if (!output.good())
  {
    CORECLIENTLOGINIT;
    LERROR << "Can't open log file" << outputFile.c_str();
  }
  return output.good();
}

} // end namespace
} // end namespace
