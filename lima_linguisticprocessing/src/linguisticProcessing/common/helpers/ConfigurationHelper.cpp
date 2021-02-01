/*
    Copyright 2002-2021 CEA LIST

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

#include "ConfigurationHelper.h"

#include "common/misc/Exceptions.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"

#include <string>

using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{

void ConfigurationHelper::getStringParameter(
    XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    const string& name,
    string& value,
    int flags,
    string default_value)
{
  string tmp;
  try
  {
    tmp = unitConfiguration.getParamsValueAtKey(name);
  }
  catch (NoSuchParam& )
  {
    if (flags & Flags::REQUIRED)
    {
      LOGINIT(m_loggingCategory.c_str());
      LERROR << "no param \"" << name << "\" in " << m_processUnitName << " group configuration";
      throw InvalidConfiguration();
    }

    tmp = default_value;
  }

  if (flags & Flags::NOT_EMPTY && tmp.size() == 0)
  {
    LOGINIT(m_loggingCategory.c_str());
    LERROR << "param \"" << name << "\" in " << m_processUnitName << " group configuration has zero length";
    throw InvalidConfiguration();
  }

  value = tmp;
}

string ConfigurationHelper::getStringParameter(
    XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    const string& name,
    int flags,
    string default_value)
{
  std::string value;
  getStringParameter(unitConfiguration, name, value, flags, default_value);
  return value;
}

void ConfigurationHelper::getIntParameter(
    XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    const string& name,
    int& value,
    int flags,
    int default_value)
{
  int tmp;
  try
  {
    tmp = QString::fromStdString(unitConfiguration.getParamsValueAtKey(name)).toInt();
  }
  catch (NoSuchParam& )
  {
    if (flags & Flags::REQUIRED)
    {
      LOGINIT(m_loggingCategory.c_str());
      LERROR << "no param \"" << name << "\" in " << m_processUnitName << " group configuration";
      throw InvalidConfiguration();
    }

    tmp = default_value;
  }

  if (flags & Flags::NOT_ZERO && tmp == 0)
  {
    LOGINIT(m_loggingCategory.c_str());
    LERROR << "param \"" << name << "\" in " << m_processUnitName << " group configuration is zero";
    throw InvalidConfiguration();
  }

  value = tmp;
}

int ConfigurationHelper::getIntParameter(
    XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    const string& name,
    int flags,
    int default_value)
{
  int value;
  getIntParameter(unitConfiguration, name, value, flags, default_value);
  return value;
}

bool ConfigurationHelper::isInitLazy()
{
  string lazy_init;
  MediaticData::MediaticData::single().getOptionValue(string("lazy-init"), lazy_init);
  if (lazy_init == "true")
  {
    return true;
  }
  return false;
}


}
}
