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

#include "mediaData.h"
#include "mediaticData.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/InitializableObjectFactory.h"
#include "common/AbstractFactoryPattern/InitializableObjectManager.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include <string>
#include <fstream>

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace Common {
namespace MediaticData
{

class MediaDataPrivate
{
  friend class MediaData;
  
  MediaDataPrivate() {};

  virtual ~MediaDataPrivate() {};


  MediaId m_media;

  //PropertyCode::PropertyCodeManager m_propCodeManager;

  //std::list< LinguisticCode > m_sentenceBreakMicros;
};

MediaData::MediaData() : InitializableObject<MediaData>(), m_d(new MediaDataPrivate())
{}

MediaData::MediaData(const MediaData& md) : InitializableObject<MediaData>(md), m_d(new MediaDataPrivate(*md.m_d))
{}

MediaData::~MediaData()
{
  delete m_d;
}


/** @return the numerical id of the language */
MediaId MediaData::getMedia() const {return m_d->m_media;}

void  MediaData::initialize ( MediaId med,
                              const std::string& resourcesPath,
                              XMLConfigurationFileParser& conf )
{
  LIMA_UNUSED(resourcesPath);
  LIMA_UNUSED(conf);
  m_d->m_media=med;
}

void MediaData::init (
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager )
{
  LIMA_UNUSED(unitConfiguration);
  LIMA_UNUSED(manager);
    MDATALOGINIT;
    LDEBUG << "MediaData:init" ;
}

SimpleFactory<MediaData,MediaData> mediaDataFactory ( MEDIADATA_CLASSID );

} // MediaticData
} // Common
} // Lima

// MediaData::Factory::getFactory("MediaData")::create(unitConfiguration,manager);
