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
  * @file          mediaticData.h
  * @author        Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *                Copyright (C) 2002-2012 by CEA LIST
  * @date          Started on Mon dec, 2 2002
  */


#ifndef LIMA_COMMONS_MEDIATICDATA_H
#define LIMA_COMMONS_MEDIATICDATA_H

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "common/misc/fsaStringsPool.h"
#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/MediaticData/EntityType.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

#include "mediaData.h"

#include <cstdlib>
#include <string>
#include <map>
#include <set>

#define ENTITY_TYPE_NONE_STRING "NO_TYPE"
#define ENTITY_TYPE_NONE 0

namespace Lima
{
namespace Common
{
namespace MediaticData
{
#define ID_NONE 0
#define SYMBOLIC_NONE "NONE"

class MediaticDataPrivate;

/**
 * @brief holds data about codes and names
 * for grammatical categories, etc.
 *
 * @author Gael de Chalendar
 */
class LIMA_MEDIATICDATA_EXPORT MediaticData : public Singleton<MediaticData>
{

    friend class Singleton<MediaticData>;

public:

    /**
      * initializes the system with the configuration file.
      * @param configurationFileName @b IN path to main configuration file
      * @param media @b IN the only media to load. If not specified then load all
      *                       activated medias in configuration file.
      */
    void init(
        const std::string& resourcesPath,
        const std::string& configPath,
        const std::string& configFile,
        const std::deque< std::string >& meds);

    void initMedia(const std::string& media);

    void initMediaData(MediaId med);

    const FsaStringsPool& stringsPool(MediaId med) const;
    FsaStringsPool& stringsPool(MediaId med);


    /*********************************************************************
      * Ids convertion functions
      ********************************************************************/

    const std::map< std::string, MediaId >& getMediasIds() const;
    /** @return the string value of the given numerical Media ID */

    const std::string& getMediaId(MediaId idNum) const;
    /** @return the numerical value of the given string Media id (3 chars code) */
    MediaId getMediaId(const std::string& stringId) const;

    const std::string& media(MediaId media) const;
    MediaId media(const std::string& media) const;

    const std::deque<std::string>& getMedias() const;

    const MediaData& mediaData(MediaId media) const;
    const MediaData& mediaData(const std::string& med) const;
    MediaData& mediaData(MediaId media);

    /// @{ entity types manager

    EntityType getEntityType(const LimaString& entityName) const;
    EntityType getEntityType(EntityGroupId groupId, const LimaString& entityName) const;
    EntityGroupId getEntityGroupId(const LimaString& groupName) const;

    //const LimaString& getEntityName(const EntityType& type) const;
    //cannot return const ref : name is build from group name and entity name
    LimaString getEntityName(const EntityType& type) const;
    const LimaString& getEntityGroupName(EntityGroupId id) const;

    EntityGroupId addEntityGroup(const LimaString& groupName);
    EntityType addEntity(const LimaString& groupName, const LimaString& entityName);
    EntityType addEntity(EntityGroupId groupId, const LimaString& entityName);

    // simple implementation of entity taxonomy: child-parent links
    void addEntityParentLink(const EntityType& child, const EntityType& parent);
    bool isEntityAncestor(const EntityType& child, const EntityType& parent) const;
    
    // read/write function necessary because entity types
    // are saved in temporary files
    void writeEntityTypes(std::ostream& os) const;
    void readEntityTypes(std::istream& os,
                         std::map<EntityGroupId,EntityGroupId>& EntityGroupIdMapping,
                         std::map<EntityType,EntityType>& EntityTypeMapping);

    /// @}

    uint8_t getRelation(const std::string& relation) const ;
    const std::string& getRelation(uint8_t relation) const ;

    ConceptType getConceptType(const std::string& typeName) const;
    const std::string& getConceptName(const ConceptType& typeName) const;

    /*********************************************************************
      * Configuration functions
      ********************************************************************/

    bool releaseStringsPool() const;

    const std::string& getResourcesPath() const;

    const std::string& getConfigPath() const;

    const std::string& getConfigFile() const;

    /** @return the number of categories */
    size_t getNbCategories() const;

    virtual ~MediaticData();

    virtual void initEntityTypes(
        XMLConfigurationFiles::XMLConfigurationFileParser& configParser);

    const LimaString& getEntityTypeNameSeparator() const;

    bool isValidMedia(const std::string& media);
private:
  MediaticData();
  MediaticData(const MediaticData& md);
  MediaticData& operator=(const MediaticData& md);
  
  MediaticDataPrivate* m_d;

};


} // closing namespace MediaticData
} // closing namesapce Common
} // closing namespace Lima


#endif // LIMA_CONFIGURATION_MEDIATICDATA_H
