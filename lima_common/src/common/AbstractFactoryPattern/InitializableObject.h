// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECT_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECT_H

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "InitializableObjectManager.h"
#include "InitializableObjectFactory.h"

namespace Lima
{

/**
  * @brief defines an object that can be create and initialized by a SimpleFactory
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @param Base must be an InitializableObject, is a base Type.
  * @param InitializationParameters parameters that object can use at initialization. \
           this parameters are available through the associated Manager
  * @see Manager     
  */
template<typename Base,typename InitializationParameters = NoParameters>
class InitializableObject {
public:

  InitializableObject() : m_id() {}
  InitializableObject(const InitializableObject& object) { m_id = object.m_id; }
  InitializableObject& operator=(const InitializableObject& object) { m_id = object.m_id; return *this; }
  
  /**
  * Manager is the type of the Manager associated to the initializableObject.
  * This type is an instanciation of InitializableObjectManager template with
  * same template parameters as InitializableObject.
  * The manager is responsible for creating and manager objects of type specified
  * by the Base template parameters.
  * @brief defines associated Manager
  */
  typedef InitializableObjectManager<Base,InitializationParameters> Manager;

  /**
  * Factory is the type of the Factory associated to the initializableObject.
  * This type is an instanciation of InitializableObjectFactory template with
  * the parameter Base.
  * @brief defines associated Factory
  */
  typedef InitializableObjectFactory<Base> Factory;
  
  virtual ~InitializableObject() {}

  /**
  * @brief initialize with parameters from configuration file.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @param manager @IN : manager that asked for initialization and carries init params
  * Use it to initialize other objects of same kind.
  * @throw InvalidConfiguration when parameters are invalids.
  */
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) = 0;
  
  /**
   * @brief get the object id
   * @return the object id
   */
  const std::string& getId() const { return m_id; }
  
  /**
   * @brief set the object id
   * @param id object id
   */
  void setId(const std::string& id) { m_id=id; }
  
protected:
  std::string m_id;
  
};

} // Lima

#endif
