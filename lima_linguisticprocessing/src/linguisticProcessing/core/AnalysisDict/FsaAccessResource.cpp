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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "FsaAccessResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/tools/FileUtils.h"

#include <QReadLocker>
#include <QWriteLocker>
#include <QFileInfo>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common;
using namespace std;

namespace Lima
{
namespace Common {
  namespace FsaAccess {
  extern template class LIMA_FSAACCESS_EXPORT FsaAccessReader16<class boost::adjacency_list<struct boost::vecS,struct boost::vecS,struct boost::bidirectionalS,struct boost::property<enum vertex_count_t,class std::vector<int,class std::allocator<int> >,struct boost::property<enum boost::vertex_name_t,uint64_t,struct boost::property<enum vertex_text_t,class QString,struct boost::no_property> > >,struct boost::no_property,struct boost::no_property,struct boost::listS> >;
  }
}
namespace LinguisticProcessing
{
namespace AnalysisDict
{

#define FSAACCESSRESSOURCE_CLASSID "FsaAccess"

SimpleFactory<AbstractResource,FsaAccessResource> fsaAccessResourceFactory(FSAACCESSRESSOURCE_CLASSID);

FsaAccessResource::FsaAccessResource(QObject* parent)
    : AbstractAccessResource(parent),m_fsaAccess(0)
{
  connect(this,SIGNAL(resourceFileChanged(QString)),this,SLOT(accessFileChanged(QString)));
}


FsaAccessResource::~FsaAccessResource()
{
  if (m_fsaAccess!=0)
  {
    delete m_fsaAccess;
  }
}

void FsaAccessResource::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="FsaAccess"&gt;</b>
   *    -  keyFile : file containing the compiled access keys
   */
    
  ANALYSISDICTLOGINIT;
  try
  {
    QStringList resourcesPaths = QString::fromUtf8(Common::MediaticData::MediaticData::single().getResourcesPath().c_str()).split(LIMA_PATH_SEPARATOR);
    Q_FOREACH(QString resPath, resourcesPaths)
    {
      if  (QFileInfo::exists(resPath + "/" + unitConfiguration.getParamsValueAtKey("keyFile").c_str()))
      {
        string keyfile= (resPath + "/" + unitConfiguration.getParamsValueAtKey("keyFile").c_str()).toUtf8().constData();
        FsaAccess::FsaAccessSpare16* fsaAccess=new FsaAccess::FsaAccessSpare16();
        resourceFileWatcher().addPath(QString::fromUtf8(keyfile.c_str()));
        QWriteLocker locker(&m_lock);
        LINFO << "FsaAccessResource::init read keyFile" << QString::fromUtf8(keyfile.c_str());
        fsaAccess->read(keyfile);
        m_fsaAccess=fsaAccess;
        break;
      }
    }
    if (!m_fsaAccess) {
      // FIXME: In this case, the m_fsaAccess pointer is still NULL. Try to access to
      // this ressource will crash the application.
      LERROR << "resource file" << unitConfiguration.getParamsValueAtKey("keyFile") << "not found in path"
        << Common::MediaticData::MediaticData::single().getResourcesPath();
    }
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'keyFile' in FsaAccessResource group for language " << (int)  manager->getInitializationParameters().language;
    throw InvalidConfiguration();
  }
  catch (AccessByStringNotInitialized& )
  {
    LERROR << "keyfile "
           << Common::MediaticData::MediaticData::single().getResourcesPath()
           << "/"
           << unitConfiguration.getParamsValueAtKey("keyFile")
           << " no found for language " 
           << (int)  manager->getInitializationParameters().language;
    throw InvalidConfiguration();
  }
}

AbstractAccessByString* FsaAccessResource::getAccessByString() const
  { return m_fsaAccess;}

void FsaAccessResource::accessFileChanged ( const QString & path )
{
  ANALYSISDICTLOGINIT;
  // Check if the file exists as, when a file is replaced, accessFileChanged can be triggered 
  // two times, when it is first suppressed and when the new version is available. One should not 
  // try to load the missing file
  if (QFileInfo::exists(path))
  {
    LINFO << "FsaAccessResource::accessFileChanged reload" << path;
    FsaAccess::FsaAccessSpare16* fsaAccess=new FsaAccess::FsaAccessSpare16();
    QWriteLocker locker(&m_lock);
    fsaAccess->read(path.toUtf8().constData());
    delete m_fsaAccess;
    m_fsaAccess=fsaAccess;
    Q_EMIT accessFileReloaded(m_fsaAccess);
  }
  else
  {
    LINFO << "FsaAccessResource::accessFileChanged deleted, ignoring" << path;
  }
}

} // AnalysisDict
} // LinguisticProcessing
} // Lima
