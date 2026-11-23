// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "MediaProcessors.h"

#include "MediaProcessUnit.h"
#include "MediaAnalysisDumper.h"

#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include <iostream>


using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;

namespace Lima {

class MediaProcessorsPrivate
{
  friend class MediaProcessors;

  MediaProcessorsPrivate();
  MediaProcessorsPrivate(const MediaProcessorsPrivate& /*mpp*/) {}
  virtual ~MediaProcessorsPrivate() {}

  std::map<MediaId,MediaProcessUnit::Manager*> m_pipelineManagers;
  std::map<std::string,std::map<MediaId,std::shared_ptr<MediaProcessUnitPipeline>> > m_pipelines;

//   std::map<MediaId,MediaAnalysisDumper::Manager*> m_dumperManagers;
  std::map<std::string,std::map<MediaId,std::shared_ptr<MediaAnalysisDumper> > > m_analysisDumpers;

  // The pipelines of the medias re-initialized by initMedia, to instantiate
  // again in initPipelines
  std::map<MediaId,std::set<std::string> > m_pipelinesToRestore;

  /** Removes the pipelines and analysis dumpers of a media and deletes its
   * process units manager (thus its process units, unless a pipeline in use
   * still holds them). Returns the ids of the pipelines it had. */
  std::set<std::string> release(MediaId med);

  /** Instantiates the pipeline @ref id of the media @ref med from its manager.
   * Returns false (and logs) on failure. */
  bool initPipeline(const std::string& id,
                    const std::map<std::string,std::string>& pipelineMapping,
                    MediaId med,
                    MediaProcessUnit::Manager* manager);

  // recursive read of configuration files
  void includeProcessors(Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
                          Common::XMLConfigurationFiles::ModuleConfigurationStructure& newModule);


};

MediaProcessorsPrivate::MediaProcessorsPrivate() :
    m_pipelineManagers(), m_pipelines(), m_analysisDumpers()
{
}

MediaProcessors::MediaProcessors() :
    Singleton<MediaProcessors>(),
    m_d(new MediaProcessorsPrivate())
{
}

MediaProcessors::MediaProcessors(const MediaProcessors& mp) :
    Singleton<MediaProcessors>(),
    m_d(new MediaProcessorsPrivate(*mp.m_d))
{
}

MediaProcessors::~MediaProcessors()
{
  for (auto it = m_d->m_pipelineManagers.begin();
       it != m_d->m_pipelineManagers.end(); it++ )
  {
    delete it->second;
    it->second=0;
  }
  //   for (std::map<MediaId,MediaAnalysisDumper::Manager*>::iterator it=m_dumperManagers.begin();
  //        it!=m_dumperManagers.end();
  //        it++)
  //   {
  //     delete it->second;
  //     it->second=0;
  //   }
  delete m_d;
}

const std::map<MediaId,MediaProcessUnit::Manager*>& MediaProcessors::managers() const
{
  return m_d->m_pipelineManagers;
}

void MediaProcessors::initMedia (
    MediaId med,
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& confProc )
{
#ifdef DEBUG_CD
  PROCESSORSLOGINIT;
  LDEBUG << "MediaProcessors::initMedia" << med;
#endif

  // A media initialized again (e.g. by a new analyzer, possibly with other
  // models): release its former process units before building the new ones,
  // otherwise their models would never be freed. Its former pipelines are
  // instantiated again by initPipelines.
  if (m_d->m_pipelineManagers.find(med) != m_d->m_pipelineManagers.end())
  {
    auto previous = m_d->release(med);
    m_d->m_pipelinesToRestore[med].insert(previous.begin(), previous.end());
  }

  MediaProcessUnitInitializationParameters lpuinit;
  lpuinit.media=med;
  m_d->m_pipelineManagers[med] = new MediaProcessUnit::Manager(confProc,
                                                               lpuinit);

  //   MediaAnalysisDumperInitializationParameters ladinit;
  //   ladinit.media=med;
  //   m_dumperManagers[lang]=new MediaAnalysisDumper::Manager(confDump,ladinit);

  // Recursive init for 'Processors'
  auto& module = m_d->m_pipelineManagers[med]->getModuleConfigurationStructure();
  m_d->includeProcessors(module, module);
}

void MediaProcessors::initPipelines(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& confGroup,
    const std::deque<std::string>& pipelines )
{
#ifdef DEBUG_CD
  PROCESSORSLOGINIT;
#endif
  std::deque<std::string> pipelinesToInit ( pipelines );
  std::map<std::string, std::map<std::string, std::string> > allpipelines;
  try
  {
    allpipelines = confGroup.getMaps();
  }
  catch ( NoSuchGroup& e )
  {
    PROCESSORSLOGINIT;
    LERROR << "no group 'declaration' to configure linguistic pipelines ";
    throw InvalidConfiguration(e.what());
  }

  // if pipelines is empty, then initialize all pipelines
  if ( pipelinesToInit.empty() )
  {
    for (auto mapItr = allpipelines.cbegin();
          mapItr != allpipelines.cend(); mapItr++ )
    {
      pipelinesToInit.push_back( mapItr->first );
    }
  }

#ifdef DEBUG_CD
  LDEBUG << "MediaProcessors::initPipelines ";
  for (auto pipItr = pipelinesToInit.cbegin();
        pipItr != pipelinesToInit.cend(); pipItr++ )
  {
    LDEBUG << "\t" << *pipItr << ", ";
  }
#endif
  // init pipelines for all available medias
  for (auto pipItr = pipelinesToInit.cbegin();
        pipItr != pipelinesToInit.cend(); pipItr++ )
  {
    //      std::cout << "initialize pipeline type  '" << *pipItr << "'" << std::endl;
    const map<string,string>& pipelineMapping=allpipelines[*pipItr];
    if ( pipelineMapping.empty() )
    {
      PROCESSORSLOGINIT;
      LERROR << "no pipeline '" << *pipItr
              << "' defined in pipeline declaration module!";
      continue;
    }
    for (auto mapItr = m_d->m_pipelineManagers.cbegin();
          mapItr != m_d->m_pipelineManagers.cend(); mapItr++ )
    {
      m_d->initPipeline(*pipItr, pipelineMapping, mapItr->first, mapItr->second);
    }
  }

  // Restore, for the medias initialized again, the pipelines they had and
  // that were not requested here (only for them: not to load them for the
  // other medias)
  std::set<std::string> initialized(pipelinesToInit.begin(), pipelinesToInit.end());
  for (const auto& [med, ids]: m_d->m_pipelinesToRestore)
  {
    auto managerItr = m_d->m_pipelineManagers.find(med);
    if (managerItr == m_d->m_pipelineManagers.end())
      continue;
    for (const auto& id: ids)
    {
      if (initialized.find(id) != initialized.end())
        continue;
      auto mappingItr = allpipelines.find(id);
      if (mappingItr != allpipelines.end())
        m_d->initPipeline(id, mappingItr->second, med, managerItr->second);
    }
  }
  m_d->m_pipelinesToRestore.clear();
}

bool MediaProcessorsPrivate::initPipeline(
    const std::string& id,
    const std::map<std::string,std::string>& pipelineMapping,
    MediaId med,
    MediaProcessUnit::Manager* manager)
{
  const std::string& mediaStr=MediaticData::single().getMediaId ( med );
  auto entryItr = pipelineMapping.find ( mediaStr );
  if ( entryItr == pipelineMapping.end() )
  {
    PROCESSORSLOGINIT;
    LINFO << "no pipeline '" << id << "' for media " << mediaStr;
    return false;
  }
#ifdef DEBUG_CD
  PROCESSORSLOGINIT;
  LDEBUG << "MediaProcessors::initPipelines instanciating"
          << entryItr->second;
#endif
  std::shared_ptr<MediaProcessUnit> pu = nullptr;
  try
  {
    pu = manager->getObject( entryItr->second );
  }
  catch (const InvalidConfiguration& e)
  {
  }
  if (pu == nullptr)
  {
    PROCESSORSLOGINIT;
    LERROR << "no process unit '" << entryItr->second << "' for media " << mediaStr;
    return false;
  }
  auto  pipeline =  std::dynamic_pointer_cast<MediaProcessUnitPipeline> ( pu );
  if ( pipeline==nullptr )
  {
    PROCESSORSLOGINIT;
    LERROR << "pipeline '" << id << "' for media "
            << mediaStr << " is not of type ProcessUnitPipeline!";
    return false;
  }
  m_pipelines[id][med]=pipeline;
  return true;
}

std::set<std::string> MediaProcessorsPrivate::release(MediaId med)
{
  std::set<std::string> ids;
  for (auto& [id, pipelines]: m_pipelines)
  {
    if (pipelines.erase(med) > 0)
      ids.insert(id);
  }
  for (auto& [type, dumpers]: m_analysisDumpers)
    dumpers.erase(med);
  auto managerItr = m_pipelineManagers.find(med);
  if (managerItr != m_pipelineManagers.end())
  {
    delete managerItr->second;
    m_pipelineManagers.erase(managerItr);
  }
  return ids;
}

void MediaProcessors::releaseMedia(MediaId med)
{
  PROCESSORSLOGINIT;
  LINFO << "MediaProcessors::releaseMedia" << med;
  m_d->release(med);
  m_d->m_pipelinesToRestore.erase(med);
}


std::shared_ptr<MediaProcessUnitPipeline> MediaProcessors::getPipelineForId (
    MediaId med,
    const std::string& id ) const
{
  auto pipItr = m_d->m_pipelines.find ( id );
  if ( pipItr == m_d->m_pipelines.end() )
  {
    PROCESSORSLOGINIT;
    LERROR << "no pipeline type '" << id << "' defined in media " << med
           << " configuration file !";
    return nullptr;
  }
  auto medItr = pipItr->second.find ( med );
  if ( medItr==pipItr->second.end() )
  {
    PROCESSORSLOGINIT;
    LERROR << "no media [" << med << ":"
           << MediaticData::single().getMediaId(med).c_str()
           << "] defined for pipeline '" << id.c_str()
           << "' in the configuration file!";
    return nullptr;
  }
  return medItr->second;
}

std::shared_ptr<MediaAnalysisDumper> MediaProcessors::getAnalysisDumperForId (
    MediaId med,
    const std::string& type ) const
{
  PROCESSORSLOGINIT;
  auto dumpItr = m_d->m_analysisDumpers.find ( type );
  if ( dumpItr == m_d->m_analysisDumpers.end() )
  {
    LWARN << "no dumper type '" << type << "' defined in media " << med
          << " configuration file ";
    return nullptr;
  }
  auto medItr = dumpItr->second.find ( med );
  if ( medItr==dumpItr->second.end() )
  {
    LWARN << "no media [" << med << ":"
          << MediaticData::single().getMediaId(med) << "] defined for dumper '"
          << type << "' in the configuration file !";
      return nullptr;
  }
  return medItr->second;
}

void MediaProcessors::getAvailableMedia ( std::set<MediaId>& ids ) const
{
  ids.clear();
  for ( auto & kv : m_d->m_pipelines )
  {
    for ( auto & id2pipeline : kv.second )
    {
      ids.insert(id2pipeline.first);
    }
  }
}

void MediaProcessors::getAvailablePipelinesForMedia ( MediaId id, std::set<std::string>& pipelines ) const
{
  pipelines.clear();
  for ( auto & kv : m_d->m_pipelines )
  {
    if (kv.second.cend() != kv.second.find(id))
    {
      pipelines.insert(kv.first);
    }
  }
}

// two arguments: first is the module to modify, second is the module in which
// the 'include' group can be found : both arguments can be the same
void MediaProcessorsPrivate::includeProcessors(
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& module,
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& includeModule)
{
  try
  {
    auto includeList = includeModule.getListValuesAtKeyOfGroupNamed("includeList",
                                                                    "include");
    for (auto it = includeList.cbegin(); it!=includeList.cend(); it++)
    {
      string::size_type i=(*it).find("/");
      if ( i== string::npos)
      {
        //PROCESSORSLOGINIT;
        //LERROR << "Cannot include processors " << *it
        //       << ": must specify file and module name";
        continue;
      }
      QString fileName;
      std::string moduleName("");
      try
      {
        //PROCESSORSLOGINIT;
        //LDEBUG << "i="<< i;
        fileName = Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getConfigPath().c_str(),
                                      std::string((*it),0,i).c_str());
        //LDEBUG << "filename="<< fileName;
        moduleName = std::string((*it),i+1);
        //LDEBUG << "moduleName="<< moduleName;
        XMLConfigurationFileParser parser(fileName);
        ModuleConfigurationStructure& newMod = parser.getModuleConfiguration(moduleName);
        module.addModule(newMod);
        //LDEBUG << "added module";
        //ostringstream oss;
        //for (auto it = module.cbegin(); it != module.cend(); it++)
        //{
        //    oss << (*it).first << ";";
        //}
        //LDEBUG << "module contain following groups: " << oss.str();
        // recursive inclusions

        // 'include' group is not inserted in module by addModule function because a group
        // with same name was already there : use the one in newMod
        includeProcessors(module,newMod);
      }
      catch (NoSuchModule& )
      {
          //PROCESSORSLOGINIT;
          //LERROR << "Cannot find module " << moduleName
          //       << " in file " << fileName;
      }
      catch (std::exception& )
      {
          //PROCESSORSLOGINIT;
          //LERROR << "Error trying to find module " << moduleName
          //       << " in file " << fileName << ":" << e.what();
      }
    }
  }
  catch (NoSuchList& ) { } // do nothing: optional
  catch (NoSuchGroup& ) { } // do nothing: optional
}


} // Lima
