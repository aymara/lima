// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECTFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECTFACTORY_H

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "RegistrableFactory.h"

namespace Lima
{

/**
  * @brief Defines Factory for an Initializable Object
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * 
  */
template<typename Object>
class InitializableObjectFactory : public RegistrableFactory<InitializableObjectFactory<Object> >
{
public:

  InitializableObjectFactory(const std::string& id) : 
    RegistrableFactory<InitializableObjectFactory<Object> >(id) 
    {};

  virtual Object* create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    typename Object::Manager* manager) const = 0;

  virtual ~InitializableObjectFactory() {}
};

} // Lima

#endif
