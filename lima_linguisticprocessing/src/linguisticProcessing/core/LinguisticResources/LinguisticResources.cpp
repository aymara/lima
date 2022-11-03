// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

namespace LinguisticProcessing
{

class LinguisticResourcesPrivate
{
friend class LinguisticResources;

public:
  LinguisticResourcesPrivate();
  virtual ~LinguisticResourcesPrivate();

  void clear();

  std::map<MediaId, std::shared_ptr<AbstractResource::Manager> > m_resourcesManagers;

  // private member functions
  void includeResources(Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
                        Common::XMLConfigurationFiles::ModuleConfigurationStructure& includeModule);

};

LinguisticResourcesPrivate::LinguisticResourcesPrivate()
{
}

LinguisticResourcesPrivate::~LinguisticResourcesPrivate()
{
  clear();
}

void LinguisticResourcesPrivate::clear()
{
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
  auto it = m_d->m_resourcesManagers.find(lang);
  if (it==m_d->m_resourcesManagers.end())
  {
    LIMA_EXCEPTION_SELECT_LOGINIT(
      RESOURCESLOGINIT,
      "LinguisticResources::getResource: language " << (int)lang << " is not initialized !",
      MediaNotInitialized);
  }
  return it->second->getObject(id);
}

void LinguisticResources::initLanguage(
  MediaId lang,
  Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule,
  bool registerMainKeysInStringPool)
{
  // TODO make this function thread safe
  RESOURCESLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "LinguisticResources::initLanguage" << this << confModule.getName();
#endif
  if (m_d->m_resourcesManagers.find(lang) != m_d->m_resourcesManagers.end())
  {
    LINFO << "LinguisticResources::initLanguage resources already initialized for" << lang;
    return;
  }
  ResourceInitializationParameters params;
  params.language=lang;
  m_d->m_resourcesManagers[lang] = std::make_shared<AbstractResource::Manager>(confModule, params);
  auto& module= m_d->m_resourcesManagers[lang]->getModuleConfigurationStructure();
  m_d->includeResources(module, module);

  if (registerMainKeysInStringPool)
  {
    // Initialize FsaStringsPool : main keys must be registered at beginning
    try
    {
      const auto& mainkeys = confModule.getParamValueAtKeyOfGroupNamed("mainKeys","FsaStringsPool");
      LINFO << "load mainKeys " << mainkeys << " into StringPool";
      auto res = getResource(lang,mainkeys);
      auto access = static_cast<AnalysisDict::AbstractAccessResource*>(res);
      access->setMainKeys(true);
      Common::MediaticData::MediaticData::changeable().stringsPool(lang).registerMainKeys(access->getAccessByString());
    }
    catch (NoSuchGroup& )
    {
      LERROR << "No group 'FsaStringsPool' defined in Resource module. Can't register any main keys";
      throw InvalidConfiguration();
    }
    catch (NoSuchParam& )
    {
      LERROR << "No param 'mainKeys' defined in FsaStringsPool group. Can't register any main keys";
      throw InvalidConfiguration();
    }
  }
}

void LinguisticResourcesPrivate::includeResources(
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& includeModule)
{
  RESOURCESLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "LinguisticResourcesPrivate::includeResources" << this << &module << module.getName()
          << &includeModule << includeModule.getName();
#endif
  try
  {
    const auto& includeList = includeModule.getListValuesAtKeyOfGroupNamed("includeList", "include");
    for (const auto& include: includeList)
    {
      auto i = include.find("/");
      if (i==string::npos)
      {
        LERROR << "Cannot include resources " << include << ": must specify file and module name";
        continue;
      }
      QString fileName;
      std::string moduleName;
      try
      {
        RESOURCESLOGINIT;
#ifdef DEBUG_LP
        LDEBUG << "i="<< i;
#endif
        auto configPaths = QString::fromUtf8(
          Common::MediaticData::MediaticData::single().getConfigPath().c_str()).split(LIMA_PATH_SEPARATOR);
        for(const auto& confPath: configPaths)
        {
          if  (QFileInfo::exists(confPath + "/" + string(include, 0, i).c_str()))
          {

            fileName = (confPath + "/" + string(include, 0, i).c_str());
            break;
          }
        }
        if (fileName.isEmpty())
        {
          LERROR << "No resources" << include << "found in"
                  << Common::MediaticData::MediaticData::single().getConfigPath();
          continue;
        }
        moduleName = std::string(include, i+1);
        LINFO << "LinguisticResourcesPrivate::includeResources filename="<< fileName << "moduleName="<< moduleName;
        XMLConfigurationFileParser parser(fileName);
        auto& newMod = parser.getModuleConfiguration(moduleName);
        module.addModule(newMod);
#ifdef DEBUG_LP
        ostringstream oss;
        for (const auto& group: module)
        {
          oss << group.first << ";";
        }
       LDEBUG << "LinguisticResourcesPrivate::includeResources added module with the following groups: " << oss.str();
#endif
        // recursive inclusions
        includeResources(module, newMod);
      }
      catch(NoSuchModule& )
      {
        RESOURCESLOGINIT;
        LERROR << "Cannot find module " << moduleName << " in file " << fileName;
      }
      catch(exception& e)
      {
        RESOURCESLOGINIT;
        LERROR << "Error trying to find module " << moduleName << " in file " << fileName << ":" << e.what();
      }
    }
  }
  catch(NoSuchList& ) { } // do nothing: optional
  catch(NoSuchGroup& ) { } // do nothing: optional
}


Common::XMLConfigurationFiles::ModuleConfigurationStructure& LinguisticResources::getModuleConfiguration(MediaId lang)
{
#ifdef DEBUG_LP
  RESOURCESLOGINIT;
  LDEBUG << "LinguisticResources::getModuleConfiguration" << this << lang;
#endif
  auto it = m_d->m_resourcesManagers.find(lang);
  if (it == m_d->m_resourcesManagers.end())
  {
    RESOURCESLOGINIT;
    LERROR << "no Resources module configuration for language "<< (uint64_t)lang;
    throw InvalidConfiguration();
  }
  return (*it).second->getModuleConfigurationStructure();
}


} // LinguisticProcessing
} // Lima
