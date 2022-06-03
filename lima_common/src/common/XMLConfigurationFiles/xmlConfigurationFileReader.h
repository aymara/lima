/*
    Copyright 2022 CEA LIST

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
