/*
    Copyright 2002-2015 CEA LIST

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
#include "EnhancedAnalysisDictionary.h"
#include "EnhancedAnalysisDictionaryIterator.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <iostream>
#include <QReadWriteLock>
#include <QWriteLocker>
#include <QReadLocker>
#include <QFileInfo>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace std;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

SimpleFactory<AbstractResource,EnhancedAnalysisDictionary> enhancedAnalysisDictionaryFactory(ENHANCEDANALYSISDICTIONARY_CLASSID);


class EnhancedAnalysisDictionaryPrivate
{
  friend class EnhancedAnalysisDictionary;
  
  EnhancedAnalysisDictionaryPrivate();
  
  EnhancedAnalysisDictionaryPrivate(
    FsaStringsPool* sp,
    Lima::Common::AbstractAccessByString* access,
    const std::string& dataFile);

  ~EnhancedAnalysisDictionaryPrivate();

  Lima::Common::AbstractAccessByString* m_access;
  DictionaryData* m_dicoData;
  Lima::FsaStringsPool* m_sp;
  bool m_isMainKeys;
  // A read/write locker to prevent accessing m_dicoData during its loading
  QReadWriteLock m_lock;
};

EnhancedAnalysisDictionaryPrivate::EnhancedAnalysisDictionaryPrivate() :
    m_access(0),
    m_dicoData(new DictionaryData()),
    m_sp(0)
{
}

EnhancedAnalysisDictionaryPrivate::EnhancedAnalysisDictionaryPrivate(
      FsaStringsPool* sp,
      Lima::Common::AbstractAccessByString* access,
      const std::string& dataFile) :
   m_access(access),
   m_dicoData(new DictionaryData()),
   m_sp(sp)
{
  QWriteLocker locker(&m_lock);
  m_dicoData->loadBinaryFile(dataFile);
}


EnhancedAnalysisDictionaryPrivate::~EnhancedAnalysisDictionaryPrivate()
{
  delete m_dicoData;
}


EnhancedAnalysisDictionary::EnhancedAnalysisDictionary(const QString& dataFilePath)
    : AbstractAnalysisDictionary(),
    m_d(new EnhancedAnalysisDictionaryPrivate())
{
  ANALYSISDICTLOGINIT;
//   LDEBUG  << "EnhancedAnalysisDictionary::EnhancedAnalysisDictionary";
  if (!connect(this,SIGNAL(resourceFileChanged(QString)),this,SLOT(dictionaryFileChanged(QString))))
  {
    LERROR << "Unable to connect resourceFileChanged";
    throw LimaException("Unable to connect resourceFileChanged");
  }
  
  if (!dataFilePath.isEmpty())
  {
    resourceFileWatcher().addPath(dataFilePath);
    QWriteLocker locker(&m_d->m_lock);
    m_d->m_dicoData->loadBinaryFile(dataFilePath.toUtf8().constData());
  }
}

EnhancedAnalysisDictionary::EnhancedAnalysisDictionary(
      FsaStringsPool* sp,
      Lima::Common::AbstractAccessByString* access,
      const std::string& dataFile) :
   m_d(new EnhancedAnalysisDictionaryPrivate(sp, access, dataFile))
{
  ANALYSISDICTLOGINIT;
//   LDEBUG  << "EnhancedAnalysisDictionary::EnhancedAnalysisDictionary" << dataFile.c_str();
  if   (!connect(this,SIGNAL(resourceFileChanged(QString)),this,SLOT(dictionaryFileChanged(QString))))
  {
    LERROR << "Unable to connect resourceFileChanged";
    throw LimaException("Unable to connect resourceFileChanged");
  }
  resourceFileWatcher().addPath(QString::fromUtf8(dataFile.c_str()));
  QWriteLocker locker(&m_d->m_lock);
  m_d->m_dicoData->loadBinaryFile(dataFile);
}


EnhancedAnalysisDictionary::~EnhancedAnalysisDictionary()
{
  delete m_d;
}

void EnhancedAnalysisDictionary::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  ANALYSISDICTLOGINIT;
  LDEBUG  << "EnhancedAnalysisDictionary::init";
  MediaId language=manager->getInitializationParameters().language;
  m_d->m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);
  try
  {
    string accessId=unitConfiguration.getParamsValueAtKey("accessKeys");
    const AbstractResource* res=LinguisticResources::single().getResource(language,accessId);
    const AbstractAccessResource* aar=static_cast<const AbstractAccessResource*>(res);
    if (!connect(aar,SIGNAL(accessFileReloaded(Common::AbstractAccessByString*)),this,SLOT(slotAccessFileReloaded(Common::AbstractAccessByString*))))
    {
      LERROR << "Unable to connect accessFileReloaded";
      throw LimaException("Unable to connect accessFileReloaded");
    }
    m_d->m_isMainKeys=aar->isMainKeys();
    m_d->m_access=aar->getAccessByString();
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'accessKeys' in EnhancedAnalysisDictionary group for language " << (int) language;
    throw InvalidConfiguration();
  }
  try
  {
    QString binaryFilePath = Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
                                                   unitConfiguration.getParamsValueAtKey("dictionaryValuesFile").c_str());
    resourceFileWatcher().addPath(binaryFilePath);
    QWriteLocker locker(&m_d->m_lock);
    m_d->m_dicoData->loadBinaryFile(binaryFilePath.toUtf8().constData());
  }
  catch (NoSuchList& )
  {
    LERROR << "no list 'dictionaryValues' in EnhancedAnalysisDictionary group for language " << (int) language;
    throw InvalidConfiguration();
  }


}

void EnhancedAnalysisDictionary::slotAccessFileReloaded(Common::AbstractAccessByString* access)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "EnhancedAnalysisDictionary::slotAccessFileReloaded";
  m_d->m_access=access;
 }

void EnhancedAnalysisDictionary::dictionaryFileChanged ( const QString & path )
{
  ANALYSISDICTLOGINIT;
  // Check if the file exists as, when a file is replaced, dictionaryFileChanged can be triggered 
  // two times, when it is first suppressed and when the new version is available. One should not 
  // try to load the missing file
  if (QFileInfo(path).exists())
  {
    LINFO << "EnhancedAnalysisDictionary::dictionaryFileChanged reload" << path;
    QWriteLocker locker(&m_d->m_lock);
    if (m_d->m_dicoData != 0)
      delete m_d->m_dicoData;
    m_d->m_dicoData = new DictionaryData();
    m_d->m_dicoData->loadBinaryFile(path.toUtf8().constData());
  }
  else
  {
    LINFO << "EnhancedAnalysisDictionary::dictionaryFileChanged deleted, ignoring" << path;
  }
}

uint64_t EnhancedAnalysisDictionary::getSize() const
{
//   QReadLocker locker(&m_d->m_lock);
  return m_d->m_dicoData->getSize();
}

DictionaryEntry EnhancedAnalysisDictionary::getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const
{
  if (m_d->m_isMainKeys) return getEntryData(wordId);
  return getEntryData(static_cast<StringsPoolIndex>(m_d->m_access->getIndex(word)));
}

DictionaryEntry EnhancedAnalysisDictionary::getEntry(const Lima::LimaString& word) const
{
  return getEntryData( static_cast<StringsPoolIndex>(m_d->m_access->getIndex(word)) );
}

DictionaryEntry EnhancedAnalysisDictionary::getEntry(const StringsPoolIndex wordId) const
{
  if (m_d->m_isMainKeys) return getEntryData(wordId);
  return getEntryData( static_cast<StringsPoolIndex>(m_d->m_access->getIndex((*m_d->m_sp)[wordId])) );
}


DictionaryEntry EnhancedAnalysisDictionary::getEntryData(const StringsPoolIndex wordId) const
{
//  ANALYSISDICTLOGINIT;
//  LDEBUG << "getEntry " << wordId;
//   QReadLocker locker(&m_d->m_lock);
  if (wordId >= m_d->m_dicoData->getSize())
  {
//    LDEBUG << "return empty : index out of range";
    return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(static_cast<StringsPoolIndex>(0),false,true,false,false,false,0,0,m_d->m_dicoData,m_d->m_isMainKeys,m_d->m_access,m_d->m_sp));
  }
  
  StringsPoolIndex strId=wordId;
  if (!m_d->m_isMainKeys) {
    strId = (*m_d->m_sp)[m_d->m_access->getSpelling(wordId)];
  }
 
  unsigned char* p=m_d->m_dicoData->getEntryAddr(wordId);
  uint64_t read=DictionaryData::readCodedInt(p);
  bool final=false;
  if (read == 1)
  {
    final=true;
    read=DictionaryData::readCodedInt(p);
  }
  if (read == 0)
  {
//    LDEBUG << "return empty entry";
    return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(strId,final,true,false,false,false,p,p,m_d->m_dicoData,m_d->m_isMainKeys,m_d->m_access,m_d->m_sp));
  }
  unsigned char* start=p;
  unsigned char* end=p+read;
  bool hasLing=false;
  bool hasAccented=false;
  bool hasConcat=false;
  read=DictionaryData::readCodedInt(p);
  hasLing=(read != 0);
//  LDEBUG << "info length = " << read;
  p+=read;
  if (p!=end)
  {
    read=DictionaryData::readCodedInt(p);
//    LDEBUG << "accented length = " << read;
    hasAccented=(read != 0);
    p+=read;
    if (p!=end)
    {
      read=DictionaryData::readCodedInt(p);
//      LDEBUG << "concat length = " << read;
      hasConcat=(read != 0);
      p+=read;
    }
  }
  Q_ASSERT(p==end);
//  LDEBUG << "return entry " << (uint64_t)start << " , " << (uint64_t)end;
  return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(strId,final,false,hasLing,hasConcat,hasAccented,start,end,m_d->m_dicoData,m_d->m_isMainKeys,m_d->m_access,m_d->m_sp));
}

std::pair< DictionarySubWordIterator, DictionarySubWordIterator > EnhancedAnalysisDictionary::getSubWordEntries(const int offset, const LimaString& key) const
{
  std::pair<AccessSubWordIterator,AccessSubWordIterator> accessItrs=
    m_d->m_access->getSubWords(offset,key);
  return std::pair< DictionarySubWordIterator, DictionarySubWordIterator >(
           DictionarySubWordIterator(new EnhancedAnalysisDictionarySubWordIterator(accessItrs.first,*this)),
           DictionarySubWordIterator(new EnhancedAnalysisDictionarySubWordIterator(accessItrs.second,*this)));

}

std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > EnhancedAnalysisDictionary::getSuperWordEntries(const LimaString& key) const
{
  std::pair<AccessSuperWordIterator,AccessSuperWordIterator> accessItrs=
    m_d->m_access->getSuperWords(key);
  return std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator >(
           DictionarySuperWordIterator(new EnhancedAnalysisDictionarySuperWordIterator(accessItrs.first)),
           DictionarySuperWordIterator(new EnhancedAnalysisDictionarySuperWordIterator(accessItrs.second)));
}

} // namespace
} // namespace
} // namespace
