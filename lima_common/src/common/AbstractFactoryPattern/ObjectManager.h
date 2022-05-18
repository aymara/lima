// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_OBJECTMANAGER_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_OBJECTMANAGER_H

#include "MainFactory.h"
#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

namespace Lima
{

/**
* Manager for objects. The manager create object when necessay, store them,
* avoiding to create same object twice.
* @brief Manager for Objects
* @param Objects to manage
*/
template <typename Object>
class ObjectManager
{
public:

  /**
  *  @brief ObjectManager destructor
  */
  virtual ~ObjectManager();
  
  /**
   * If object doesn't exists, call the create method
   * @brief get object of the given id
   * @param id Id of the object to return
   * @return pointer to object corresponding to id
   * @throw InvalidConfiguration if failed to create object from given configuration
   */
  virtual Object* getObject(const std::string& id);
  
protected:

  /**
   * @brief create the required object.
   * @param id Id of the object to return
   * @return pointer to object corresponding to id
   * @throw InvalidConfiguration if failed to create object from given configuration
   */
  virtual Object* createObject(const std::string& id) = 0;
  
private:

  typedef typename std::map<std::string,Object*> ObjectMap;
  typedef typename std::map<std::string,Object*>::iterator ObjectMapItr;
  ObjectMap m_objects;
};

template <typename Object>
ObjectManager<Object>::~ObjectManager()
{
  for (ObjectMapItr it=m_objects.begin();
       it!=m_objects.end();
       it++)
  {
    delete it->second;
    it->second=0;
  }
  m_objects.clear();
}

template <typename Object>
Object* ObjectManager<Object>::getObject(
  const std::string& id) {
  ObjectMapItr ioItr=m_objects.find(id);
  if (ioItr==m_objects.end())
  {
    Object* obj=this->createObject(id);
    m_objects[id]=obj;
    return obj;
  }
  return ioItr->second;
}

} // Lima

#endif
