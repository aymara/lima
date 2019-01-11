/*
    Copyright 2002-2019 CEA LIST

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
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORYLINUX_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORYLINUX_H

#ifndef WIN32 // We are on Linux

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
    static MainFactory<Factory>* s_instance;

    typedef std::map<std::string,std::shared_ptr<Factory>> FactoryMap;
    typedef typename FactoryMap::const_iterator FactoryMapCItr;
    FactoryMap m_factories;

  };

  template<typename Factory>
  MainFactory<Factory>::MainFactory() : m_factories() {}

  template<typename Factory>
  MainFactory<Factory>* MainFactory<Factory>::s_instance(0);

  template<typename Factory>
  const MainFactory<Factory>& MainFactory<Factory>::single()
  {
    if (s_instance==0)
    {
      s_instance=new MainFactory<Factory>();
    }
    return *s_instance;
  }

  template<typename Factory>
  MainFactory<Factory>& MainFactory<Factory>::changeable()
  {
    if (s_instance==0)
    {
      s_instance=new MainFactory<Factory>();
    }
    return *s_instance;
  }


  template<typename Factory>
  void MainFactory<Factory>::cleanup()
  {
    delete s_instance;
    s_instance=0;
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
//     std::cerr << "register factory for classId '" << classId << "'" << std::endl;
    if (m_factories.find(classId)!=m_factories.end())
    {
      std::cerr << "Factory for classId '" << classId << "' already exists do not replace it." << std::endl;
      //     throw InvalidConfiguration();
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

#endif // WIN32 test

#endif
