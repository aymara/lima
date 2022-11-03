// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "NameIndexResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/tools/FileUtils.h"
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

/*
#include <QReadLocker>
#include <QWriteLocker>
#include <QFileInfo>
*/
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

#define NAMEINDEXRESSOURCE_CLASSID "NameIndex"

SimpleFactory<AbstractResource,NameIndexResource> fsaAccessResourceFactory(NAMEINDEXRESSOURCE_CLASSID);
/*
NameIndexResource::NameIndexResource(QObject* parent)
    : AbstractResource(parent),m_fsaAccess(0)
*/
NameIndexResource::NameIndexResource()
    : AbstractResource(0), m_withIndex(false)
{
/*
  connect(this,SIGNAL(resourceFileChanged(QString)),this,SLOT(accessFileChanged(QString)));
*/
}

NameIndexResource::~NameIndexResource()
{
}

void NameIndexResource::readNames( const std::string& filepath ) {
  MORPHOLOGINIT;
  std::ifstream names(filepath.data());
  char strbuff[200];

  for( int counter = 0 ; ; counter++ ) {
    // lecture d'une ligne du fichier
    names.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( line.size() == 0 ) {
      LDEBUG << "end of list of words. counter= " << counter;
      break;
    }
    else {
      // 
      Lima::LimaString namels = Lima::Common::Misc::utf8stdstring2limastring(line);
      int i=namels.indexOf('\t');
      if (i==-1) {
        insert(std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> >(
          L"",LimaStr2wcharStr( namels )));
      }
      else {
        insert(std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> >(
          LimaStr2wcharStr(namels.mid(i+1)),LimaStr2wcharStr(namels.left(i))));
      }
    }
  }
}

std::basic_string<wchar_t> NameIndexResource::LimaStr2wcharStr( const QString& limastr ) {
  // convert QString to std::basic_string<wchar_t>
      wchar_t warray[limastr.length()+1];
      int warray_len = limastr.toWCharArray(warray);
      warray[warray_len]=0;
      return std::basic_string<wchar_t>(warray, warray_len);
}


void NameIndexResource::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="NameIndex"&gt;</b>
   *    -  file : file containing the names with index
   */
    
  ANALYSISDICTLOGINIT;
  try
  {
    QString filepath = Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
                                                   unitConfiguration.getParamsValueAtKey("filename").c_str());
    struct stat sb;
    if( stat(filepath.toUtf8().constData(),&sb) == 0)
    {
      readNames(filepath.toUtf8().constData());
    }
    else {
      // FIXME: In this case, the m_fsaAccess pointer is still NULL. Try to access to
      // this ressource will crash the application.
      LERROR << "resource file" << filepath << "not found";
    }
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'filename' in NameIndex group for language " << (int)  manager->getInitializationParameters().language;
    throw InvalidConfiguration();
  }
  catch (AccessByStringNotInitialized& )
  {
    LERROR << "filename "
           << Common::MediaticData::MediaticData::single().getResourcesPath()
           << "/"
           << unitConfiguration.getParamsValueAtKey("filename")
           << " no found for language " 
           << (int)  manager->getInitializationParameters().language;
    throw InvalidConfiguration();
  }
  try
  {
    std::string withIndex = (unitConfiguration.getParamsValueAtKey("withIndex"));
    m_withIndex = (withIndex.compare("true") == 0);
  }
  catch (NoSuchParam& )
  {
    // parameter 'withIndex' is optional, default is false
  }
}

/*
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
*/

} // AnalysisDict
} // LinguisticProcessing
} // Lima
