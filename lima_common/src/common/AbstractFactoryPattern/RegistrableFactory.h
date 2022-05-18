// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_REGISTRABLEFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_REGISTRABLEFACTORY_H

#include "MainFactory.h"
#include <deque>
#include <string>

namespace Lima
{

/**
  * @brief Defines a Factory that automatically register to a MainFactory singleton
  * 
  * Defines a template for a factory that automatically register to a
  * MainFactory. The registered factories can be retreived by the static method
  * getFactory
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @param Factory is the kind of Factory to register
  */
template<typename Factory>
class RegistrableFactory
{
public:

  virtual ~RegistrableFactory() = default;

  /**
   * @brief get the factory corresponding to the given id
   * @param id Id of Factory to get
   * @return Factory*
   */
  static std::shared_ptr<Factory> getFactory(const std::string& id);

  /**
   * @brief get the list of factories registered
   * @return deque of all ids of registered factories
   */
  static std::deque<std::string> getRegisteredFactories();

protected:
  /**
   * Constructor is protected, and must be redefined by subclass
   * @param id Id to use to register
   */
  RegistrableFactory(const std::string& id);

};

template<typename Factory>
RegistrableFactory<Factory>::RegistrableFactory(const std::string& id)
{
//   std::cerr << "RegistrableFactory<Factory>::RegistrableFactory(" << id << ")" << std::endl;
  MainFactory< RegistrableFactory<Factory> >::changeable().registerFactory(id, this);
}

template<typename Factory>
std::shared_ptr<Factory> RegistrableFactory<Factory>::getFactory(const std::string& id)
{
//   std::cerr << "RegistrableFactory<Factory>::getFactory(" << id << ")" << std::endl;
// 
  if (id.empty())
  {
    std::cerr << "Trying to access to factory with empty name!" << std::endl;
    throw InvalidConfiguration();
  }
  return std::dynamic_pointer_cast<Factory>(MainFactory< RegistrableFactory<Factory> >::changeable().getFactory(id));
}

template<typename Factory>
std::deque<std::string> RegistrableFactory<Factory>::getRegisteredFactories()
{
  return MainFactory< RegistrableFactory<Factory> >::single().getRegisteredFactories();
}

}

#endif
