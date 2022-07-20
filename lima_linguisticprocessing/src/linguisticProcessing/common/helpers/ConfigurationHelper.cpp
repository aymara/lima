// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  Lima::Common::MediaticData::MediaticData::single().getOptionValue(string("lazy-init"), lazy_init);
  if (lazy_init == "true")
  {
    return true;
  }
  return false;
}


}
}
