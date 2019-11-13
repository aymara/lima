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
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

SimpleFactory<AbstractResource,MultiLevelAnalysisDictionary>
multiLevelAnalysisDictionaryFactory(MULTILEVELANALYSISDICTIONARY_CLASSID);


struct LevelDico {
  LevelDico() : id(),keys(0),data(),mainKeys(false) {};
  std::string id;
  Lima::Common::AbstractAccessByString* keys;
  DictionaryData* data;
  bool mainKeys;
};

class MultiLevelAnalysisDictionaryPrivate
{
friend class MultiLevelAnalysisDictionary;
    MultiLevelAnalysisDictionaryPrivate() = default;

    ~MultiLevelAnalysisDictionaryPrivate() = default;

    MultiLevelAnalysisDictionaryPrivate(const MultiLevelAnalysisDictionaryPrivate&) = delete;
    MultiLevelAnalysisDictionaryPrivate& operator=(const MultiLevelAnalysisDictionaryPrivate&) = delete;

  /**
  * Les dicos sont ordonn�es du plus sp�cifique au plus g�n�rique.
  * L'ordre dans lequel ils sont d�clar�s dans le fichier de conf est l'ordre dans
  * lequel ils sont lus.
  */
  std::vector<LevelDico> m_dicos;
  Lima::FsaStringsPool* m_sp;
  uint64_t m_mainKeySize;

};


MultiLevelAnalysisDictionary::MultiLevelAnalysisDictionary() :
    AbstractAnalysisDictionary(),
    m_d(new MultiLevelAnalysisDictionaryPrivate())
{
}


MultiLevelAnalysisDictionary::~MultiLevelAnalysisDictionary()
{
  delete m_d;
}

void MultiLevelAnalysisDictionary::init(
    XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
{
  ANALYSISDICTLOGINIT;
  LINFO << "init MultiLevelAnalysisDictionary";
  auto language = manager->getInitializationParameters().language;
  m_d->m_sp = &Common::MediaticData::MediaticData::changeable().stringsPool(language);
  m_d->m_mainKeySize = 0;
  try
  {
    const auto& accesses = unitConfiguration.getListsValueAtKey("accessKeys");
    const auto& data = unitConfiguration.getListsValueAtKey("dictionaryValuesFiles");
    auto keyIt = accesses.cbegin(), dataIt = data.cbegin();
    auto hasMainKeys = false;
    for (; keyIt!=accesses.end() && dataIt!=data.end();)
    {
      LINFO << "load LevelDictionary : key=" << *keyIt << " data=" << *dataIt;
      auto res = LinguisticResources::single().getResource(language, *keyIt);
      auto aar = static_cast<const AbstractAccessResource*>(res);
      LevelDico ldico;
      ldico.id = *keyIt;
      ldico.keys = aar->getAccessByString();
      ldico.mainKeys = aar->isMainKeys();
      if (aar->isMainKeys())
      {
        hasMainKeys = true;
        m_d->m_mainKeySize = ldico.keys->getSize();
      }
      auto dataFile = findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
                                      (*dataIt).c_str());
      ldico.data = new DictionaryData();
      ldico.data->loadBinaryFile(dataFile.toStdString());
      m_d->m_dicos.push_back(ldico);
      keyIt++;
      dataIt++;
    }
    if ((keyIt!=accesses.end()) || (dataIt!=data.end()))
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "missing keys or data in configuration for MultiLevelAnalysisDictionary ! for language "
          << (int) language;
      LERROR << errorString;
      throw InvalidConfiguration(errorString.toStdString());
    }
    if (!hasMainKeys)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "no accessKeys are main keys (stringspool keys) in MultiLevelAnalysisDictionary  for language "
              << (int) language;
      LERROR << errorString;
      throw InvalidConfiguration(errorString.toStdString());
    }
  }
  catch (NoSuchList& )
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "no param 'accessKeys' in MultiLevelAnalysisDictionary group for language "
        << (int) language;
    LERROR << errorString;
    throw InvalidConfiguration(errorString.toStdString());
  }
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const Lima::LimaString& word) const
{
  StringsPoolIndex form(0);
  std::vector<uint64_t> indexes;
  for (const auto& dico: m_d->m_dicos)
  {
    indexes.push_back(dico.keys->getIndex(word));
    if (dico.mainKeys)
    {
      form=indexes.back();
    }
  }
  return getEntry(form, indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex wordId) const
{
  const auto& csp = *m_d->m_sp;
  LimaString word = csp[wordId];
  return getEntry(wordId, word);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex wordId,
                                                       const Lima::LimaString& word) const
{
  std::vector<uint64_t> indexes;
  for (const auto& dico : m_d->m_dicos)
  {
    if (dico.mainKeys)
    {
      if (wordId < m_d->m_mainKeySize)
      {
        indexes.push_back(wordId);
      }
      else
      {
        indexes.push_back(0);
      }
    }
    else
    {
      indexes.push_back(dico.keys->getIndex(word));
    }
  }
  return getEntry(wordId, indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const std::vector<uint64_t>& indexes) const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "MultiLevelAnalysisDictionary::getEntry" << indexes;
  auto indexItr = indexes.cbegin();
  for (auto dicoItr = m_d->m_dicos.cbegin();
       (indexItr != indexes.cend()) && (dicoItr != m_d->m_dicos.cend());
       indexItr++, dicoItr++)
  {
    if (dicoItr->mainKeys)
    {
      break;
    }
  }
  Q_ASSERT(indexItr!=indexes.end());
  if (*indexItr != 0)
  {
    return getEntry(static_cast<StringsPoolIndex>(*indexItr), indexes);
  }
  // entry doesn't exist in main key : search in other key
  indexItr = indexes.cbegin();
  StringsPoolIndex form(0);
  for (auto dicoItr = m_d->m_dicos.cbegin();
       (indexItr != indexes.end()) && (dicoItr != m_d->m_dicos.end());
       indexItr++, dicoItr++)
  {
    if (*indexItr > 0)
    {
      form = (*m_d->m_sp)[dicoItr->keys->getSpelling(*indexItr)];
      break;
    }
  }
  return getEntry(form,indexes);
}

DictionaryEntry MultiLevelAnalysisDictionary::getEntry(const StringsPoolIndex form,
                                                       const std::vector<uint64_t>& indexes) const
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "getEntry";

  std::vector<MultiLevelAnalysisDictionaryEntry::LevelData> entryData;

  auto empty = true;
  auto final = false;
  auto hasLing = false;
  auto hasAccented = false;
  auto hasConcat = false;

  auto indexIt = indexes.cbegin();
  auto dicoIt = m_d->m_dicos.cbegin();
  MultiLevelAnalysisDictionaryEntry::LevelData data;
  for (;indexIt != indexes.end() && dicoIt != m_d->m_dicos.end() && !final;
       indexIt++, dicoIt++)
  {
    LDEBUG << "read level " << dicoIt->id << " : entry index = " << *indexIt;
    auto p = dicoIt->data->getEntryAddr(*indexIt);
    auto read = DictionaryData::readCodedInt(p);
    LDEBUG << "read " << read;
    if (read == 1)
    {
      LDEBUG << "final = true";
      final = true;
      read = DictionaryData::readCodedInt(p);
    }
    if (read == 0)
    {
      // this entry is empty, skip it
      LDEBUG << "entry is empty";
      continue;
    }
    // if here, then entry is not empty
    empty = false;
    LDEBUG << "entry has length " << read;
    data.startEntryData = p;
    data.endEntryData = p + read;
    read = DictionaryData::readCodedInt(p);
    if (read!=0)
    {
      hasLing=true;
    }
    LDEBUG << "info length = " << read;
    p += read;
    if (p != data.endEntryData)
    {
      read = DictionaryData::readCodedInt(p);
      LDEBUG << "accented length = " << read;
      if (read != 0)
      {
        hasAccented = true;
      }
      p += read;
      if (p != data.endEntryData)
      {
        read = DictionaryData::readCodedInt(p);
        LDEBUG << "concat length = " << read;
        if (read != 0)
        {
          hasConcat = true;
        }
        p += read;
      }
    }
    Q_ASSERT(p == data.endEntryData);

    data.dicoData = dicoIt->data;
    data.keys = dicoIt->keys;
    data.mainKeys = dicoIt->mainKeys;
    entryData.push_back(data);
  }
  return DictionaryEntry(new MultiLevelAnalysisDictionaryEntry(form,
                                                               final,
                                                               empty,
                                                               hasLing,
                                                               hasConcat,
                                                               hasAccented,
                                                               entryData,
                                                               m_d->m_sp));
}

std::pair< DictionarySubWordIterator, DictionarySubWordIterator > MultiLevelAnalysisDictionary::getSubWordEntries(
    const int offset, const LimaString& key) const
{
  // build iterator
  std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> > accessItrs;
  for (const auto& dico : m_d->m_dicos)
  {
    accessItrs.push_back(dico.keys->getSubWords(offset, key));
  }

  // build end iterator
  std::vector<std::pair<Common::AccessSubWordIterator,Common::AccessSubWordIterator> > accessItrsEnd(accessItrs);
  for (auto& accessItr : accessItrsEnd)
  {
    accessItr.first = accessItr.second;
  }

  return std::pair< DictionarySubWordIterator, DictionarySubWordIterator >(
           DictionarySubWordIterator(new MultiLevelAnalysisDictionarySubWordIterator(accessItrs,*this)),
           DictionarySubWordIterator(new MultiLevelAnalysisDictionarySubWordIterator(accessItrsEnd,*this)));
}

std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > MultiLevelAnalysisDictionary::getSuperWordEntries(
    const LimaString& key) const
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

uint64_t MultiLevelAnalysisDictionary::getDictionaryCount() const
{
  return m_d->m_dicos.size();
}

}

}

}
