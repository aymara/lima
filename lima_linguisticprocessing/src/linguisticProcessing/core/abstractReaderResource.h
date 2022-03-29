/*
    Copyright 2005-2021 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_XMLREADER_ABSTRACTREADERRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_XMLREADER_ABSTRACTREADERRESOURCES_H

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"
#include "common/AbstractFactoryPattern/InitializableObjectManager.h"

namespace Lima {
namespace DocumentsReader {

/**
  * @brief like linguistic resource abstraction.
  * AbstractXMLParser and DocumentPropertyType inherits from this class
  * @author Olivier Mesnard <olivier.mesnard@cea.fr>
  *
  *
  */
struct ReaderResourceInitializationParameters {
};

class AbstractReaderResource : public Lima::InitializableObject<AbstractReaderResource,ReaderResourceInitializationParameters>
{
public:

  /**
  * @brief initialize with parameters from configuration file and languageId.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @param manager @IN : manager that asked for initialization and carries parameters.
  * @throw Lima::InvalidConfiguration when parameters are invalids.
  */
  virtual void init(
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override = 0;

};

} // DocumentsReader
} // Lima


#endif
