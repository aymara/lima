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
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECTMANAGER_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_INITIALIZABLEOBJECTMANAGER_H

#include "ObjectManager.h"
#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/DynamicLibrariesManager.h"
#include "InitializableObjectFactory.h"

namespace Lima
{

/**
* Manage initialization of InitializableObjects using configuration module and parameters
* @brief Manager for InitializableObjects
* @param Object should be InitializableObject. This is the base of managed objects.
* @param InitializationParameters type parameters
*/
template <typename Object,typename InitializationParameters = NoParameters>
class InitializableObjectManager : public ObjectManager<Object>
{
public:

  /**
  * @brief constructor with configuration module
  * @param conf configuration module
  */
  InitializableObjectManager(
    const Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf,
    const InitializationParameters& params);

  /**
  * @brief constructor with configuration module
  * @param conf configuration module
  */
  InitializableObjectManager(
    const Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf);

  /**
   * @brief virtual destructor
   */
  virtual ~InitializableObjectManager() {}
  
  /**
  * @brief get Initialization Parameters
  */
  const InitializationParameters& getInitializationParameters() const;

  /**
  * @brief get Initialization Parameters
  */
  InitializationParameters& getInitializationParameters();

  /**
  * @brief get ModuleConfigurationStructure
  */
  const Common::XMLConfigurationFiles::ModuleConfigurationStructure& 
    getModuleConfigurationStructure() const;
  Common::XMLConfigurationFiles::ModuleConfigurationStructure& 
    getModuleConfigurationStructure();

protected:

  /**
   * @brief create Object. 
   * @param obj 
   * @param gconf 
   */
  virtual Object* createObject(const std::string& id) override;

private:

  Common::XMLConfigurationFiles::ModuleConfigurationStructure m_conf;
  InitializationParameters m_params;

};

template <typename Object,typename InitializationParameters>
InitializableObjectManager<Object,InitializationParameters>::InitializableObjectManager(
  const Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf,
  const InitializationParameters& params) :
    ObjectManager<Object>(),
    m_conf(conf),
    m_params(params)
{
//     std::cerr << this << "InitializableObjectManager::InitializableObjectManager " << m_conf.getName() << "'" << std::endl;
}

template <typename Object,typename InitializationParameters>
const InitializationParameters& InitializableObjectManager<Object,InitializationParameters>::getInitializationParameters() const
{
  return m_params;
}

template <typename Object,typename InitializationParameters>
InitializationParameters& InitializableObjectManager<Object,InitializationParameters>::getInitializationParameters()
{
  return m_params;
}

template <typename Object,typename InitializationParameters>
const Common::XMLConfigurationFiles::ModuleConfigurationStructure& 
InitializableObjectManager<Object,InitializationParameters>::getModuleConfigurationStructure() const {
  return m_conf;
}

template <typename Object,typename InitializationParameters>
Common::XMLConfigurationFiles::ModuleConfigurationStructure& 
InitializableObjectManager<Object,InitializationParameters>::getModuleConfigurationStructure() {
  return m_conf;
}

template <typename Object,typename InitializationParameters>
InitializableObjectManager<Object,InitializationParameters>::InitializableObjectManager(
  const Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf) :
    ObjectManager<Object>(),
    m_conf(conf)
{}

template <typename Object,typename InitializationParameters>
Object* InitializableObjectManager<Object,InitializationParameters>::createObject(
  const std::string& id)
{
#ifdef DEBUG_CD
  ABSTRACTFACTORYPATTERNLOGINIT;
  LDEBUG << "InitializableObjectManager::createObject" << this << id 
         << "from module" << &m_conf << m_conf.getName().c_str();
#endif
  Object* obj = nullptr;
  try
  {
    Common::XMLConfigurationFiles::GroupConfigurationStructure& gconf=m_conf.getGroupNamed(id);

    std::string classId;
    try
    {
      classId=gconf.getAttribute("class");
//       std::cerr << "class id is '" << classId.c_str() << "' !" << std::endl;
    }
    catch (Common::XMLConfigurationFiles::NoSuchAttribute& )
    {
      std::cerr << "no class attribute in unitConfiguration "<< id << std::endl;
      throw InvalidConfiguration("no class attribute in unitConfiguration !");
    }
    if (classId.empty())
    {
      std::cerr << "Empty class attribute in unitConfiguration " << id << "/class"  << std::endl;
      throw InvalidConfiguration("Empty class attribute in unitConfiguration!");
    }
    try
    {
      std::string libs=gconf.getAttribute("lib");
      // parse string in case several libs are 
      std::string::size_type begin=0;
      std::string::size_type i=libs.find(",",begin);
      while (i!=std::string::npos) {
        std::string libName(libs,begin,i-begin);
//         std::cerr << "import library '" << libName.c_str() << "'" << std::endl;
        Common::DynamicLibrariesManager::changeable().loadLibrary(libName);
        begin=i+1;
        i=libs.find(",",begin);
      }
      std::string libName(libs,begin);
//       std::cerr << "import library '" << libName.c_str() << "'" << std::endl;
      Common::DynamicLibrariesManager::changeable().loadLibrary(libName);
    }
    catch (Common::XMLConfigurationFiles::NoSuchAttribute& )
    {
      // optional
      //LDEBUG << "no lib attribute in unitConfiguration";
    }

//     std::cerr << "class id(2) is '" << classId.c_str() << "' !" << std::endl;
    const std::shared_ptr<InitializableObjectFactory<Object>> fact=InitializableObjectFactory<Object>::getFactory(classId);
    obj=fact->create(gconf,this);
  }
  catch (Common::XMLConfigurationFiles::NoSuchGroup& )
  {
    std::stringstream s_mess;
    s_mess << "No Object '" << id.c_str() << "' defined in module '" << m_conf.getName().c_str() << "'";
    std::cerr << s_mess.str().c_str() << std::endl;
    throw InvalidConfiguration( s_mess.str() );
  }
  return obj;
}

} // Lima

#endif
