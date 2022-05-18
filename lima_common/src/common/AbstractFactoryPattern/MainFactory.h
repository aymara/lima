// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORY_H

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"

#include <map>
#include <deque>
#include <string>
#include <typeinfo>

#include "MainFactoriesMap.h"

namespace Lima
{

  /**
   * @brief Defines the MainFactory to create Object
   * @file
   * @author Benoit Mathieu <mathieub@zoe.cea.fr>
   * The main factory accepts Factory registrations
   * @param Factory Type of factory to manage
   */
  template<typename Factory>
  class MainFactory
  {

  public:

    /**
     * @brief const singleton accessor
     */
    static const MainFactory<Factory>& single();

    /**
     * @brief singleton accessor
     */
    static MainFactory<Factory>& changeable();

    /**
     * @brief destroy factory and all registered factories
     */
    static void cleanup();

    /**
     * @brief create an Object using the appropriate registered factory.
     * @param unitConfiguration @b IN : object configuration parameters
     * @return Object* pointer to the created Object
     * @throw InvalidConfiguration if 'class' parameter does not exist or \
     *        if no appropriate factory is available
     * @throw UndefinedMethod if corresponding create method is not defined
     *
     * Use the 'class' parameter of unitConfiguration to select the factory to
     * which delegate the creation of the object.
     */
    const std::shared_ptr<Factory> getFactory(const std::string& classId) const;

    /**
     * @brief create an Object using the appropriate registered factory.
     * @param unitConfiguration @b IN : object configuration parameters
     * @return Object* pointer to the created Object
     * @throw InvalidConfiguration if 'class' parameter does not exist or \
     *        if no appropriate factory is available
     * @throw UndefinedMethod if corresponding create method is not defined
     *
     * Use the 'class' parameter of unitConfiguration to select the factory to
     * which delegate the creation of the object.
     */
    std::shared_ptr<Factory> getFactory(const std::string& classId);

    /**
     * @brief register a factory with an id
     * @param classId @b IN : classId for the factory
     * @param fact @b IN : factory to register
     * @throw InvalidConfiguration if already a factory for this classId
     */
    void registerFactory(
      const std::string& classId,
      Factory* fact);

    /**
     * @brief show registered classId
     * @return list of registered classId
     */
    std::deque<std::string> getRegisteredFactories() const;

    virtual ~MainFactory();

  private:

    MainFactory();

    typedef std::map<std::string, std::shared_ptr<Factory>> FactoryMap;
    typedef typename FactoryMap::const_iterator FactoryMapCItr;
    FactoryMap m_factories;

  };

  template<typename Factory>
  MainFactory<Factory>::MainFactory() : m_factories() {}

  template<typename Factory>
  const MainFactory<Factory>& MainFactory<Factory>::single()
  {
    typename MainFactoryMap::const_iterator it = MainFactoriesMap::get().find(typeid(Factory).name());
    if (MainFactoriesMap::get().end() == it)
      MainFactoriesMap::get().insert(std::pair<std::string, void*>(typeid(Factory).name(), new MainFactory<Factory>()));

    it = MainFactoriesMap::get().find(typeid(Factory).name());
    if (MainFactoriesMap::get().end() == it)
      throw LimaException("This can't happen.");

    return *((const MainFactory<Factory>*)(it->second));
  }

  template<typename Factory>
  MainFactory<Factory>& MainFactory<Factory>::changeable()
  {
    typename MainFactoryMap::iterator it = MainFactoriesMap::get().find(typeid(Factory).name());
    if (MainFactoriesMap::get().end() == it)
      MainFactoriesMap::get().insert(std::pair<std::string, void*>(typeid(Factory).name(), new MainFactory<Factory>()));

    it = MainFactoriesMap::get().find(typeid(Factory).name());
    if (MainFactoriesMap::get().end() == it)
      throw LimaException("This can't happen.");

    return *((MainFactory<Factory>*)(it->second));
  }


  template<typename Factory>
  void MainFactory<Factory>::cleanup()
  {
  }

  template<typename Factory>
  MainFactory<Factory>::~MainFactory()
  {
  }

  template<typename Factory>
  const std::shared_ptr<Factory> MainFactory<Factory>::getFactory(const std::string& classId) const
  {
    if (classId.empty())
    {
      std::cerr << "Trying to access to factory with empty name!" << std::endl;
      throw InvalidConfiguration();
    }
    FactoryMapCItr factItr=m_factories.find(classId);
    if (factItr==m_factories.end())
    {
      std::cerr << "1: No AbstractFactory for classId '" << classId << "' ! " << std::endl;
      std::cerr << "   Should be initialized at library loading time." << std::endl;
      std::cerr << "   Maybe you forgot to link with the library defining this classId ?" << std::endl;
      std::cerr << "   Or you forgot to define it as a plugin using, in its CMakeLists.txt," << std::endl;
      std::cerr << "   DECLARE_LIMA_PLUGIN, instead of add_library ?" << std::endl;
      throw InvalidConfiguration();
    }
    return factItr->second;
  }

  template<typename Factory>
  std::shared_ptr<Factory> MainFactory<Factory>::getFactory(const std::string& classId)
  {
    if (classId.empty())
    {
      std::cerr << "Trying to access to factory with empty name!" << std::endl;
      throw InvalidConfiguration();
    }
    FactoryMapCItr factItr=m_factories.find(classId);
    if (factItr==m_factories.end())
    {
      std::cerr << "2: No AbstractFactory for classId '" << classId << "' ! " << std::endl;
      std::cerr << "   Should be initialized at library loading time." << std::endl;
      std::cerr << "   Maybe you forgot to link with the library defining this classId ?" << std::endl;
      std::cerr << "   Or you forgot to define it as a plugin using, in its CMakeLists.txt," << std::endl;
      std::cerr << "   DECLARE_LIMA_PLUGIN, instead of add_library ?" << std::endl;
      throw InvalidConfiguration();
    }
    return factItr->second;
  }

  template<typename Factory>
  void MainFactory<Factory>::registerFactory(
    const std::string& classId,
    Factory* fact)
  {
    // std::cerr << "register factory for classId '" << classId << "'" << std::endl;
    if (m_factories.find(classId)!=m_factories.end())
    {
      std::cerr << "Factory for classId '" << classId << "' already exists do not replace it." << std::endl;
      // throw InvalidConfiguration();
    }
    else
    {
      m_factories[classId]= std::shared_ptr<Factory>(fact);
    }
  }

  template<typename Factory>
  std::deque<std::string> MainFactory<Factory>::getRegisteredFactories() const
  {
    std::deque<std::string> result;
    for (FactoryMapCItr factItr=m_factories.begin();
         factItr!=m_factories.end();
         factItr++)
         {
           result.push_back(factItr->first);
         }
         return result;
  }

} // Lima

#endif
