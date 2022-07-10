// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    typename Base::Manager* manager) const override;

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
