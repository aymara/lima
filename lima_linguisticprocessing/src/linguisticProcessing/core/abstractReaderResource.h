// Copyright 2005-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
