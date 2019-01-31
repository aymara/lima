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

#include "LinguisticResources.h"

#include "AbstractAccessResource.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/tools/FileUtils.h"

#include <QFileInfo>

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;


namespace Lima
{
template<> MainFactory< RegistrableFactory< InitializableObjectFactory< LinguisticProcessing::AbstractResource > > >* MainFactory< RegistrableFactory< InitializableObjectFactory< LinguisticProcessing::AbstractResource > > >::s_instance(0);

template<> MainFactory< RegistrableFactory< InitializableObjectFactory< LinguisticProcessing::LinguisticResources > > >* MainFactory< RegistrableFactory< InitializableObjectFactory< LinguisticProcessing::LinguisticResources > > >::s_instance(0);

  namespace LinguisticProcessing
{

class LinguisticResourcesPrivate
{
friend class LinguisticResources;

public:
  LinguisticResourcesPrivate();
  virtual ~LinguisticResourcesPrivate();

  void clear();

  std::map<MediaId,AbstractResource::Manager*> m_resourcesManagers;

  // private member functions
  void includeResources(Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
                        Common::XMLConfigurationFiles::ModuleConfigurationStructure& includeModule);

};

LinguisticResourcesPrivate::LinguisticResourcesPrivate() {}

LinguisticResourcesPrivate::~LinguisticResourcesPrivate()
{
  clear();
}

void LinguisticResourcesPrivate::clear()
{
  for (std::map<MediaId,AbstractResource::Manager*>::iterator it=m_resourcesManagers.begin();
       it!=m_resourcesManagers.end();
       it++)
  {
    delete it->second;
    it->second=0;
  }
  m_resourcesManagers.clear();
}

LinguisticResources::LinguisticResources() :
    Singleton<LinguisticResources>(),
    m_d(new LinguisticResourcesPrivate())
{}

// LinguisticResources::LinguisticResources(const LinguisticResources& lr) :
//     Singleton<LinguisticResources>(),
//     m_d(new LinguisticResourcesPrivate(*lr.m_d))
// {}

LinguisticResources::~LinguisticResources()
{
  delete m_d;
}

void LinguisticResources::clearResources()
{
  m_d->clear();
}

AbstractResource* LinguisticResources::getResource(MediaId lang,const std::string& id) const
{
#ifdef DEBUG_LP
  RESOURCESLOGINIT;
  LDEBUG << "LinguisticResources::getResource" << this << lang << id.c_str();
#endif
  std::map<MediaId,AbstractResource::Manager*>::const_iterator it=m_d->m_resourcesManagers.find(lang);
  if (it==m_d->m_resourcesManagers.end())
  {
    RESOURCESLOGINIT;
    LERROR << "In getLanguageResources : language " << (int)lang << " is not initialized !";
    throw LanguageNotInitialized(lang);
  }
  return it->second->getObject(id);
}

void LinguisticResources::initLanguage(
  MediaId lang,
  Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule,
  bool registerMainKeysInStringPool)
{
  RESOURCESLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "LinguisticResources::initLanguage" << this << confModule.getName();
#endif
  ResourceInitializationParameters params;
  params.language=lang;
  m_d->m_resourcesManagers[lang]=new AbstractResource::Manager(confModule,params);
  Common::XMLConfigurationFiles::ModuleConfigurationStructure& module=
    m_d->m_resourcesManagers[lang]->getModuleConfigurationStructure();
  m_d->includeResources(module,module);

  if (registerMainKeysInStringPool)
  {
    // Initialize FsaStringsPool : main keys must be registered at beginning
    try {
      string mainkeys=confModule.getParamValueAtKeyOfGroupNamed("mainKeys","FsaStringsPool");
      LINFO << "load mainKeys " << mainkeys << " into StringPool";
      AbstractResource* res=getResource(lang,mainkeys);
      AnalysisDict::AbstractAccessResource* access=static_cast<AnalysisDict::AbstractAccessResource*>(res);
      access->setMainKeys(true);
      Common::MediaticData::MediaticData::changeable().stringsPool(lang).registerMainKeys(access->getAccessByString());
    } catch (NoSuchGroup& )
    {
      LERROR << "No group 'FsaStringsPool' defined in Resource module. Can't register any main keys";
      throw InvalidConfiguration();
    } catch (NoSuchParam& )
    {
      LERROR << "No param 'mainKeys' defined in FsaStringsPool group. Can't register any main keys";
      throw InvalidConfiguration();
    }
  }
}

void LinguisticResourcesPrivate::
includeResources(Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
                 Common::XMLConfigurationFiles::ModuleConfigurationStructure& includeModule)
{
  RESOURCESLOGINIT;
#ifdef DEBUG_LP
        LDEBUG << "LinguisticResourcesPrivate::includeResources" << this << &module << module.getName() << &includeModule << includeModule.getName();
#endif
  try {
    deque<string> includeList=includeModule.getListValuesAtKeyOfGroupNamed("includeList","include");
    for (deque<string>::const_iterator it=includeList.begin(),
           it_end=includeList.end(); it!=it_end; it++) {
      string::size_type i=(*it).find("/");
      if (i==string::npos) {
        LERROR << "Cannot include resources " << *it
               << ": must specify file and module name";
        continue;
      }
      QString fileName;
      std::string moduleName;
      try {
        RESOURCESLOGINIT;
#ifdef DEBUG_LP
        LDEBUG << "i="<< i;
#endif
        QStringList configPaths = QString::fromUtf8(Common::MediaticData::MediaticData::single().getConfigPath().c_str()).split(LIMA_PATH_SEPARATOR);
        Q_FOREACH(QString confPath, configPaths)
        {
          if  (QFileInfo::exists(confPath + "/" + string((*it),0,i).c_str()))
          {

            fileName = (confPath + "/" + string((*it),0,i).c_str());
            break;
          }
        }
        if (fileName.isEmpty())
        {
          LERROR << "No resources" << *it << "found in" << Common::MediaticData::MediaticData::single().getConfigPath();
          continue;
        }
        moduleName=string((*it),i+1);
        LINFO << "LinguisticResourcesPrivate::includeResources filename="<< fileName << "moduleName="<< moduleName;
        XMLConfigurationFileParser parser(fileName);
        ModuleConfigurationStructure& newMod=parser.getModuleConfiguration(moduleName);
        module.addModule(newMod);
#ifdef DEBUG_LP
        ostringstream oss;
        for (auto it=module.cbegin(),it_end=module.cend(); it!=it_end; it++)
        {
          oss << (*it).first << ";";
        }
       LDEBUG << "LinguisticResourcesPrivate::includeResources added module with the following groups: " << oss.str();
#endif
        // recursive inclusions
        includeResources(module,newMod);
      }
      catch(NoSuchModule& ) {
        RESOURCESLOGINIT;
        LERROR << "Cannot find module " << moduleName
               << " in file " << fileName;
      }
      catch(exception& e) {
        RESOURCESLOGINIT;
        LERROR << "Error trying to find module " << moduleName
               << " in file " << fileName << ":" << e.what();
      }
    }
  }
  catch(NoSuchList& ) { } // do nothing: optional
  catch(NoSuchGroup& ) { } // do nothing: optional
}


Common::XMLConfigurationFiles::ModuleConfigurationStructure&
LinguisticResources::getModuleConfiguration(MediaId lang)
{
#ifdef DEBUG_LP
  RESOURCESLOGINIT;
  LDEBUG << "LinguisticResources::getModuleConfiguration" << this << lang;
#endif
  std::map<MediaId,AbstractResource::Manager*>::iterator it=
    m_d->m_resourcesManagers.find(lang);
  if (it == m_d->m_resourcesManagers.end()) {
    RESOURCESLOGINIT;
    LERROR << "no Resources module configuration for language "<< (uint64_t)lang;
    throw InvalidConfiguration();
  }
  return (*it).second->getModuleConfigurationStructure();
}


} // LinguisticProcessing
} // Lima
