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

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORYWIN32_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_MAINFACTORYWIN32_H

#include <typeinfo>

#ifdef WIN32

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

  // FIXME: Wordaround to return a shared pointer from a void* pointer : to keep main map
  // valid, do not delete shared pointer.
  inline static void deleter(Factory *) { }

private:

  MainFactory();
  static MainFactory<Factory>* s_instance; // use this! always points to the same object
  static MainFactory<Factory> s_instance_one_per_dll; // only used in initialisation

};

template<typename Factory>
MainFactory<Factory>::MainFactory() {}

template<typename Factory>
MainFactory<Factory> MainFactory<Factory>::s_instance_one_per_dll;

template<typename Factory>
MainFactory<Factory>* MainFactory<Factory>::s_instance = 0;

template <class T>
T *set_the_global(T *candidate)
{
  try {
#ifdef DEBUG_FACTORIES
    std::cerr << "set_the_global: " << (void*)candidate << std::endl;
    std::cerr << "set_the_global: " << typeid(*candidate).name() << std::endl;
#endif
    FactoryMap::iterator r = MainFactoriesMap::mainFactoriesMap().find(std::string(typeid(*candidate).name()));
    if(r == MainFactoriesMap::mainFactoriesMap().end()) {
      MainFactoriesMap::mainFactoriesMap().insert(std::make_pair(std::string(typeid(*candidate).name()),(void*)candidate));
#ifdef DEBUG_FACTORIES
      std::cerr << "set_the_global set to: " << (void*)candidate << std::endl;
#endif
      return candidate;  // new class: use it as global storage location
    } else {
#ifdef DEBUG_FACTORIES
      std::cerr << "set_the_global got: " << r->second << std::endl;
#endif
      return (T*)(r->second);  // class already has global storage location
    }
  } catch (const std::__non_rtti_object& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Unable to instantiate a factory: non RTTI object exception catched." << std::endl;
//#endif
    return 0;
  } catch (const std::bad_typeid& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Got a null factory: bad_typeid exception catched." << std::endl;
    std::cerr << "This will probably crash." << std::endl;
//#endif
    return 0;
  }
}


template<typename Factory>
const MainFactory<Factory>& MainFactory<Factory>::single()
{
  if (s_instance==0)
  {
    s_instance=set_the_global(&MainFactory<Factory>::s_instance_one_per_dll);
  }
  try {
#ifdef DEBUG_FACTORIES
  std::cerr << "MainFactory<Factory>::single for " << typeid(*s_instance).name() << " returns " << (void*)s_instance << std::endl;
#endif
  } catch (const std::__non_rtti_object& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Got an invalid factory: non RTTI object exception catched." << std::endl;
    std::cerr << "This will probably crash." << std::endl;
//#endif
  } catch (const std::bad_typeid& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Got a null factory: bad_typeid exception catched." << std::endl;
    std::cerr << "This will probably crash." << std::endl;
//#endif
  }
  return *s_instance;
}

template<typename Factory>
MainFactory<Factory>& MainFactory<Factory>::changeable()
{
  if (s_instance==0)
  {
    s_instance=set_the_global(&MainFactory<Factory>::s_instance_one_per_dll);
  }
  try {
#ifdef DEBUG_FACTORIES
    std::cerr << "MainFactory<Factory>::changeable for " << typeid(*s_instance).name() << " returns " << (void*)s_instance << std::endl;
#endif
  } catch (const std::__non_rtti_object& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Got an invalid factory: non RTTI object exception catched." << std::endl;
    std::cerr << "This will probably crash." << std::endl;
//#endif
  } catch (const std::bad_typeid& ) {
//#ifdef DEBUG_FACTORIES
    std::cerr << "Got a null factory: bad_typeid exception catched." << std::endl;
    std::cerr << "This will probably crash." << std::endl;
//#endif
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
  FactoryMap::const_iterator factItr = MainFactoriesMap::mainFactoriesMap().find(classId);
  if (factItr==MainFactoriesMap::mainFactoriesMap().end())
  {
    std::cerr << "1: No AbstractFactory for classId '" << classId << "' ! " << std::endl;
    std::cerr << "   Should be initialized at library loading time." << std::endl;
    std::cerr << "   Maybe you forgot to link with the library defining this classId ?" << std::endl;
    std::cerr << "   Or you forgot to define it as a plugin using, in its CMakeLists.txt," << std::endl;
    std::cerr << "   DECLARE_LIMA_PLUGIN, instead of add_library ?" << std::endl;
    throw InvalidConfiguration();
  }
#ifdef DEBUG_FACTORIES
  std::cerr << "MainFactory<Factory>::getFactory("<<classId<< ") got " << ((Factory*)factItr->second) << std::endl;
#endif
  return std::shared_ptr<Factory>(static_cast<Factory*>(factItr->second), deleter);
}

template<typename Factory>
std::shared_ptr<Factory> MainFactory<Factory>::getFactory(const std::string& classId)
{
  if (classId.empty())
  {
    std::cerr << "Trying to access to factory with empty name!" << std::endl;
    throw InvalidConfiguration();
  }
  FactoryMap::const_iterator factItr = MainFactoriesMap::mainFactoriesMap().find(classId);
  if (factItr==MainFactoriesMap::mainFactoriesMap().end())
  {
    std::cerr << "2: No AbstractFactory for classId '" << classId << "' ! " << std::endl;
    std::cerr << "   Main factory (this) is: " << (void*) this << std::endl;
    std::cerr << "   Should be initialized at library loading time." << std::endl;
    std::cerr << "   Maybe you forgot to link with the library defining this classId ?" << std::endl;
    std::cerr << "   Or you forgot to define it as a plugin using, in its CMakeLists.txt," << std::endl;
    std::cerr << "   DECLARE_LIMA_PLUGIN, instead of add_library ?" << std::endl;
    throw InvalidConfiguration();
  }
#ifdef DEBUG_FACTORIES
  std::cerr << "MainFactory<Factory>::getFactory("<<classId<< ") got " << ((Factory*)factItr->second) << std::endl;
#endif
  return std::shared_ptr<Factory>(static_cast<Factory*>(factItr->second), deleter);
}

template<typename Factory>
void MainFactory<Factory>::registerFactory(
  const std::string& classId,
  Factory* fact)
{
#ifdef DEBUG_FACTORIES
  std::cerr << "Main factory (this) is: " << (void*)this << ". register factory for classId '" << classId << "'" << std::endl;
#endif
  if (MainFactoriesMap::mainFactoriesMap().find(classId)!=MainFactoriesMap::mainFactoriesMap().end())
  {
//      std::cerr << "Factory for classId '" << classId << "' already exists do not replace it." << std::endl;
//     throw InvalidConfiguration();
  }
  else
  {
    MainFactoriesMap::mainFactoriesMap()[classId]=fact;
  }
}

template<typename Factory>
std::deque<std::string> MainFactory<Factory>::getRegisteredFactories() const
{
  std::deque<std::string> result;
  for (FactoryMap::const_iterator factItr=MainFactoriesMap::mainFactoriesMap().begin();
       factItr!=MainFactoriesMap::mainFactoriesMap().end();
       factItr++)
  {
    result.push_back(factItr->first);
  }
  return result;
}

} // Lima

#endif // WIN32 test

#endif
