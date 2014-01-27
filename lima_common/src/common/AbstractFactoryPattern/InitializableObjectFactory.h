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
