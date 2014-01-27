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

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_SIMPLEFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_SIMPLEFACTORY_H

// #include "MainFactory.h"
#include "InitializableObjectFactory.h"

namespace Lima
{

/**
  * @brief Defines a Factory to create Object of type Base
  * @file SimpleFactory.h
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * 
  */
template<typename Base,typename Object>
class SimpleFactory : public InitializableObjectFactory<Base>
{
public:

  /**
    * @brief Constructor
    * @param id @b IN : classId used to register to the main factory
    */
  SimpleFactory(const std::string& id);

  virtual ~SimpleFactory() {}

  /**
    * @brief create an object of class Object.
    * @param unitConfiguration @b IN : object configuration parameters
    * @return Object* pointer to the created Object
    * @throw InvalidConfiguration if 'class' parameter does not exist or \
    *        if no appropriate factory is available
    * Constraint : Object must be an InitializableObject
    */
  virtual Base* create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    typename Base::Manager* manager) const;

};

template<typename Base,typename Object>
SimpleFactory<Base,Object>::SimpleFactory(const std::string& classId) :
  InitializableObjectFactory<Base>(classId)
{}

template<typename Base,typename Object>
Base* SimpleFactory<Base,Object>::create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    typename Base::Manager* manager) const
{
  Base* obj=new Object();
  obj->setId(unitConfiguration.getName());
  obj->init(unitConfiguration,manager);
  return obj;
}

} // Lima

#endif
