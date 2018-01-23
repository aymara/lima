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
/**
  * @file          MediaticData.cpp
  * @author        Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *                Copyright (C) 2002-2012 by CEA LIST
  * @date          Started on Mon dec, 2 2002
  */

#include "mediaticData.h"

// ---------------------------------------------------------------------------
//  Local includes
// ---------------------------------------------------------------------------
#include "common/LimaCommon.h"
#include "common/QsLog/QsLog.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/Data/readwritetools.h"
#include "common/misc/DoubleAccessObjectToIdMap.h"
//#include "common/misc/strwstrtools.h"
//#include "common/misc/strx.h"
//#include "common/misc/traceUtils.h"


// ---------------------------------------------------------------------------
//  Standard libraries includes
// ---------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <deque>

#include <QSet>
#include <QRegExp>
#include <QString>
#include <QFileInfo>

using namespace std;

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace Common
{
using namespace XMLConfigurationFiles;
//using namespace Misc;

namespace MediaticData
{

class MediaticDataPrivate
{
  friend class MediaticData;

private:
  MediaticDataPrivate();
  virtual ~MediaticDataPrivate();
  
protected:

    virtual void initMedias(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser,
        const std::deque< std::string >& meds);

    void initReleaseStringsPool(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    void initRelations(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    void initConceptTypes(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    std::deque< std::string > m_medias;

private:
    static const LimaString s_entityTypeNameSeparator;

    std::map< std::string, MediaId > m_mediasIds;
    std::map< MediaId, std::string > m_mediasSymbol;
    std::map< MediaId, QString > m_mediaDefinitionFiles;
    std::map< MediaId, MediaData* > m_mediasData;

    // entity types
    typedef Common::Misc::DoubleAccessObjectToIdMap<LimaString,EntityGroupId> EntityGroupMap;
    typedef Common::Misc::DoubleAccessObjectToIdMap<LimaString,EntityTypeId> EntityTypeMap;
    EntityGroupMap m_entityGroups;
    std::vector<EntityTypeMap*> m_entityTypes;

    std::map< std::string, uint8_t > m_relTypes;
    std::map< uint8_t, std::string > m_relTypesNum;

    static std::string s_undefinedRelation;


    std::map<MediaId,FsaStringsPool*> m_stringsPool;
    bool m_releaseStringsPool;

    std::string m_resourcesPath;
    std::string m_configPath;
    std::string m_configFile;

    static const std::string s_nocateg;

    std::map< std::string, ConceptType > m_conceptTypes;
    std::map< ConceptType, std::string > m_conceptNames;
};
  
const LimaString MediaticDataPrivate::s_entityTypeNameSeparator=Common::Misc::utf8stdstring2limastring(".");


std::string MediaticDataPrivate::s_undefinedRelation("Unknown");
const std::string MediaticDataPrivate::s_nocateg=SYMBOLIC_NONE;


MediaticData::MediaticData() :
    Singleton<MediaticData>(),
    m_d(new MediaticDataPrivate())
{
}

MediaticData::MediaticData(const MediaticData& md) :
    Singleton<MediaticData>(),
    m_d(new MediaticDataPrivate(*md.m_d))
{
}

MediaticData::~MediaticData()
{
  delete m_d;
}

const std::map< std::string, MediaId >& MediaticData::getMediasIds() const
{
    return m_d->m_mediasIds;
}

const std::string& MediaticData::media(MediaId media) const
{
    return getMediaId(media);
}

MediaId MediaticData::media(const std::string& media) const
{
    return getMediaId(media);
}

const std::deque<std::string>& MediaticData::getMedias() const
{
    return m_d->m_medias;
}

/*********************************************************************
  * Configuration functions
  ********************************************************************/

bool MediaticData::releaseStringsPool() const
{
    return m_d->m_releaseStringsPool;
}

const std::string& MediaticData::getResourcesPath() const
{
    return m_d->m_resourcesPath;
}

const std::string& MediaticData::getConfigPath() const
{
    return m_d->m_configPath;
}

const std::string& MediaticData::getConfigFile() const
{
    return m_d->m_configFile;
}

void MediaticData::init(
  const std::string& resourcesPath,
  const std::string& configPath,
  const std::string& configFile,
  const std::deque< std::string >& meds)
{

//  TimeUtils::updateCurrentTime();
  MDATALOGINIT;
  LINFO << "MediaticData::init " << resourcesPath << " " << configPath << " " << configFile;
  //LINFO << "Mediatic data initialization";

  m_d->m_resourcesPath=resourcesPath;
  m_d->m_configPath=configPath;
  m_d->m_configFile=configFile;

  //LINFO << "initialize XMLParser";
  QStringList configPaths = QString::fromUtf8(configPath.c_str()).split(LIMA_PATH_SEPARATOR);
  QStringList configFiles = QString::fromUtf8(configFile.c_str()).split(LIMA_PATH_SEPARATOR);
  bool configurationFileFound = false;
  Q_FOREACH(QString confPath, configPaths)
  {
    Q_FOREACH(QString confFile, configFiles)
    {
      if (QFileInfo::exists(confPath + "/" + confFile))
      {
        LDEBUG << "MediaticData::init parse configuration file: " << (confPath + "/" + confFile);
        configurationFileFound = true;
        Common::XMLConfigurationFiles::XMLConfigurationFileParser configuration((confPath + "/" + confFile).toUtf8().constData());

        //    initHomoSyntagmaticChainsAndRelationsTypes(*configParser);
        LDEBUG << "MediaticData::init initialize global parameters";
        m_d->initReleaseStringsPool(configuration);

        initEntityTypes(configuration);

        m_d->initRelations(configuration);
        
        m_d->initConceptTypes(configuration);
        
        /**
          * initialize active medias
          */
        LINFO << "!!! MediaticData::init for ";
        for (std::deque< std::string >::const_iterator it = meds.begin(); it != meds.end(); it++)
          LINFO << "    " << (*it).c_str();

        m_d->initMedias(configuration, meds);
        
        m_d->m_mediasData.clear();
        for (map<string,MediaId>::const_iterator it=m_d->m_mediasIds.begin();
            it!=m_d->m_mediasIds.end();
            it++)
        {
          initMediaData(it->second);
        }
      }
      if (configurationFileFound) break;
    }
    if (configurationFileFound) break;
  }
  if (!configurationFileFound)
  {
    MDATALOGINIT;
    LERROR << "No configuration file has been found with" << configPath << "and" << configFile;
  }
  //LINFO << "Mediatic data initialization finished";
//  TimeUtils::logElapsedTime("MediaticDataInit");
}

bool MediaticData::isValidMedia(const std::string& media){
  QRegExp rx("(\\b\\w{3})\\b");
  QString q_media(media.c_str());
  return rx.exactMatch(q_media );
}

void MediaticData::initMedia(const std::string& media)
{
  if(!isValidMedia(media)){
    MDATALOGINIT;
    LERROR << "MediaId for string '" << media << "' will not be initialized ! ";
    throw MediaNotInitialized(media);
  }
//  TimeUtils::updateCurrentTime();
  MDATALOGINIT;
  LINFO << "MediaticData::initMedia" << media;

  //LINFO << "parse configuration file: " << configPath << "/" << configFile;
  Common::XMLConfigurationFiles::XMLConfigurationFileParser configuration(Common::Misc::findFileInPaths(m_d->m_configPath.c_str(), m_d->m_configFile.c_str()).toUtf8().constData());
  Lima::Common::MediaticData::MediaticData::changeable().initEntityTypes(configuration);

  std::deque< std::string > meds;
  meds.push_back(media);
  m_d->initMedias(configuration, meds);

  initMediaData(m_d->m_mediasIds[media]);

}

/** @return the string value of the given numerical media ID */
const std::string& MediaticData::getMediaId(MediaId idNum) const
{
  map<MediaId,string>::const_iterator it=m_d->m_mediasSymbol.find(idNum);
  if (it==m_d->m_mediasSymbol.end())
  {
//     MDATALOGINIT;
//     LERROR << "No media id for " ;//<< (int)idNum;
    throw MediaNotInitialized(idNum);
  }
  return it->second;
}

/** @return the numerical value of the given string media id (3 chars code) */
MediaId MediaticData::getMediaId(const std::string& stringId) const
{
  if (stringId.empty())
  {
    MDATALOGINIT;
    LERROR << "MediaticData::getMediaId invalid empty argument stringId at" << __FILE__ << ", line" << __LINE__;
    throw std::runtime_error( std::string("MediaticData::getMediaId invalid empty argument stringId at ").c_str() );
  }
  std::map< std::string, MediaId >::const_iterator it = m_d->m_mediasIds.find(stringId);
  if (it == m_d->m_mediasIds.end())
  {
    // try on-demand initialization
    MDATALOGINIT;
    LINFO << "MediaticData::getMediaId On-demand initialization of media" << stringId.c_str();
    const_cast<MediaticData*>(this)->initMedia(stringId);
    it = m_d->m_mediasIds.find(stringId);
    if (it == m_d->m_mediasIds.end())
    {
// printing of error message suppressed as calling getMediaId on a non-initialized
// media string can be wanted in case of on-demand media initialization
// if this is a real error, the catcher can print this message.
      MDATALOGINIT;
      LERROR << "MediaId for string " << stringId.c_str() << " is still not initialized after on-demand initialization ! ";
      throw MediaNotInitialized(stringId);
    }
  }
  return it->second;
}

const MediaData& MediaticData::mediaData(MediaId media) const
{
  map<MediaId,MediaData*>::const_iterator it = m_d->m_mediasData.find(media);
  if (it == m_d->m_mediasData.end())
  {
    MDATALOGINIT;
    LERROR << "Media data for id " << (int)media << " is not initialized ! ";
    throw MediaNotInitialized(media);
  }
  return *(it->second);
}

const MediaData& MediaticData::mediaData(const std::string& med) const
{
  MediaId medId=getMediaId(med);
  return mediaData(medId);
}

MediaData& MediaticData::mediaData(MediaId media)
{
  map<MediaId,MediaData*>::iterator it = m_d->m_mediasData.find(media);
  if (it == m_d->m_mediasData.end())
  {
    MDATALOGINIT;
    LERROR << "Media data for id " << (int)media << " is not initialized ! ";
    throw MediaNotInitialized(media);
  }
  return *(it->second);
}

void MediaticDataPrivate::initMedias(
  XMLConfigurationFileParser& configParser,
  const std::deque< std::string >& meds)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initMedias" << meds.size();
#endif
  //LINFO << "initializes available medias list";
  if (meds.size()==0)
  {
//     try
//     {
//       m_medias =
//         configParser.getModuleGroupListValues("common", "mediaDeclaration", "available");
//     }
//     catch (NoSuchList& )
//     {
//       LERROR << "missing 'medias/declaration/available' list in configuration file";
//       throw InvalidConfiguration();
//     }
  }
  else
  {
    std::set< std::string > qmeds;
    for (auto it = m_medias.begin(); it != m_medias.end(); it++)
    {
      qmeds.insert(*it);
    }
    for (auto it = meds.begin(); it != meds.end(); it++)
    {
      MediaId id(0);
      try
      {
        id = static_cast<MediaId>(std::atoi(configParser.getModuleGroupParamValue("common","mediasIds",*it).c_str()));
#ifdef DEBUG_CD
        LDEBUG << "media '" << (*it).c_str() << "' has id " << id;
        LDEBUG << (void*)this << " initialize string pool";
#endif
      }
      catch (NoSuchList& e)
      {
        MDATALOGINIT;
        LERROR << "missing id for media " << (*it).c_str() << ":" << e.what();
        throw InvalidConfiguration(std::string("Failed to init media ")+(*it)+": "+e.what());
      }
      catch (NoSuchParam& e)
      {
        MDATALOGINIT;
        LERROR << "missing id for media " << (*it).c_str() << ":" << e.what();
        throw InvalidConfiguration(std::string("Failed to init media ")+(*it)+": "+e.what());
      }

      if (qmeds.find(*it) != qmeds.end())
      {
        LERROR << "media" << (*it).c_str() << "already initialized: reinit";
        // clear initialization
        delete m_stringsPool[id];
      }
      //else 
      // always perform initialization, even if language already initialized 
      // (allows dynamic reinitialization of lima client)
      {
        m_medias.push_back(*it);
        try
        {
          // initialize strings pool
          m_stringsPool.insert(std::make_pair(id, new FsaStringsPool()));

          m_mediasIds[*it]=id;
          m_mediasSymbol[id]=*it;

          QString deffile= QString::fromUtf8(configParser.getModuleGroupParamValue("common","mediaDefinitionFiles",*it).c_str());
          QStringList configPaths = QString::fromUtf8(m_configPath.c_str()).split(LIMA_PATH_SEPARATOR);
          bool mediaDefinitionFileFound = false;
          for(auto confPath = configPaths.begin(); confPath != configPaths.end(); ++confPath)
          {
            if (QFileInfo::exists(*confPath + "/" + deffile))
            {
              m_mediaDefinitionFiles[id] = (*confPath+"/"+deffile);
#ifdef DEBUG_CD
              LDEBUG << "media definition file for id" << id << "is" << m_mediaDefinitionFiles[id];
#endif
              mediaDefinitionFileFound = true;
              break;
            }
          }
          if (!mediaDefinitionFileFound)
          {
            MDATALOGINIT;
            LERROR << "No media definition file'"<<deffile<<"' has been found for media id" << id 
                    << "in config paths:" << configPaths;
            throw InvalidConfiguration();
          }
        }
        catch (NoSuchList& e)
        {
          MDATALOGINIT;
          LERROR << "missing definition file for media " << (*it).c_str() << ":" << e.what();
          throw InvalidConfiguration(std::string("Failed to init media ")+(*it)+": "+e.what());
        }
        catch (NoSuchParam& e)
        {
          MDATALOGINIT;
          LERROR << "missing definition file for media " << (*it).c_str() << ":" << e.what();
        throw InvalidConfiguration(std::string("Failed to init media ")+(*it)+": "+e.what());
        }
      }
    }
  }
}

void MediaticData::initMediaData(MediaId med)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticData::initMediaData '" << (int)med << "'";
#endif
  auto it=m_d->m_mediaDefinitionFiles.find(med);
  if (it==m_d->m_mediaDefinitionFiles.end())
  {
    MDATALOGINIT;
    LERROR << "No media definition file for med id " << med;
    std::ostringstream oss;
    oss << "No media definition file for med id " << med;
    throw InvalidConfiguration(oss.str());
  }
#ifdef DEBUG_CD
  LDEBUG << "MediaticData::initMediaData Parse MediaConfigurationFile " << (it->second);
#endif
  XMLConfigurationFileParser parser((it->second).toUtf8().constData());

#ifdef DEBUG_CD
  LDEBUG << "MediaticData::initMediaData Class: " << parser.getModuleGroupParamValue("MediaData","Class","class").c_str();
#endif
  std::string& param = parser.getModuleGroupParamValue("MediaData","Class","class");
  if (!param.empty())
  {
    MediaData* ldata = MediaData::Factory::getFactory(param)->create(parser.getModuleGroupConfiguration("MediaData","Class"),0);

    //   MediaData* ldata=new MediaData();
    m_d->m_mediasData[med]=ldata;
    ldata->initialize(med,m_d->m_resourcesPath,parser);
  }
  else
  {
    MDATALOGINIT;
    LERROR << "Empty class name for MediaData/Class/class for media" << med;
    std::ostringstream oss;
    oss << "Empty class name for MediaData/Class/class for media " << med;
    throw InvalidConfiguration(oss.str());
  }
}



// uint32_t MediaticData::
// getEntityType(const std::string& entityTypeGroup,
//               const MediaId& media,
//               const std::string& name) const
// {
//   return mediaData(media).getEntityCommonType(entityTypeGroup,name);
// }

/** @return the string value of the given relation numerical value */
const std::string& MediaticData::getRelation(uint8_t relation) const
{
  std::map< uint8_t, std::string >::const_iterator it = m_d->m_relTypesNum.find(relation);
  if (it == m_d->m_relTypesNum.end())
  {
    MDATALOGINIT;
    LWARN << "ask getRelation for undefined relation " << (int)relation;
    return m_d->s_undefinedRelation;
  }
  return ( (*it).second );
}

/** @return the numerical value of the given relation name */
uint8_t MediaticData::getRelation(const std::string& relation) const
{
  std::map< std::string, uint8_t >::const_iterator it = m_d->m_relTypes.find(relation);
  if (it == m_d->m_relTypes.end())
  {
    MDATALOGINIT;
    LWARN << "ask getRelation for undefined relation " << relation.c_str();
    return 0;
  }
  return ( (*it).second );
}

void MediaticDataPrivate::initRelations(
  XMLConfigurationFiles::XMLConfigurationFileParser& configParser)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initRelations";
#endif
  m_relTypes[s_undefinedRelation]=0;
  m_relTypesNum[0]=s_undefinedRelation;
  
  try {
    const map<string,string>& rels=configParser.getModuleConfiguration("common").getGroupNamed("semanticRelations").getMapAtKey("declaration");
    for (map<string,string>::const_iterator it=rels.begin();
         it!=rels.end();
         it++)
    {
      uint8_t relId=atoi(it->second.c_str());
#ifdef DEBUG_CD
      LDEBUG << "read relation " << it->first.c_str() << " -> " << (int)relId;
#endif
      m_relTypes[it->first]=relId;
      m_relTypesNum[relId]=it->first;
    }
  
  } catch (NoSuchGroup& e) {
    MDATALOGINIT;
    LERROR << "No group 'semanticRelations' in 'common' module of lima-common configuration file";
    throw InvalidConfiguration(std::string("No group 'semanticRelations' in 'common' module of lima-common configuration file:")+e.what());
  } catch (NoSuchMap& e) {
    MDATALOGINIT;
    LERROR << "No map 'declaration' in 'semanticRelations' group of lima-common configuration file";
    throw InvalidConfiguration(std::string("No map 'declaration' in 'semanticRelations' group of lima-common configuration file:")+e.what());
  }
}

void MediaticDataPrivate::initConceptTypes(
    XMLConfigurationFiles::XMLConfigurationFileParser& configParser) 
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initConceptTypes";
#endif
  
  try {
    const map<string,string>& mapping=configParser.getModuleConfiguration("common").getGroupNamed("SemanticData").getMapAtKey("conceptTypes");
    for (map<string,string>::const_iterator it=mapping.begin();
         it!=mapping.end();
         it++)
    {
      ConceptType type = static_cast<ConceptType>(atoi(it->second.c_str()));
#ifdef DEBUG_CD
      LDEBUG << "read concept type " << it->first.c_str() << " -> " << type;
#endif
      m_conceptTypes[it->first] = type;
      m_conceptNames[type] = it->first;
    }
  
  } catch (NoSuchGroup& e) {
    MDATALOGINIT;
    LERROR << "No group 'SemanticData' in 'common' module of lima-common configuration file";
    throw InvalidConfiguration(e.what());
  } catch (NoSuchMap& e) {
    MDATALOGINIT;
    LERROR << "No map 'conceptTypes' in 'SemanticData' group of lima-common configuration file";
    throw InvalidConfiguration(e.what());
  }
}

ConceptType MediaticData::getConceptType(const std::string& typeName) const
{
  if (m_d->m_conceptTypes.find(typeName)==m_d->m_conceptTypes.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type name " << typeName.c_str() << " not found. Returning value for LatticeDown (should be '0').";
        return (*(m_d->m_conceptTypes.find("LatticeDown"))).second;
  }
  else
  {
    return (*(m_d->m_conceptTypes.find(typeName))).second;
  }
}


const std::string& MediaticData::getConceptName(const ConceptType& type) const
{
  if (m_d->m_conceptNames.find(type)==m_d->m_conceptNames.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type " << type << " not found. Returning for 0 (should be 'LatticeDown').";
    return (*(m_d->m_conceptNames.find(static_cast<ConceptType>(0)))).second;
  }
  else
  {
    return (*(m_d->m_conceptNames.find(type))).second;
  }
}


void MediaticDataPrivate::initReleaseStringsPool(
  XMLConfigurationFiles::XMLConfigurationFileParser& configParser)
{
  MDATALOGINIT;
  LINFO << "initializes the release of strings pool on each text: ";
  m_releaseStringsPool = false;
  try
  {
    std::string release =
      configParser.getModuleGroupParamValue("common", "stringPool", "release") ;
    std::istringstream releaseS(release);
    releaseS >> std::boolalpha >> m_releaseStringsPool;
    LINFO << int(m_releaseStringsPool);
  }
  catch (const NoSuchParam& )
  {
    LWARN << "ReleaseStringsPool parameter not found. Using " << m_releaseStringsPool;
  }
}


//***********************************************************************
// entity types initialization

// internal output function for debug
void printEntities(QsLogging::Logger& logger,
                   const DoubleAccessObjectToIdMap<LimaString,EntityGroupId>& groups,
                   const std::vector<DoubleAccessObjectToIdMap<LimaString,EntityTypeId>* >& types)
{
  const DoubleAccessObjectToIdMap<LimaString,EntityGroupId>::AccessMap& g=groups.getAccessMap();
  for (DoubleAccessObjectToIdMap<LimaString,EntityGroupId>::AccessMap::const_iterator it=g.begin(),
         it_end=g.end(); it!=it_end; it++) {
    LDEBUG << *((*it).first)
       << "(" << (*it).first << ")"
       << "->" << (*it).second;
    if ((*it).second < types.size()) {
      const DoubleAccessObjectToIdMap<LimaString,EntityTypeId>::AccessMap& t=types[(*it).second]->getAccessMap();
      for (DoubleAccessObjectToIdMap<LimaString,EntityTypeId>::AccessMap::const_iterator it2=t.begin(),
             it2_end=t.end(); it2!=it2_end; it2++) {
        LDEBUG << "   " << *((*it2).first)
           << "(" << (*it2).first << ")"
           << "->" << (*it2).second;
      }
    }
  }
  // reverse maps
  const std::vector<const LimaString*>& rg=groups.getReverseAccessMap();
  for (uint32_t i(0);i<rg.size(); i++) {
    LDEBUG << "reverse " << i << "->" << rg[i];
    if (rg[i]!=0) {
      const std::vector<const LimaString*>& rt=types[i]->getReverseAccessMap();
      for (uint32_t j(0);j<rt.size(); j++) {
        LDEBUG << "    reverse " << j << "->" << rt[j];
      }
    }
  }
}

void MediaticData::initEntityTypes(XMLConfigurationFileParser& configParser)
{
  MDATALOGINIT;
  LINFO << "MediaticData::initEntityTypes";
  // look at all groups : ModuleConfigurationStructure is a map
  try {
    ModuleConfigurationStructure& moduleConf=configParser.getModuleConfiguration("entities");
    
    for (ModuleConfigurationStructure::iterator it=moduleConf.begin(),
           it_end=moduleConf.end(); it!=it_end; it++) {
#ifdef DEBUG_CD
      LDEBUG << "initEntityTypes: looking at group " << (*it).first.c_str();
#endif
     
      LimaString groupName=Common::Misc::utf8stdstring2limastring((*it).first);

      if (groupName=="include") 
      {
        deque<string> includeList=moduleConf.getListValuesAtKeyOfGroupNamed("includeList","include");
        string::size_type i;
        string moduleName("");
        for (std::size_t k=0; k<includeList.size(); k++) {
          i=includeList[k].find("/");
          if (i==string::npos) {
            LERROR << "Cannot include resources " << includeList[k] 
                << ": must specify file and module name" << LENDL;
          continue;
          }
          QStringList configPaths = QString::fromUtf8(m_d->m_configPath.c_str()).split(LIMA_PATH_SEPARATOR);
          Q_FOREACH(QString confPath, configPaths)
          {
            if  (QFileInfo::exists(confPath + "/" + string(includeList[k],0,i).c_str()))
            {

              std::string  fileName= (confPath + "/" + string(includeList[k],0,i).c_str()).toUtf8().constData();
        
              Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig2(fileName);
              Common::MediaticData::MediaticData::changeable().initEntityTypes(lpconfig2);
              break;
            }
          }
        }
        
      } 
      else 
      {  
        EntityGroupId groupId=addEntityGroup(groupName);
#ifdef DEBUG_CD
        LDEBUG << "initEntityTypes: id is " << groupId;
#endif
        
        GroupConfigurationStructure& groupConf=(*it).second;
        
        deque<string>& entityList=groupConf.getListsValueAtKey("entityList");
        for (deque<string>::const_iterator ent=entityList.begin(),
              ent_end=entityList.end(); ent!=ent_end; ent++) {
          
          LimaString entityName=Common::Misc::utf8stdstring2limastring(*ent);
#ifdef DEBUG_CD
          LDEBUG << "initEntityTypes: add entityType " << (*ent).c_str() << " in group "
          << groupName;
#endif
          EntityType type=addEntity(groupId,entityName);
#ifdef DEBUG_CD
          LDEBUG << "initEntityTypes: type is " << type;
#endif
        }
      }
    }
  }
  catch(NoSuchModule& e) {
    MDATALOGINIT;
    LWARN << "no module 'entities' in entity types configuration" << e.what();
  }
  catch(NoSuchGroup& e) {
    MDATALOGINIT;
    LERROR << "missing group in entity types configuration";
    std::ostringstream oss;
    oss  << "missing group in entity types configuration: " << e.what();
    throw InvalidConfiguration(oss.str());
  }
  catch(NoSuchList& e) {
    MDATALOGINIT;
    LERROR << "missing list 'entityList' in entity types configuration";
    std::ostringstream oss;
    oss  << "missing list 'entityList' in entity types configuration: " << e.what();
    throw InvalidConfiguration(oss.str());
  }
#ifdef DEBUG_CD
  /*if (logger.loggingLevel()<=QsLogging::DebugLevel) {
    printEntities(logger,m_d->m_entityGroups,m_d->m_entityTypes);
  }*/
#endif
}

EntityGroupId MediaticData::addEntityGroup(const LimaString& groupName)
{
  EntityGroupId groupId= m_d->m_entityGroups.insert(groupName);
  // insert may have created new element or not
  if (static_cast<std::size_t>(groupId) >= m_d->m_entityTypes.size()) {
    m_d->m_entityTypes.push_back(new MediaticDataPrivate::EntityTypeMap());
  }
  return groupId;
}

EntityType MediaticData::addEntity(EntityGroupId groupId, const LimaString& entityName)
{
  if (static_cast<std::size_t>(groupId)>=m_d->m_entityTypes.size()) {
    MDATALOGINIT;
    LERROR << "MediaticData::addEntity unknown entity group id " << groupId
            << "adding" << entityName;
    std::ostringstream oss;
    oss  << "MediaticData::addEntity unknown entity group id " << groupId
            << " adding " << entityName.toStdString() ;
    throw LimaException(oss.str());
  }
  EntityTypeId typeId= m_d->m_entityTypes[groupId]->insert(entityName);
  return EntityType(typeId,groupId);
}

EntityType MediaticData::addEntity(const LimaString& groupName,
          const LimaString& entityName)
{
  EntityGroupId groupId=getEntityGroupId(groupName);
  return addEntity(groupId,entityName);
}

// entity types accessors
EntityType MediaticData::getEntityType(const LimaString& entityName) const
{
  int i=entityName.indexOf(m_d->s_entityTypeNameSeparator);
  if (i==-1) {
    MDATALOGINIT;
    LERROR << "missing group name in entity name " << entityName;
    throw LimaException();
  }
  LimaString groupName = entityName.left(i);
  LimaString name = entityName.mid(i+m_d->s_entityTypeNameSeparator.length());
  return getEntityType(getEntityGroupId(groupName),name);
}

EntityType MediaticData::getEntityType(const EntityGroupId groupId,
              const LimaString& entityName) const
{
  if (static_cast<size_t>(groupId)>=m_d->m_entityTypes.size()) {
    MDATALOGINIT;
    LERROR << "MediaticData::getEntityType unknown entity group id " << groupId
            <<"accessing" << entityName;
    throw LimaException("MediaticData::getEntityType unknown entity group id");
  }
  try {
    return EntityType(m_d->m_entityTypes[groupId]->get(entityName),groupId);
  }
  catch(LimaException& e) {
    MDATALOGINIT;
    LWARN << "Unknown entity type " << entityName << "in group id:"<<groupId<<"; exception:" << e.what();
    throw;
  }
}

EntityGroupId MediaticData::getEntityGroupId(const LimaString& groupName) const
{
  try {
    return m_d->m_entityGroups.get(groupName);
  }
  catch(LimaException& e) {
    MDATALOGINIT;
    LERROR << "Unknown entity group " << groupName << e.what();
    throw;
  }
}

LimaString MediaticData::getEntityName(const EntityType& type) const
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticData::getEntityName("  << type << ")";
  /*if (logger.loggingLevel()<=QsLogging::TraceLevel) {
    printEntities(logger,m_d->m_entityGroups,m_d->m_entityTypes);
  }*/
#endif
  if (type.getGroupId()==0) {
    MDATALOGINIT;
    LERROR << "MediaticData::getEntityName invalid entity group id " << type.getGroupId() << " in entity " << type;
    throw LimaException();
  }
  if (static_cast<size_t>(type.getGroupId())>=m_d->m_entityTypes.size()) {
    MDATALOGINIT;
    LERROR << "MediaticData::getEntityName type.getGroupId()=" << type.getGroupId()<<" > m_entityTypes.size()=" << m_d->m_entityTypes.size();
    LERROR << "MediaticData::getEntityName unknown entity group id " << type.getGroupId() << " in entity " << type;
    throw LimaException();
  }
  try {
    // return m_entityTypes[type.getGroupId()]->get(type.getTypeId());
    return
      m_d->m_entityGroups.get(type.getGroupId())+
      m_d->s_entityTypeNameSeparator+
      m_d->m_entityTypes[type.getGroupId()]->get(type.getTypeId());
  }
  catch(LimaException& ) {
    MDATALOGINIT;
    LERROR << "Cannot find name of entity type " << type;
    throw;
  }
}

const LimaString& MediaticData::getEntityGroupName(EntityGroupId id) const
{
  try {
    return m_d->m_entityGroups.get(id);
  }
  catch(LimaException& e) {
    MDATALOGINIT;
    LERROR << "Cannot find name of entity group "
           << id << ":" << e.what();
    throw;
  }
}

void MediaticData::writeEntityTypes(std::ostream& file) const
{
#ifdef DEBUG_CD
  MDATALOGINIT;
#endif

  const DoubleAccessObjectToIdMap<LimaString,EntityGroupId>::AccessMap& groups=m_d->m_entityGroups.getAccessMap();
  Misc::writeCodedInt(file,groups.size());
  for (DoubleAccessObjectToIdMap<LimaString,EntityGroupId>::AccessMap::const_iterator 
         it=groups.begin(),it_end=groups.end();it!=it_end; it++) {
#ifdef DEBUG_CD
    LDEBUG << "writeEntityTypes: write group id " << (*it).second;
#endif
    Misc::writeCodedInt(file,(*it).second);
#ifdef DEBUG_CD
    LDEBUG << "writeEntityTypes: write group name " << *((*it).first);
#endif
    Misc::writeUTF8StringField(file,*((*it).first));
#ifdef DEBUG_CD
    LDEBUG  << "writeEntityTypes: after group name file at " << file.tellp();
#endif
    // write entities for this group
    const DoubleAccessObjectToIdMap<LimaString,EntityTypeId>::AccessMap& entities=m_d->m_entityTypes[(*it).second]->getAccessMap();
#ifdef DEBUG_CD
    LDEBUG << "writeEntityTypes: write nb entities: " << entities.size();
#endif
    Misc::writeCodedInt(file,entities.size());
#ifdef DEBUG_CD
    LDEBUG  << "writeEntityTypes: after write nb entities, file at " << file.tellp();
#endif
    for (DoubleAccessObjectToIdMap<LimaString,EntityTypeId>::AccessMap::const_iterator 
           it2=entities.begin(),it2_end=entities.end();it2!=it2_end; it2++) {
#ifdef DEBUG_CD
      LDEBUG << "writeEntityTypes: write entity id " << (*it2).second;
#endif
      Misc::writeCodedInt(file,(*it2).second);
#ifdef DEBUG_CD
      LDEBUG  << "writeEntityTypes: after write entity id file at " << file.tellp();
      LDEBUG << "writeEntityTypes: write entity name " << *((*it2).first);
#endif
      Misc::writeUTF8StringField(file,*((*it2).first));
 #ifdef DEBUG_CD
   LDEBUG  << "writeEntityTypes: after write entity name file at " << file.tellp();
#endif
    }
  }
#ifdef DEBUG_CD
  /*if (logger.loggingLevel()<=QsLogging::TraceLevel) {
    printEntities(logger,m_d->m_entityGroups,m_d->m_entityTypes);
  }*/
#endif
}

void MediaticData::readEntityTypes(std::istream& file,
                std::map<EntityGroupId,EntityGroupId>& entityGroupIdMapping,
                std::map<EntityType,EntityType>& entityTypeMapping)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticData::readEntityTypes from binary file with its mapping";
#endif
  uint64_t size=Misc::readCodedInt(file);
  // read group names
  for (uint64_t i(0); i<size; i++) 
  {
    EntityGroupId groupId= static_cast<EntityGroupId>(Misc::readCodedInt(file));
#ifdef DEBUG_CD
    LDEBUG << "readEntityTypes: read group id " << groupId;
#endif
    LimaString groupName;
    Misc::readUTF8StringField(file,groupName);
#ifdef DEBUG_CD
    LDEBUG << "readEntityTypes: read group name " << groupName;
#endif
    EntityGroupId newGroupId=addEntityGroup(groupName);
    entityGroupIdMapping[groupId]=newGroupId;
#ifdef DEBUG_CD
    LDEBUG << "readEntityTypes: added group id mapping " << groupId << "->" << newGroupId;
#endif
    // read entities for this group
    uint64_t nbEntities=Misc::readCodedInt(file);
    
    for (uint64_t j(0);j<nbEntities; j++) {
      EntityTypeId typeId = static_cast<EntityTypeId>(Misc::readCodedInt(file));
#ifdef DEBUG_CD
      LDEBUG << "readEntityTypes: read entity id " << typeId;
#endif
      LimaString entityName;
      Misc::readUTF8StringField(file,entityName);
#ifdef DEBUG_CD
      LDEBUG << "readEntityTypes: read entity name " << entityName;
      /*if (logger.loggingLevel()<=QsLogging::TraceLevel) {
        printEntities(logger,m_d->m_entityGroups,m_d->m_entityTypes);
      }*/
#endif
      EntityType oldTypeId(typeId,groupId);
      EntityType newTypeId=addEntity(newGroupId,entityName);
#ifdef DEBUG_CD
      LDEBUG << "readEntityTypes: added entity type mapping " << oldTypeId << "->" << newTypeId;
      LDEBUG << "before insert " << entityTypeMapping.size();
#endif
      entityTypeMapping.insert(make_pair(oldTypeId,newTypeId));
#ifdef DEBUG_CD
      LDEBUG << "after insert " << entityTypeMapping.size();
      if (logger.loggingLevel()<=QsLogging::TraceLevel) {
        LDEBUG << "readEntityTypes: type mapping is";
        std::ostringstream oss;
        for (std::map<EntityType,EntityType>::const_iterator
               it=entityTypeMapping.begin(),it_end=entityTypeMapping.end();
         it!=it_end; it++) {
          oss << (*it).first << " -> " << (*it).second << std::endl;
        }
        LDEBUG << oss.str();
      }
#endif
    }
  }
#ifdef DEBUG_CD
  /*if (logger.loggingLevel()<=QsLogging::TraceLevel) {
    printEntities(logger,m_d->m_entityGroups,m_d->m_entityTypes);
  }*/
#endif
}

const FsaStringsPool& MediaticData::stringsPool(MediaId med) const
{
  map<MediaId,FsaStringsPool*>::const_iterator it=m_d->m_stringsPool.find(med);
  if (it == m_d->m_stringsPool.end())
  {
    MDATALOGINIT;
    LERROR << "no available string pool for media " << (int)med;
    throw MediaNotInitialized(med);
  }
  return *(it->second);
}

FsaStringsPool& MediaticData::stringsPool(MediaId med)
{
  map<MediaId,FsaStringsPool*>::const_iterator it=m_d->m_stringsPool.find(med);
  if (it == m_d->m_stringsPool.end())
  {
    MDATALOGINIT;
    LERROR << "no available string pool for media " << (int)med;
    throw MediaNotInitialized(med);
  }
  return *(it->second);
}

MediaticDataPrivate::MediaticDataPrivate() :
    m_mediasIds(),
    m_mediasSymbol(),
    m_mediaDefinitionFiles(),
    m_mediasData(),
    m_entityGroups(),
    m_entityTypes(),
    m_relTypes(),
    m_relTypesNum(),
    m_stringsPool(),
    m_releaseStringsPool(false),
    m_resourcesPath(),
    m_configPath(),
    m_conceptTypes(),
    m_conceptNames()
{
  // null first element
  m_entityTypes.push_back( static_cast<EntityTypeMap*>(0));
}

MediaticDataPrivate::~MediaticDataPrivate()
{
  m_mediasIds.clear();
  for (map<MediaId, MediaData*>::const_iterator it=m_mediasData.begin();
       it!=m_mediasData.end();
       it++)
  {
    delete it->second;
  }
  for (auto it = m_entityTypes.begin(); it != m_entityTypes.end(); it++)
  {
    delete *it;
  }
  for (auto it = m_stringsPool.begin(); it != m_stringsPool.end(); it++)
  {
    delete it->second;
  }
}

const LimaString& MediaticData::getEntityTypeNameSeparator() const
{
  return m_d->s_entityTypeNameSeparator;
}

} // closing namespace MediaticData
} // closing namespace Common
} // closing namespace Lima
