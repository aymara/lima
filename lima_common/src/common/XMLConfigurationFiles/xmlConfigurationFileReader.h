// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @date       begin Wed May, 25 2022
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  */

#ifndef XMLCONFIGURATIONFILEREADER_H
#define XMLCONFIGURATIONFILEREADER_H

#include "common/LimaCommon.h"
#include "configurationStructure.h"

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class XmlConfigurationFileReaderPrivate;
/**
 * This is a SAX document handler the  configuration files
 * @author Gael de Chalendar
 */
class LIMA_XMLCONFIGURATIONFILES_EXPORT XmlConfigurationFileReader
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlConfigurationFileReader(ConfigurationStructure& theConfiguration);
  ~XmlConfigurationFileReader();

  bool parse(QIODevice *device);

  QString errorString() const;

private:
  XmlConfigurationFileReaderPrivate* m_d;
};

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
