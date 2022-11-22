// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @author        Gael de Chalendar <Gael.de-Chalendar@cea.fr>
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


// ---------------------------------------------------------------------------
//  Standard libraries includes
// ---------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <deque>

#include <QFileInfo>
#include <QReadWriteLock>
#include <QRegularExpression>
#include <QSet>
#include <QString>

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
  MediaticDataPrivate(const MediaticDataPrivate&) = delete;
  MediaticDataPrivate& operator=(const MediaticDataPrivate&) = delete;
  virtual ~MediaticDataPrivate() = default;

protected:

    virtual void initMedias(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser,
        const std::deque< std::string >& meds);

    void initReleaseStringsPool(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    void initRelations(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    void initEntityTypes(XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    void initConceptTypes(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    EntityType addEntity(const LimaString& groupName, const LimaString& entityName);
    EntityType addEntity(EntityGroupId groupId, const LimaString& entityName);

    void addEntityParentLink(const EntityType& child, const EntityType& parent);

    void initMediaData(MediaId med);

    void initMedia(const std::string& media);

    bool isValidMedia(const std::string& media);

    const MediaData& mediaData(MediaId media) const;

    const std::string& getMediaId(MediaId idNum);
    MediaId getMediaId(const std::string& stringId);

    EntityGroupId getEntityGroupId(const LimaString& groupName) const;

    EntityType getEntityType(const LimaString& groupName, const LimaString& entityName) const;

    EntityType getEntityType(EntityGroupId groupId, const LimaString& entityName) const;

    EntityGroupId addEntityGroup(const LimaString& groupName);

private:
    QReadWriteLock m_lock;
    bool m_alreadyInitialized = false;
    static const LimaString s_entityTypeNameSeparator;

    std::deque< std::string > m_medias;
    std::map< std::string, MediaId > m_mediasIds;
    std::map< MediaId, std::string > m_mediasSymbol;
    std::map< MediaId, QString > m_mediaDefinitionFiles;
    std::map< MediaId, std::shared_ptr<MediaData> > m_mediasData;


    std::map< std::string, std::string > m_options;

    // entity types
    typedef Common::Misc::DoubleAccessObjectToIdMap<LimaString,EntityGroupId> EntityGroupMap;
    typedef Common::Misc::DoubleAccessObjectToIdMap<LimaString,EntityTypeId> EntityTypeMap;
    EntityGroupMap m_entityGroups;
    std::vector< std::shared_ptr<EntityTypeMap> > m_entityTypes;
    EntityTypeHierarchy m_entityHierarchy;

    std::map< std::string, uint8_t > m_relTypes;
    std::map< uint8_t, std::string > m_relTypesNum;

    static std::string s_undefinedRelation;


    std::map<MediaId, std::shared_ptr<FsaStringsPool> > m_stringsPool;
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

MediaticDataPrivate::MediaticDataPrivate() :
    m_lock(QReadWriteLock::Recursive),
    m_releaseStringsPool(false)

{
  // null first element
  m_entityTypes.push_back( std::shared_ptr<EntityTypeMap>(nullptr));
}

MediaticData::MediaticData() :
    Singleton<MediaticData>(),
    m_d(new MediaticDataPrivate())
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

void MediaticData::init(
  const std::string& resourcesPath,
  const std::string& configPath,
  const std::string& configFile,
  const std::deque< std::string >& meds,
  const std::map< std::string, std::string >& opts)
{
  QWriteLocker lock(&m_d->m_lock);
//  TimeUtils::updateCurrentTime();
  MDATALOGINIT;
  LINFO << "MediaticData::init " << resourcesPath << " "
        << configPath << " " << configFile << "; already initialized?:" << m_d->m_alreadyInitialized;
  if (m_d->m_alreadyInitialized)
  {
#ifdef DEBUG_CD
    LERROR << "MediaticData::init already initialized. Updating";
#endif
    // return;
  }
  m_d->m_alreadyInitialized = true;
  if (m_d->m_resourcesPath.empty())
    m_d->m_resourcesPath = resourcesPath;
  else
    m_d->m_resourcesPath = resourcesPath + LIMA_PATH_SEPARATOR + m_d->m_resourcesPath ;
  if (m_d->m_configPath.empty())
    m_d->m_configPath = configPath;
  else
    m_d->m_configPath = configPath + LIMA_PATH_SEPARATOR + m_d->m_configPath;
  if (m_d->m_configFile.empty())
    m_d->m_configFile = configFile;
  else
    m_d->m_configFile = configFile + LIMA_PATH_SEPARATOR + m_d->m_configFile;

  for (auto [k, v]: opts)
  {
    m_d->m_options[k] = v;
  }

  //LINFO << "initialize XMLParser";
  QStringList configPaths = QString::fromUtf8(configPath.c_str()).split(LIMA_PATH_SEPARATOR);
  QStringList configFiles = QString::fromUtf8(configFile.c_str()).split(LIMA_PATH_SEPARATOR);
  bool configurationFileFound = false;
  for(QString confPath: configPaths)
  {
    for(QString confFile: configFiles)
    {
      if (QFileInfo::exists(confPath + "/" + confFile))
      {
        LDEBUG << "MediaticData::init parse configuration file: "
                << (confPath + "/" + confFile);
        configurationFileFound = true;
        XMLConfigurationFileParser configuration(confPath + "/" + confFile);

        LDEBUG << "MediaticData::init initialize global parameters";
        m_d->initReleaseStringsPool(configuration);

        m_d->initEntityTypes(configuration);

        m_d->initRelations(configuration);

        m_d->initConceptTypes(configuration);

        /**
          * initialize active medias
          */
        LINFO << "!!! MediaticData::init for ";
        for (auto it = meds.cbegin(); it != meds.cend(); it++)
          LINFO << "    " << (*it).c_str();

        m_d->initMedias(configuration, meds);
      }
      if (configurationFileFound) break;
    }
    if (configurationFileFound) break;
  }
  if (!configurationFileFound)
  {
    MDATALOGINIT;
    LERROR << "No configuration file has been found with" << configPath
            << "and" << configFile;
  }
  //LINFO << "Mediatic data initialization finished";
//  TimeUtils::logElapsedTime("MediaticDataInit");
}

bool MediaticDataPrivate::isValidMedia(const std::string& media)
{
  QRegularExpression re("^(ud-)?...?$");
  QRegularExpressionMatch match = re.match(media.c_str());
  return match.hasMatch();
}

void MediaticDataPrivate::initMedia(const std::string& media)
{
  if(!isValidMedia(media))
  {
    MDATALOGINIT;
    LERROR << "MediaId for string '" << media << "' is invalid. It will not be initialized ! ";
    throw MediaNotInitialized(media);
  }
//  TimeUtils::updateCurrentTime();
  MDATALOGINIT;

  std::deque< std::string > meds({media});
  LINFO << "MediaticData::initMedia" << media << "parse configuration file: " << m_configPath
        << "/" << m_configFile;
  XMLConfigurationFileParser configuration(findFileInPaths(m_configPath.c_str(), m_configFile.c_str()));
  initMedias(configuration, meds);

  initEntityTypes(configuration);
}

void MediaticData::initMedia(const std::string& media)
{
  QWriteLocker lock(&m_d->m_lock);
  return m_d->initMedia(media);
}

const std::string& MediaticDataPrivate::getMediaId(MediaId idNum)
{
  auto it = m_mediasSymbol.find(idNum);
  if (it == m_mediasSymbol.end())
  {
//     MDATALOGINIT;
//     LERROR << "No media id for " ;//<< (int)idNum;
    throw MediaNotInitialized(idNum);
  }
  return it->second;
}

const std::string& MediaticData::getMediaId(MediaId idNum) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->getMediaId(idNum);
}

MediaId MediaticDataPrivate::getMediaId(const std::string& stringId)
{
  if (stringId.empty())
  {
    MDATALOGINIT;
    LERROR << "MediaticData::getMediaId invalid empty argument stringId at"
            << __FILE__ << ", line" << __LINE__;
    throw std::runtime_error(
      std::string("MediaticData::getMediaId invalid empty argument stringId at ").c_str() );
  }
  auto it = m_mediasIds.find(stringId);
  if (it == m_mediasIds.end())
  {
    // try on-demand initialization
    MDATALOGINIT;
    LINFO << "MediaticData::getMediaId On-demand initialization of media" << stringId.c_str();
    initMedia(stringId);
    it = m_mediasIds.find(stringId);
    if (it == m_mediasIds.end())
    {
      // printing of error message suppressed as calling getMediaId on a
      // non-initialized media string can be wanted in case of on-demand media
      // initialization if this is a real error, the catcher can print this
      // message.
      MDATALOGINIT;
      LERROR << "MediaId for string " << stringId.c_str()
              << " is still not initialized after on-demand initialization ! ";
      throw MediaNotInitialized(stringId);
    }
  }
  return it->second;
}

MediaId MediaticData::getMediaId(const std::string& stringId) const
{
  QWriteLocker lock(&m_d->m_lock);
  return m_d->getMediaId(stringId);
}

const MediaData& MediaticDataPrivate::mediaData(MediaId media) const
{
  auto it = m_mediasData.find(media);
  if (it == m_mediasData.end())
  {
    MDATALOGINIT;
    LERROR << "Media data for id " << (int)media << " is not initialized ! ";
    throw MediaNotInitialized(media);
  }
  return *(it->second);
}

const MediaData& MediaticData::mediaData(MediaId media) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->mediaData(media);
}

const MediaData& MediaticData::mediaData(const std::string& med) const
{
  QReadLocker lock(&m_d->m_lock);
  MediaId medId=m_d->getMediaId(med);
  return m_d->mediaData(medId);
}

MediaData& MediaticData::mediaData(MediaId media)
{
  QReadLocker lock(&m_d->m_lock);
  auto it = m_d->m_mediasData.find(media);
  if (it == m_d->m_mediasData.end())
  {
    MDATALOGINIT;
    LERROR << "Media data for id " << (int)media << " is not initialized ! ";
    throw MediaNotInitialized(media);
  }
  return *(it->second);
}

void MediaticDataPrivate::initMedias(XMLConfigurationFileParser& configParser,
                                     const std::deque< std::string >& meds)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initMedias" << meds.size();
#endif

  std::set< std::string > qmeds;
  for (const auto& med: m_medias)
  {
    qmeds.insert(med);
  }

  // med_str can be modified below. thus, do not use const reference
  for (auto med_str: meds)
  {
    MediaId id(0);
    try
    {
      ModuleConfigurationStructure& mod_config = configParser.getModuleConfiguration("common");
      GroupConfigurationStructure& grp_config = mod_config.getGroupNamed("mediasIds");
      std::string value;
      bool key_found = grp_config.getParamsValueAtKey(med_str, value);
      if (!key_found)
      {
#ifdef DEBUG_CD
        LDEBUG << "There is no language '" << med_str.c_str() << "' in LIMA. Trying 'ud'.";
#endif
        if (med_str.find("ud-") != 0)
          med_str = std::string("ud-") + med_str;
        if (m_options.find("udlang") == m_options.end())
          m_options["udlang"] = med_str;
        else
          m_options["udlang"] = m_options["udlang"] + "," + med_str;
        med_str = "ud";
      }
      id = static_cast<MediaId>(std::atoi(configParser.getModuleGroupParamValue("common","mediasIds",med_str).c_str()));
#ifdef DEBUG_CD
      LDEBUG << "media '" << med_str.c_str() << "' has id " << id;
      LDEBUG << (void*)this << " initialize string pool";
#endif
    }
    catch (NoSuchParam& e)
    {
      MDATALOGINIT;
      LERROR << "Param common/mediasIds/" << med_str << "missing:" << e.what();
      throw InvalidConfiguration(std::string("Failed to init media ")+med_str+": "+e.what());
    }

    if (qmeds.find(med_str) != qmeds.end())
    {
      MDATALOGINIT;
      LINFO << "media" << med_str.c_str() << "already initialized: reinit";
      // clear initialization
      // do not need to erase from strings pool here: the smart pointer is replaced below
      // m_stringsPool.erase(id);
    }
    else
    {
      m_medias.push_back(med_str);
      qmeds.insert(med_str);
    }
    // always perform initialization, even if language already initialized
    // (allows dynamic reinitialization of lima client)
    // {
    try
    {
      // initialize strings pool
      m_stringsPool[id] = std::shared_ptr<FsaStringsPool>(nullptr);
      m_stringsPool[id].reset(new FsaStringsPool());

      m_mediasIds[med_str] = id;
      m_mediasSymbol[id] = med_str;

      QString deffile = QString::fromUtf8(configParser.getModuleGroupParamValue("common",
                                                                                "mediaDefinitionFiles",
                                                                                med_str).c_str());
      QStringList configPaths = QString::fromUtf8(m_configPath.c_str()).split(LIMA_PATH_SEPARATOR);
      bool mediaDefinitionFileFound = false;
      for(auto confPath = configPaths.begin(); confPath != configPaths.end(); ++confPath)
      {
        if (QFileInfo::exists(*confPath + "/" + deffile))
        {
          m_mediaDefinitionFiles[id] = (*confPath+"/"+deffile);
#ifdef DEBUG_CD
          LDEBUG << "media definition file for" << med_str << "; id" << id << "is" << m_mediaDefinitionFiles[id];
#endif
          mediaDefinitionFileFound = true;
          break;
        }
      }
      if (!mediaDefinitionFileFound)
      {
        MDATALOGINIT;
        LERROR << "No media definition file'" << deffile << "' has been found for media id" << id
                << "in config paths:" << configPaths;
        // Cleanup: remove data we just added concerning this failed language
        for (auto med = m_medias.begin(); med != m_medias.end(); med++)
        {
          if (*med == med_str)
            m_medias.erase(med);
        }
        m_mediasIds.erase(med_str);
        m_mediasSymbol.erase(id);
        m_mediasData.erase(id);
        m_mediaDefinitionFiles.erase(id);

        throw InvalidConfiguration();
      }
    }
    catch (NoSuchParam& e)
    {
      MDATALOGINIT;
      LERROR << "Param common/mediaDefinitionFiles/" << med_str << "missing:" << e.what();
      // Cleanup: remove data we just added concerning this failed language
      for (auto med = m_medias.begin(); med != m_medias.end(); med++)
      {
        if (*med == med_str)
          m_medias.erase(med);
      }
      m_mediasIds.erase(med_str);
      m_mediasSymbol.erase(id);
      m_mediaDefinitionFiles.erase(id);
      m_mediasData.erase(id);

      throw InvalidConfiguration(
        std::string("Failed to init media ")+(med_str)+": "+e.what());
    }
    LDEBUG << "MediaticData::initMedias" << med_str << "call initMediaData" << m_mediasIds[med_str];
    initMediaData(m_mediasIds[med_str]);
    // }
  }

#ifdef DEBUG_CD
  LDEBUG << "MediaticDataPrivate::initMedias done successfuly";
#endif
}

void MediaticDataPrivate::initMediaData(MediaId med)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initMediaData" << (int)med;
#endif
  auto it = m_mediaDefinitionFiles.find(med);
  if (it == m_mediaDefinitionFiles.end())
  {
    MDATALOGINIT;
    std::ostringstream oss;
    oss << "No media definition file for med id " << (int)med;
    LERROR << oss.str();
    const auto& media = m_mediasSymbol[med];
    LDEBUG << "Cleanup: remove data added previously concerning this failed language" << (int)med << media
           << "and then throw";
    for (auto medit = m_medias.begin(); medit != m_medias.end(); medit++)
    {
      if (*medit == media)
        m_medias.erase(medit);
    }
    m_mediasIds.erase(media);
    m_mediasSymbol.erase(med);
    m_mediasData.erase(med);
    throw InvalidConfiguration(oss.str());
  }
#ifdef DEBUG_CD
  LDEBUG << "MediaticDataPrivate::initMediaData Parse MediaConfigurationFile "
          << (it->second);
#endif
  XMLConfigurationFileParser parser(it->second);

#ifdef DEBUG_CD
  LDEBUG << "MediaticDataPrivate::initMediaData Class: "
          << parser.getModuleGroupParamValue("MediaData",
                                             "Class","class").c_str();
#endif
  std::string& param = parser.getModuleGroupParamValue("MediaData",
                                                       "Class",
                                                       "class");
  if (!param.empty())
  {
    MediaData* ldata = MediaData::Factory::getFactory(param)->create(
      parser.getModuleGroupConfiguration("MediaData","Class"),0);

    //   MediaData* ldata=new MediaData();
    m_mediasData[med] = std::shared_ptr<MediaData>(ldata);
    ldata->initialize(med, m_resourcesPath, parser);
  }
  else
  {
    MDATALOGINIT;
    LERROR << "Empty class name for MediaData/Class/class for media" << med;
    std::ostringstream oss;
    oss << "Empty class name for MediaData/Class/class for media " << med;
    // Cleanup: remove data added previously concerning this failed language
    for (auto medit = m_medias.begin(); medit != m_medias.end(); medit++)
    {
      if (*medit == m_mediasSymbol[med])
        m_medias.erase(medit);
    }
    m_mediasIds.erase(m_mediasSymbol[med]);
    m_mediasSymbol.erase(med);
    m_mediasData.erase(med);
    throw InvalidConfiguration(oss.str());
  }
}

/** @return the string value of the given relation numerical value */
const std::string& MediaticData::getRelation(uint8_t relation) const
{
  QReadLocker lock(&m_d->m_lock);
  auto it = m_d->m_relTypesNum.find(relation);
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
  QReadLocker lock(&m_d->m_lock);
  auto it = m_d->m_relTypes.find(relation);
  if (it == m_d->m_relTypes.end())
  {
    MDATALOGINIT;
    LWARN << "ask getRelation for undefined relation " << relation.c_str();
    return 0;
  }
  return ( (*it).second );
}

void MediaticDataPrivate::initRelations(XMLConfigurationFiles::XMLConfigurationFileParser& configParser)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initRelations";
#endif
  m_relTypes[s_undefinedRelation] = 0;
  m_relTypesNum[0] = s_undefinedRelation;

  try
  {
    const auto& rels = configParser.getModuleConfiguration("common")
      .getGroupNamed("semanticRelations").getMapAtKey("declaration");
    for (auto it = rels.cbegin(); it != rels.cend(); it++)
    {
      uint8_t relId = atoi(it->second.c_str());
#ifdef DEBUG_CD
      LDEBUG << "read relation " << it->first.c_str() << " -> " << (int)relId;
#endif
      m_relTypes[it->first]=relId;
      m_relTypesNum[relId]=it->first;
    }
  }
  catch (NoSuchGroup& e)
  {
    MDATALOGINIT;
    LNOTICE << "No group 'semanticRelations' in 'common' module of lima-common configuration file";
    // throw InvalidConfiguration(
    //   std::string("No group 'semanticRelations' in 'common' module of lima-common configuration file:")+e.what());
  }
  catch (NoSuchMap& e)
  {
    MDATALOGINIT;
    LERROR << "No map 'declaration' in 'semanticRelations' group of lima-common configuration file";
    throw InvalidConfiguration(
      std::string("No map 'declaration' in 'semanticRelations' group of lima-common configuration file:")+e.what());
  }
}

void MediaticDataPrivate::initConceptTypes(XMLConfigurationFiles::XMLConfigurationFileParser& configParser)
{
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticDataPrivate::initConceptTypes";
#endif

  try
  {
    const auto& mapping = configParser.getModuleConfiguration("common")
      .getGroupNamed("SemanticData").getMapAtKey("conceptTypes");
    for (auto it = mapping.cbegin(); it != mapping.cend(); it++)
    {
      ConceptType type = static_cast<ConceptType>(atoi(it->second.c_str()));
#ifdef DEBUG_CD
      LDEBUG << "read concept type " << it->first.c_str() << " -> " << type;
#endif
      m_conceptTypes[it->first] = type;
      m_conceptNames[type] = it->first;
    }
  }
  catch (NoSuchGroup& e)
  {
    MDATALOGINIT;
    LNOTICE << "No group 'SemanticData' in 'common' module of lima-common configuration file:" << e.what();
  }
  catch (NoSuchMap& e)
  {
    MDATALOGINIT;
    LERROR << "No map 'conceptTypes' in 'SemanticData' group of lima-common configuration file:" << e.what();
    throw InvalidConfiguration(e.what());
  }
}

ConceptType MediaticData::getConceptType(const std::string& typeName) const
{
  QReadLocker lock(&m_d->m_lock);
  if (m_d->m_conceptTypes.find(typeName)==m_d->m_conceptTypes.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type name " << typeName.c_str()
            << " not found. Returning value for LatticeDown (should be '0').";
    return (*(m_d->m_conceptTypes.find("LatticeDown"))).second;
  }
  else
  {
    return (*(m_d->m_conceptTypes.find(typeName))).second;
  }
}


const std::string& MediaticData::getConceptName(const ConceptType& type) const
{
  QReadLocker lock(&m_d->m_lock);
  if (m_d->m_conceptNames.find(type) == m_d->m_conceptNames.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type " << type
            << " not found. Returning for 0 (should be 'LatticeDown').";
    return (*(m_d->m_conceptNames.find(static_cast<ConceptType>(0)))).second;
  }
  else
  {
    return (*(m_d->m_conceptNames.find(type))).second;
  }
}


void MediaticDataPrivate::initReleaseStringsPool(
  XMLConfigurationFileParser& configParser)
{
  MDATALOGINIT;
  LINFO << "initializes the release of strings pool on each text: ";
  m_releaseStringsPool = false;
  try
  {
    std::string release = configParser.getModuleGroupParamValue("common",
                                                                "stringPool",
                                                                "release") ;
    std::istringstream releaseS(release);
    releaseS >> std::boolalpha >> m_releaseStringsPool;
    LINFO << int(m_releaseStringsPool);
  }
  catch (const NoSuchParam& )
  {
    LINFO << "ReleaseStringsPool parameter not found. Using " << m_releaseStringsPool;
  }
}


//***********************************************************************
// entity types initialization

// internal output function for debug
void printEntities(
    QsLogging::Logger& logger,
    const DoubleAccessObjectToIdMap<LimaString,EntityGroupId>& groups,
    const std::vector<DoubleAccessObjectToIdMap<LimaString,
                                                EntityTypeId>* >& types)
{
  const auto& g = groups.getAccessMap();
  for (auto it = g.cbegin(); it != g.cend(); it++)
  {
    LDEBUG << *((*it).first) << "(" << (*it).first << ")"
            << "->" << (*it).second;
    if ((*it).second < types.size())
    {
      const auto& t = types[(*it).second]->getAccessMap();
      for (auto it2 = t.cbegin() ; it2 != t.cend(); it2++)
      {
        LDEBUG << "   " << *((*it2).first) << "(" << (*it2).first << ")"
                << "->" << (*it2).second;
      }
    }
  }
  // reverse maps
  const auto& rg = groups.getReverseAccessMap();
  for (uint32_t i(0); i<rg.size(); i++)
  {
    LDEBUG << "reverse " << i << "->" << rg[i];
    if (rg[i]!=0)
    {
      const auto& rt = types[i]->getReverseAccessMap();
      for (uint32_t j(0);j<rt.size(); j++)
      {
        LDEBUG << "    reverse " << j << "->" << rt[j];
      }
    }
  }
}

void MediaticDataPrivate::initEntityTypes(XMLConfigurationFileParser& configParser)
{
  MDATALOGINIT;
  LINFO << "MediaticData::initEntityTypes" << configParser.getConfigurationFileName();
  // look at all groups : ModuleConfigurationStructure is a map
  try
  {
    auto& moduleConf = configParser.getModuleConfiguration("entities");

    for (auto it = moduleConf.begin(); it != moduleConf.end(); it++)
    {
#ifdef DEBUG_CD
      LDEBUG << "initEntityTypes: looking at group " << (*it).first;
#endif

      auto groupName = QString::fromStdString((*it).first);

      if (groupName == "include")
      {
        auto includeList = moduleConf.getListValuesAtKeyOfGroupNamed("includeList", "include");
        string::size_type i;
        string moduleName("");
        for (std::size_t k=0; k<includeList.size(); k++)
        {
          i=includeList[k].find("/");
          if (i==string::npos)
          {
            LERROR << "Cannot include resources " << includeList[k] << ": must specify file and module name";
            continue;
          }
          auto configPaths = QString::fromStdString(m_configPath).split(LIMA_PATH_SEPARATOR);
          for(auto confPath : configPaths)
          {
            if (QFileInfo::exists(confPath + "/" + string(includeList[k],0,i).c_str()))
            {
              auto fileName= confPath + "/" + std::string(includeList[k], 0,i).c_str();
              XMLConfigurationFileParser lpconfig2(fileName);
              MediaticData::MediaticData::changeable().initEntityTypes(lpconfig2);
              break;
            }
          }
        }
      }
      else
      {
#ifdef DEBUG_CD
        LDEBUG << "initEntityTypes: read list as " << groupName;
#endif
        addEntityGroup(groupName);
        auto& groupConf=(*it).second;
        try
        {
          deque<string>& entityList = groupConf.getListsValueAtKey("entityList");
          for (const auto& ent: entityList)
          {
            auto entityName = QString::fromStdString(ent);
#ifdef DEBUG_CD
            LDEBUG << "initEntityTypes: add entityType " << ent.c_str() << " in group " << groupName;
#endif
            auto type = addEntity(groupName, entityName);
#ifdef DEBUG_CD
            LDEBUG << "initEntityTypes: type is " << type;
#endif
          }
        }
        catch(NoSuchList& )
        {
          // no simple list: may be list of items with attributes (to deal with isA relations of entities)
          auto& items = groupConf.getListOfItems("entityList");
          for (const auto& i: items)
          {
            auto entityName = QString::fromStdString(i.getName());
#ifdef DEBUG_CD
            LDEBUG << "initEntityTypes: add entityType " << i.getName() << " in group " << groupName;
#endif
            auto ent = addEntity(groupName, entityName);
#ifdef DEBUG_CD
            LDEBUG << "initEntityTypes: type is " << ent;
#endif
            if (i.hasAttribute(QLatin1String("isA")))
            {
              auto parentName = utf8stdstring2limastring(i.getAttribute("isA"));
              EntityType parent;
              try
              {
                parent = getEntityType(groupName, parentName);
  #ifdef DEBUG_CD
                LDEBUG << "initEntityTypes: add parent link:" << ent << "->" << parent;
  #endif
              }
              catch (const LimaException& e)
              {
                LIMA_EXCEPTION( "Unknown entity type" << groupName << parentName);
              }
              addEntityParentLink(ent, parent);
            }
          }
        }
      }
    }
  }
  catch(NoSuchModule& e)
  {
    MDATALOGINIT;
    LWARN << "no module 'entities' in entity types configuration:" << e.what();
  }
  catch(NoSuchGroup& e)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "missing group in entity types configuration:" << e.what();
    LERROR << errorString;
    throw InvalidConfiguration(errorString.toStdString());
  }
  catch(NoSuchList& e)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "missing list 'entityList' in entity types configuration:" << e.what();
    LERROR << errorString;
    throw InvalidConfiguration(errorString.toStdString());
  }
}

void MediaticData::initEntityTypes(XMLConfigurationFileParser& configParser)
{
  QWriteLocker lock(&m_d->m_lock);
  return m_d->initEntityTypes(configParser);
}

EntityGroupId MediaticDataPrivate::addEntityGroup(const LimaString& groupName)
{
  EntityGroupId groupId= m_entityGroups.insert(groupName);
  // insert may have created new element or not
  if (static_cast<std::size_t>(groupId) >= m_entityTypes.size())
  {
    m_entityTypes.push_back( std::make_shared<MediaticDataPrivate::EntityTypeMap>());
  }
  return groupId;
}

EntityGroupId MediaticData::addEntityGroup(const LimaString& groupName)
{
  QWriteLocker lock(&m_d->m_lock);
  return m_d->addEntityGroup(groupName);
}

EntityType MediaticDataPrivate::addEntity(EntityGroupId groupId, const LimaString& entityName)
{
  if (static_cast<std::size_t>(groupId)>=m_entityTypes.size())
  {
    MDATALOGINIT;
    LIMA_EXCEPTION("MediaticData::addEntity unknown entity group id " << groupId
        << "adding" << entityName);
  }
  EntityTypeId typeId= m_entityTypes[groupId]->insert(entityName);
  return EntityType(typeId,groupId);
}

EntityType MediaticDataPrivate::addEntity(const LimaString& groupName, const LimaString& entityName)
{
  auto groupId = getEntityGroupId(groupName);
  return addEntity(groupId, entityName);
}

EntityType MediaticData::addEntity(const LimaString& groupName, const LimaString& entityName)
{
  QWriteLocker lock(&m_d->m_lock);
  return m_d->addEntity(groupName, entityName);
}

void MediaticDataPrivate::addEntityParentLink(const EntityType& child, const EntityType& parent)
{
  m_entityHierarchy.addParentLink(child,parent);
}

void MediaticData::addEntityParentLink(const EntityType& child, const EntityType& parent)
{
  QWriteLocker lock(&m_d->m_lock);
  m_d->addEntityParentLink(child, parent);
}

bool MediaticData::isEntityAncestor(const EntityType& child,
                                    const EntityType& parent) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->m_entityHierarchy.isAncestor(child,parent);
}

EntityType MediaticData::getEntityAncestor(const EntityType& child) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->m_entityHierarchy.getAncestor(child);
}

bool MediaticData::getEntityChildList(const EntityType& parent,
                        std::map<EntityType,EntityType>& childList) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->m_entityHierarchy.getChildren(parent, childList);
}

std::vector<EntityType> MediaticData::getGroupAncestors(EntityGroupId groupId) const
{
  QReadLocker lock(&m_d->m_lock);
  std::vector<EntityType> ancestors;
  const auto& accessMap = m_d->m_entityTypes[groupId]->getAccessMap();
  for(auto iter = accessMap.begin(), iter_end=accessMap.end(); iter != iter_end; iter++){
    const LimaString entityName = *(iter->first);
    int i=entityName.indexOf(m_d->s_entityTypeNameSeparator);
    if (i>0) continue;
    EntityType entityType;
    try {
      entityType = m_d->getEntityType( groupId, entityName );
    } catch (const LimaException& e) {
      MDATALOGINIT;
      LIMA_EXCEPTION("MediaticData::getEntityType unknown entity" << groupId
                      << entityName << e.what());
    }
    if ( entityType == this->getEntityAncestor(entityType) ) {
        ancestors.push_back( entityType );
    }
  }
  return ancestors;
}


// entity types accessors
EntityType MediaticData::getEntityType(const LimaString& entityName) const
{
  QReadLocker lock(&m_d->m_lock);
  int i=entityName.indexOf(m_d->s_entityTypeNameSeparator);
  if (i==-1)
  {
    MDATALOGINIT;
    LIMA_EXCEPTION( "MediaticData::getEntityType missing group name in entity name '" << entityName << "'" );
  }
  LimaString groupName = entityName.left(i);
  LimaString name = entityName.mid(i+m_d->s_entityTypeNameSeparator.length());
  EntityType result;
  try {
    result = getEntityType(groupName, name);
  } catch (const LimaException& e) {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << __FILE__ << ":" << __LINE__ << ": "
        << "MediaticData::getEntityType unknown entity " << groupName
        << m_d->s_entityTypeNameSeparator << name << " : " << e.what() ;
    LDEBUG << errorString;
    throw LimaException(errorString);
  }
  return result;
}

EntityType MediaticDataPrivate::getEntityType(const LimaString& groupName, const LimaString& entityName) const
{
  auto groupId = getEntityGroupId(groupName);
  return getEntityType(groupId, entityName);
}

EntityType MediaticData::getEntityType(const LimaString& groupName, const LimaString& entityName) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->getEntityType(groupName, entityName);
}

EntityType MediaticDataPrivate::getEntityType(const EntityGroupId groupId, const LimaString& entityName) const
{
  if (static_cast<size_t>(groupId) >= m_entityTypes.size())
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityType unknown entity group id " << groupId
            << " accessing '" << entityName << "'";
    LERROR << errorString;
    throw LimaException(errorString.toStdString());
  }

  if (! m_entityTypes[groupId]->hasValue(entityName))
  {
    MDATALOGINIT;
    //LIMA_EXCEPTION( "MediaticData::getEntityType Unknown entity type '" << entityName << "' in group id "<<groupId);
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityType no entity type '" << entityName
        << "' in group id "<<groupId;
    LDEBUG << errorString;
    throw LimaException(errorString.toStdString());
  }

  EntityTypeId typeId;
  try {
    typeId = m_entityTypes[groupId]->get(entityName);
  } catch (const LimaException& e) {
    MDATALOGINIT;
    LIMA_EXCEPTION("Exception while getting entity " << groupId << "."
                    << entityName << ": " << e.what())
  }
  return EntityType(typeId,groupId);
}

EntityType MediaticData::getEntityType(const EntityGroupId groupId, const LimaString& entityName) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->getEntityType(groupId, entityName);
}

EntityGroupId MediaticDataPrivate::getEntityGroupId(const LimaString& groupName) const
{
  try
  {
    return m_entityGroups.get(groupName);
  }
  catch(LimaException& e)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityGroupId Unknown entity group '" << groupName << "'; exception: "<< e.what();
    LWARN << errorString;
    throw;
  }
}

EntityGroupId MediaticData::getEntityGroupId(const LimaString& groupName) const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->getEntityGroupId(groupName);
}

LimaString MediaticData::getEntityName(const EntityType& type) const
{
  QReadLocker lock(&m_d->m_lock);
#ifdef DEBUG_CD
  MDATALOGINIT;
  LDEBUG << "MediaticData::getEntityName(" << type << ")";
#endif
  if (type.getGroupId()==0)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityName invalid entity group id "
        << type.getGroupId() << " in entity " << type;
    LERROR << errorString;
    throw LimaException(errorString.toStdString());
  }
  if (static_cast<size_t>(type.getGroupId())>=m_d->m_entityTypes.size())
  {
    MDATALOGINIT;
    LIMA_EXCEPTION( "MediaticData::getEntityName type.getGroupId()="
        << type.getGroupId()<<" > m_entityTypes.size()="
        << m_d->m_entityTypes.size() << " for entity type " << type );
  }
  try
  {
    // return m_entityTypes[type.getGroupId()]->get(type.getTypeId());
    return
      m_d->m_entityGroups.get(
        type.getGroupId())
        + m_d->s_entityTypeNameSeparator
        + m_d->m_entityTypes[type.getGroupId()]->get(type.getTypeId());
  }
  catch(LimaException& e)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityName Cannot find name of entity type " << type << "'; exception: "<< e.what();
    LWARN << errorString;
    throw;
  }
}

const LimaString& MediaticData::getEntityGroupName(EntityGroupId id) const
{
  QReadLocker lock(&m_d->m_lock);
  try
  {
    return m_d->m_entityGroups.get(id);
  }
  catch(LimaException& e)
  {
    MDATALOGINIT;
    QString errorString;
    QTextStream qts(&errorString);
    qts << "MediaticData::getEntityGroupName Cannot find name of entity group "
           << id << "; exception: " << e.what();
    LWARN << errorString;
    throw;
  }
}

void MediaticData::writeEntityTypes(std::ostream& file) const
{
  QReadLocker lock(&m_d->m_lock);
#ifdef DEBUG_CD
  MDATALOGINIT;
#endif

  const auto& groups = m_d->m_entityGroups.getAccessMap();
  Misc::writeCodedInt(file,groups.size());
  for (auto it = groups.cbegin(); it != groups.cend(); it++)
  {
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
    const auto& entities = m_d->m_entityTypes[(*it).second]->getAccessMap();
#ifdef DEBUG_CD
    LDEBUG << "writeEntityTypes: write nb entities: " << entities.size();
#endif
    Misc::writeCodedInt(file,entities.size());
#ifdef DEBUG_CD
    LDEBUG  << "writeEntityTypes: after write nb entities, file at " << file.tellp();
#endif
    for (auto it2 = entities.cbegin(); it2 != entities.end(); it2++)
    {
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
  QWriteLocker lock(&m_d->m_lock);
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
    EntityGroupId newGroupId=m_d->addEntityGroup(groupName);
    entityGroupIdMapping[groupId]=newGroupId;
#ifdef DEBUG_CD
    LDEBUG << "readEntityTypes: added group id mapping " << groupId
            << "->" << newGroupId;
#endif
    // read entities for this group
    uint64_t nbEntities=Misc::readCodedInt(file);

    for (uint64_t j(0);j<nbEntities; j++)
    {
      auto typeId = static_cast<EntityTypeId>(Misc::readCodedInt(file));
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
      EntityType newTypeId = addEntity(groupName, entityName);
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
        for (auto  it = entityTypeMapping.begin();
             it != entityTypeMapping.end(); it++)
        {
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

bool MediaticData::getOptionValue(const std::string& name, std::string& value) const
{
  QReadLocker lock(&m_d->m_lock);
  std::map< std::string, std::string >::const_iterator it = m_d->m_options.find(name);
  if (it == m_d->m_options.end())
    return false;

  value = it->second;
  return true;
}

const FsaStringsPool& MediaticData::stringsPool(MediaId med) const
{
  QReadLocker lock(&m_d->m_lock);
  auto it = m_d->m_stringsPool.find(med);
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
  QReadLocker lock(&m_d->m_lock);
  auto it = m_d->m_stringsPool.find(med);
  if (it == m_d->m_stringsPool.end())
  {
    MDATALOGINIT;
    LERROR << "no available string pool for media " << (int)med;
    throw MediaNotInitialized(med);
  }
  return *(it->second);
}

const LimaString& MediaticData::getEntityTypeNameSeparator() const
{
  QReadLocker lock(&m_d->m_lock);
  return m_d->s_entityTypeNameSeparator;
}

} // closing namespace MediaticData
} // closing namespace Common
} // closing namespace Lima
