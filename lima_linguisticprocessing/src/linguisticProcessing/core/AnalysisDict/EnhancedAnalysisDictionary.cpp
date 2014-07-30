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
#include "EnhancedAnalysisDictionary.h"
#include "EnhancedAnalysisDictionaryIterator.h"
#include "AbstractAccessResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

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

SimpleFactory<AbstractResource,EnhancedAnalysisDictionary> enhancedAnalysisDictionaryFactory(ENHANCEDANALYSISDICTIONARY_CLASSID);


EnhancedAnalysisDictionary::EnhancedAnalysisDictionary()
    : AbstractAnalysisDictionary(),
    m_access(0),
    m_sp(0)
{}

EnhancedAnalysisDictionary::EnhancedAnalysisDictionary(
      FsaStringsPool* sp,
      Lima::Common::AbstractAccessByString* access,
      const std::string& dataFile) :
   m_access(access),
   m_sp(sp)
{
  loadDataFile(dataFile);
}


EnhancedAnalysisDictionary::~EnhancedAnalysisDictionary()
{
//   if (m_access) {
//     delete m_access;
//     m_access=0;
//   }
}

void EnhancedAnalysisDictionary::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  ANALYSISDICTLOGINIT;
  MediaId language=manager->getInitializationParameters().language;
  m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);
  try
  {
    string accessId=unitConfiguration.getParamsValueAtKey("accessKeys");
    const AbstractResource* res=LinguisticResources::single().getResource(language,accessId);
    const AbstractAccessResource* aar=static_cast<const AbstractAccessResource*>(res);
    m_isMainKeys=aar->isMainKeys();
    m_access=aar->getAccessByString();
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'accessKeys' in EnhancedAnalysisDictionary group for language " << (int) language;
    throw InvalidConfiguration();
  }
  try
  {
    string temp=Common::MediaticData::MediaticData::single().getResourcesPath() + "/" + unitConfiguration.getParamsValueAtKey("dictionaryValuesFile");
    m_dicoData.loadBinaryFile(temp);
  }
  catch (NoSuchList& )
  {
    LERROR << "no list 'dictionaryValues' in EnhancedAnalysisDictionary group for language " << (int) language;
    throw InvalidConfiguration();
  }


}

DictionaryEntry EnhancedAnalysisDictionary::getEntryData(const StringsPoolIndex wordId) const
{
//  ANALYSISDICTLOGINIT;
//  LDEBUG << "getEntry " << wordId;
  if (wordId >= m_dicoData.getSize())
  {
//    LDEBUG << "return empty : index out of range";
return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(static_cast<StringsPoolIndex>(0),false,true,false,false,false,0,0,&m_dicoData,m_isMainKeys,m_access,m_sp));
  }
  
  StringsPoolIndex strId=wordId;
  if (!m_isMainKeys) {
    strId = (*m_sp)[m_access->getSpelling(wordId)];
  }
 
  unsigned char* p=m_dicoData.getEntryAddr(wordId);
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
    return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(strId,final,true,false,false,false,p,p,&m_dicoData,m_isMainKeys,m_access,m_sp));
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
  return DictionaryEntry(new EnhancedAnalysisDictionaryEntry(strId,final,false,hasLing,hasConcat,hasAccented,start,end,&m_dicoData,m_isMainKeys,m_access,m_sp));
}

std::pair< DictionarySubWordIterator, DictionarySubWordIterator > EnhancedAnalysisDictionary::getSubWordEntries(const int offset, const LimaString& key) const
{
  std::pair<AccessSubWordIterator,AccessSubWordIterator> accessItrs=
    m_access->getSubWords(offset,key);
  return std::pair< DictionarySubWordIterator, DictionarySubWordIterator >(
           DictionarySubWordIterator(new EnhancedAnalysisDictionarySubWordIterator(accessItrs.first,*this)),
           DictionarySubWordIterator(new EnhancedAnalysisDictionarySubWordIterator(accessItrs.second,*this)));

}

std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > EnhancedAnalysisDictionary::getSuperWordEntries(const LimaString& key) const
{
  std::pair<AccessSuperWordIterator,AccessSuperWordIterator> accessItrs=
    m_access->getSuperWords(key);
  return std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator >(
           DictionarySuperWordIterator(new EnhancedAnalysisDictionarySuperWordIterator(accessItrs.first)),
           DictionarySuperWordIterator(new EnhancedAnalysisDictionarySuperWordIterator(accessItrs.second)));
}

}

}

}
