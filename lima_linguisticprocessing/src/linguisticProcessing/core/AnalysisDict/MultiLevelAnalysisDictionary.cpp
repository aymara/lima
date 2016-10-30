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
#include "MultiLevelAnalysisDictionary.h"
#include "MultiLevelAnalysisDictionaryEntry.h"
#include "MultiLevelAnalysisDictionaryIterators.h"

#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"

#include <iostream>

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

SimpleFactory<AbstractResource,MultiLevelAnalysisDictionary> multiLevelAnalysisDictionaryFactory(MULTILEVELANALYSISDICTIONARY_CLASSID);

MultiLevelAnalysisDictionary::MultiLevelAnalysisDictionary()
    : AbstractAnalysisDictionary()
{}


MultiLevelAnalysisDictionary::~MultiLevelAnalysisDictionary()
{}

void MultiLevelAnalysisDictionary::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  ANALYSISDICTLOGINIT;
  LINFO << "init MultiLevelAnalysisDictionary";
  MediaId language=manager->getInitializationParameters().language;
  m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);
  m_mainKeySize=0;
  try
  {
    const deque<string>& accesses=unitConfiguration.getListsValueAtKey("accessKeys");
    const deque<string>& data=unitConfiguration.getListsValueAtKey("dictionaryValuesFiles");
    deque<string>::const_iterator keyIt=accesses.begin(),dataIt=data.begin();
    bool hasMainKeys=false;
    for (;keyIt!=accesses.end() && dataIt!=data.end();)
    {
      LINFO << "load LevelDictionary : key=" << *keyIt << " data=" << *dataIt;
      const AbstractResource* res=LinguisticResources::single().getResource(language,*keyIt);
      const AbstractAccessResource* aar=static_cast<const AbstractAccessResource*>(res);
      LevelDico ldico;
      ldico.id=*keyIt;
      ldico.keys=aar->getAccessByString();
      ldico.mainKeys=aar->isMainKeys();
      if (aar->isMainKeys())
      {
        hasMainKeys=true;
        m_mainKeySize=ldico.keys->getSize();
      }
      QString dataFile = Common::Misc::findFileInPaths( Common::MediaticData::MediaticData::single().getResourcesPath().c_str(), (*dataIt).c_str());
      ldico.data=new DictionaryData();
      ldico.data->loadBinaryFile(dataFile.toUtf8().constData());
      m_dicos.push_back(ldico);
      keyIt++;
      dataIt++;
    }
    if ((keyIt!=accesses.end()) || (dataIt!=data.end()))
    {
      LERROR << "missing keys or data in configuration for MultiLevelAnalysisDictionary ! for language " << (int) language;
      throw InvalidConfiguration();
    }
    if (!hasMainKeys)
    {
      LERROR << "no accessKeys are main keys (stringspool keys) in MultiLevelAnalysisDictionary  for language " << (int) language;
      throw InvalidConfiguration();
    }
  }
  catch (NoSuchList& )
  {
    LERROR << "no param 'accessKeys' in MultiLevelAnalysisDictionary group for language " << (int) language;
    throw InvalidConfiguration();
  }


}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const Lima::LimaString& word) const
{
  StringsPoolIndex form(0);
  vector<uint64_t> indexes;
  for (vector<LevelDico>::const_iterator it=m_dicos.begin();
       it!=m_dicos.end();
       it++)
  {
    indexes.push_back(it->keys->getIndex(word));
    if (it->mainKeys)
    {
      form=indexes.back();
    }
  }
  return getEntry(form,indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex wordId) const
{
  const FsaStringsPool& csp=*m_sp;
  LimaString word=csp[wordId];
  return getEntry(wordId,word);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex wordId, const Lima::LimaString& word) const
{
  vector<uint64_t> indexes;
  for (vector<LevelDico>::const_iterator it=m_dicos.begin();
       it!=m_dicos.end();
       it++)
  {
    if (it->mainKeys)
    {
      if (wordId<m_mainKeySize) {
        indexes.push_back(wordId);
      } else {
        indexes.push_back(0);
      }
    }
    else
    {
      indexes.push_back(it->keys->getIndex(word));
    }
  }
  return getEntry(wordId,indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const std::vector<uint64_t>& indexes) const
{
  vector<uint64_t>::const_iterator indexItr=indexes.begin();
  for (std::vector<LevelDico>::const_iterator dicoItr=m_dicos.begin();
       (indexItr != indexes.end()) && (dicoItr != m_dicos.end());
       indexItr++,dicoItr++)
  {
    if (dicoItr->mainKeys)
    {
      break;
    }
  }
  Q_ASSERT(indexItr!=indexes.end());
  if (*indexItr != 0) {
    return getEntry(static_cast<StringsPoolIndex>(*indexItr),indexes);
  }
  // entry doesn't exist in main key : search in other key
  indexItr=indexes.begin();
  StringsPoolIndex form(0);
  for (std::vector<LevelDico>::const_iterator dicoItr=m_dicos.begin();
       (indexItr != indexes.end()) && (dicoItr != m_dicos.end());
       indexItr++,dicoItr++)
  {
    if (*indexItr > 0) {
      form=(*m_sp)[dicoItr->keys->getSpelling(*indexItr)];
      break;
    }
  }
  return getEntry(form,indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex form,const std::vector<uint64_t>& indexes) const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "getEntry";

  vector<MultiLevelAnalysisDictionaryEntry::LevelData> entryData;

  bool empty=true;
  bool final=false;
  bool hasLing=false;
  bool hasAccented=false;
  bool hasConcat=false;

  vector<uint64_t>::const_iterator indexIt=indexes.begin();
  vector<LevelDico>::const_iterator dicoIt=m_dicos.begin();
  MultiLevelAnalysisDictionaryEntry::LevelData data;
  for (;indexIt!=indexes.end() && dicoIt!=m_dicos.end() && !final;indexIt++,dicoIt++)
  {
    LDEBUG << "read level " << dicoIt->id << " : entry index = " << *indexIt;
    unsigned char* p=dicoIt->data->getEntryAddr(*indexIt);
    uint64_t read=DictionaryData::readCodedInt(p);
    LDEBUG << "read " << read;
    if (read == 1)
    {
      LDEBUG << "final = true";
      final=true;
      read=DictionaryData::readCodedInt(p);
    }
    if (read == 0)
    {
      // this entry is empty, skip it
      LDEBUG << "entry is empty";
      continue;
    }
    // if here, then entry is not empty
    empty=false;
    LDEBUG << "entry has length " << read;
    data.startEntryData=p;
    data.endEntryData=p+read;
    read=DictionaryData::readCodedInt(p);
    if (read!=0)
    {
      hasLing=true;
    }
    LDEBUG << "info length = " << read;
    p+=read;
    if (p!=data.endEntryData)
    {
      read=DictionaryData::readCodedInt(p);
      LDEBUG << "accented length = " << read;
      if (read!=0)
      {
        hasAccented=true;
      }
      p+=read;
      if (p!=data.endEntryData)
      {
        read=DictionaryData::readCodedInt(p);
        LDEBUG << "concat length = " << read;
        if (read != 0)
        {
          hasConcat=true;
        }
        p+=read;
      }
    }
    Q_ASSERT(p==data.endEntryData);

    data.dicoData=dicoIt->data;
    data.keys=dicoIt->keys;
    data.mainKeys=dicoIt->mainKeys;
    entryData.push_back(data);
  }
  return DictionaryEntry(new MultiLevelAnalysisDictionaryEntry(form,final,empty,hasLing,hasConcat,hasAccented,entryData,m_sp));
}

std::pair< DictionarySubWordIterator, DictionarySubWordIterator > MultiLevelAnalysisDictionary::getSubWordEntries(const int offset, const LimaString& key) const
{

  // build iterator
  std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> > accessItrs;
  for (std::vector<LevelDico>::const_iterator dicoItr=m_dicos.begin();
       dicoItr!=m_dicos.end();
       dicoItr++)
  {
    accessItrs.push_back(dicoItr->keys->getSubWords(offset,key));
  }

  // build end iterator
  std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> > accessItrsEnd(accessItrs);
  for (std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> >::iterator it=accessItrsEnd.begin();
       it!=accessItrsEnd.end();
       it++)
  {
    it->first=it->second;
  }

  return std::pair< DictionarySubWordIterator, DictionarySubWordIterator >(
           DictionarySubWordIterator(new MultiLevelAnalysisDictionarySubWordIterator(accessItrs,*this)),
           DictionarySubWordIterator(new MultiLevelAnalysisDictionarySubWordIterator(accessItrsEnd,*this)));
}

std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > MultiLevelAnalysisDictionary::getSuperWordEntries(const LimaString& key) const
{
  LIMA_UNUSED(key);
  // TODO unimplemented method
  std::cerr << "unimplemented method at "<<__FILE__<<", line "<<__LINE__<< std::endl;
  Q_ASSERT(false);
  return std::make_pair( DictionarySuperWordIterator(0), DictionarySuperWordIterator(0));
}

uint64_t MultiLevelAnalysisDictionary::getSize() const
{
  // TODO unimplemented method
  std::cerr << "unimplemented method at "<<__FILE__<<", line "<<__LINE__<< std::endl;
  Q_ASSERT(false);
  return 0;
}

}

}

}
