// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#ifdef DEBUG_LP
  MDATALOGINIT;
  LDEBUG << "MediaData:init" ;
#endif
}

SimpleFactory<MediaData,MediaData> mediaDataFactory ( MEDIADATA_CLASSID );

} // MediaticData
} // Common
} // Lima
