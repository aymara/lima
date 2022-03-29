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
#ifndef LIMA_HELPERS_CONFIGURATION_HELPER_H
#define LIMA_HELPERS_CONFIGURATION_HELPER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <string>
#include <common/XMLConfigurationFiles/groupConfigurationStructure.h>

namespace Lima
{
namespace LinguisticProcessing
{

class LIMA_LP_HELPERS_EXPORT ConfigurationHelper
{
protected:
  ConfigurationHelper(const std::string& processUnitName, const std::string& loggingCategory)
    : m_processUnitName(processUnitName),
      m_loggingCategory(loggingCategory)
  {
  }

  enum Flags
  {
    REQUIRED = 0x1,
    NOT_EMPTY = 0x2,
    NOT_ZERO = 0x2
  };

  void getStringParameter(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      const std::string& name,
      std::string& value,
      int flags = Flags::REQUIRED,
      std::string default_value = "");

  std::string getStringParameter(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      const std::string& name,
      int flags = Flags::REQUIRED,
      std::string default_value = "");

  void getIntParameter(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      const std::string& name,
      int& value,
      int flags = Flags::REQUIRED,
      int default_value = 0);

  int getIntParameter(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      const std::string& name,
      int flags = Flags::REQUIRED,
      int default_value = 0);

  bool isInitLazy();

private:
  std::string m_processUnitName;
  std::string m_loggingCategory;
};

#define CONFIGURATIONHELPER_LOGGING_INIT(X) \
    namespace \
    { \
        inline string THIS_FILE_LOGGING_CATEGORY() \
        { \
            X; \
            return logger.zone().toStdString(); \
        } \
    }

}
}
#endif
