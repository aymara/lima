/*
    Copyright 2017 CEA LIST

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
/**
 * \file    LimaConfiguration.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef LIMACONFIGURATION_H
#define LIMACONFIGURATION_H

#include "LimaGuiExport.h"

#include <common/XMLConfigurationFiles/configurationStructure.h>

#include <QObject>
#include <QString>
#include <QFileInfo>

// #include <map>
// #include <deque>


// https://github.com/aymara/lima/wiki/Process-Units#flattokenizer

class QXmlStreamWriter;

namespace Lima 
{

namespace Gui 
{
  
namespace Config 
{

///
/// \brief This class figures a custom configuration; name and path
/// 
/// The idea was to have a class the configuration gui would instantiate to
/// create a new configuration. For each language, write the corresponding
/// config file with the writeFile function.
class LimaConfiguration : public QObject 
{
Q_OBJECT
public:
  LimaConfiguration(const QFileInfo& fileInfo = QFileInfo(), 
                    QObject* parent = nullptr);

  void loadFromFile(const QString& path);

  QString name() const { return m_name; }
  QString path() const { return m_path; }

  void setName(const QString& n) {m_name = n;}
  void setPath(const QString& p) {m_path = p;}

  const Common::XMLConfigurationFiles::ConfigurationStructure& configuration() const;
  
private:

  // for a future more complete configuration with process units
  // and resources parameters
//  std::map<QString, ProcessUnitPipelineConfiguration> m_processUnitPipelinesConfiguration;

//  std::map<QString, ProcessUnitConfiguration> m_processUnitsConfigurations;

  QString m_name;
  QString m_path;
  Common::XMLConfigurationFiles::ConfigurationStructure m_configuration;
};

#endif // LIMACONFIGURATION_H

} // Config
} // Gui
} // Lima
