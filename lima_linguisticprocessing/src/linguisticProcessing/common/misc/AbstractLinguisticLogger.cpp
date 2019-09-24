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

bool AbstractLinguisticLogger::openLogFile(std::ofstream& output,
                                           const std::string& sourceFile) const
{
  auto outputFile = sourceFile + m_outputSuffix;
  if (m_append) {
    output.open(outputFile, ios::app | std::ofstream::binary);
  }
  else {
    output.open(outputFile, std::ofstream::binary);
  }
  if (!output.good())
  {
    CORECLIENTLOGINIT;
    LERROR << "Can't open log file" << outputFile;
  }
  return output.good();
}

} // end namespace
} // end namespace
