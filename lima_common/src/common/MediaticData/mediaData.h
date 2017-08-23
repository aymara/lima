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
 *   Copyright (C) 2009 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMONS_MEDIATICDATA_MEDIADATA_H
#define LIMA_COMMONS_MEDIATICDATA_MEDIADATA_H

#include "common/LimaCommon.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"
#include <stdexcept>
// #include <list>
#ifndef WIN32
#include <stdint.h> //uint32_t
#endif

#include <boost/serialization/strong_typedef.hpp>
#include <boost/cstdint.hpp>

namespace Lima
{
namespace Common
{
  namespace XMLConfigurationFiles {
    class XMLConfigurationFileParser;
    class GroupConfigurationStructure;
  }
namespace MediaticData
{
#define MEDIADATA_CLASSID "MediaData"

BOOST_STRONG_TYPEDEF(boost::uint32_t, ConceptType);

class MediaDataPrivate;
/**
  * @brief Holds mediatic data for one media
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @date
  *
  * Cette classe contient toutes les informations mediatiques intrinseque
  * a un media. Ces informations disponibles doivent etre globales et non
  * specifiques a un module.
  *
  */
class LIMA_MEDIATICDATA_EXPORT MediaData : public InitializableObject<MediaData>
{
  friend class MediaticData;

public:

  MediaData();
  MediaData(const MediaData& md);

  virtual ~MediaData();

  /** @brief initialize languageData with the given module.
    * @return statusCode
    * @retval SUCCESS_ID success
    * @retval INVALID_CONFIGURATION error in configuration
    */
  virtual void initialize(
    MediaId med,
    const std::string& resourcesPath,
    XMLConfigurationFiles::XMLConfigurationFileParser& conf);

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  /** @return the numerical id of the language */
  MediaId getMedia() const;
  

//   const PropertyCode::PropertyCodeManager& getPropertyCodeManager() const;

private:
  MediaData& operator=(const MediaData& md);
  MediaDataPrivate* m_d;
}; // end class

// inline const PropertyCode::PropertyCodeManager& MediaData::getPropertyCodeManager() const
//   { return m_propCodeManager; }


} // MediaticData
} // Common
} // Lima

#endif
