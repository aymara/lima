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
/************************************************************************
 *
 * @file       NormalizeDateTimeResources.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Wed Jun 14 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "NormalizeDateTimeResources.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

#include <fstream>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace boost::algorithm;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

SimpleFactory<AbstractResource,NormalizeDateTimeResources> 
NormalizeDateTimeResourcesFactory(NORMALIZEDATETIMERESOURCES_CLASSID);

// initialization of static const members
const std::string NormalizeDateTimeResources::
MONTHSDAYS_MAIN_SEP=std::string(" ");
const std::string NormalizeDateTimeResources::
MONTHSDAYS_NAMELIST_SEP=std::string(",");
const std::string NormalizeDateTimeResources::
MONTHSDAYS_MONTH_ID=std::string("m");
const std::string NormalizeDateTimeResources::
MONTHSDAYS_DAY_ID=std::string("d");
const std::string NormalizeDateTimeResources::
WORD_CARDINAL_ID=std::string("c");
const std::string NormalizeDateTimeResources::
WORD_CARDINAL_SEPARATOR_ID=std::string("s");
const std::string NormalizeDateTimeResources::
WORD_ORDINAL_SUFFIX_ID=std::string("w");
const std::string NormalizeDateTimeResources::
NUMBER_ORDINAL_SUFFIX_ID=std::string("n");


NormalizeDateTimeResources::NormalizeDateTimeResources():
m_language(0),
// m_timezoneDatabase(0),
m_months(),
m_days()
{
}

NormalizeDateTimeResources::~NormalizeDateTimeResources() {
//   if (m_timezoneDatabase!=0) {
//     delete m_timezoneDatabase;
//     m_timezoneDatabase=0;
//   }
}

void NormalizeDateTimeResources::
init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
   
{
  m_language=manager->getInitializationParameters().language;
  string resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();

  // read timezone database file
  string tzDbFile("");
  try
  {
    tzDbFile = unitConfiguration.getParamsValueAtKey("timezoneDatabase");
    tzDbFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), tzDbFile.c_str()).toUtf8().constData();
//     m_timezoneDatabase = new boost::local_time::tz_database();
//     m_timezoneDatabase->load_from_file(tzDbFile);
  }
  catch (NoSuchParam& ) {
    SELOGINIT;
    LERROR << "No param 'timezoneDatabase' in NormalizeDateTimeResources group for language " 
           << (int)m_language;
    throw InvalidConfiguration();
  }
  catch (exception& e) {
    SELOGINIT;
    LERROR << "Error loading timezoneDatabase '"
           << tzDbFile << "':" << e.what();
    // reset database to 0
//     delete m_timezoneDatabase;
//     m_timezoneDatabase=0;
  }
  
  // read resources for months and days
  try
  {
    string monthsDaysFile = unitConfiguration.getParamsValueAtKey("monthsDays");
    monthsDaysFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), monthsDaysFile.c_str()).toUtf8().constData();
    if (!readMonthDays(monthsDaysFile)) {
      SELOGINIT;
      LERROR << "Error loading monthsDays resources '" 
             << monthsDaysFile << "'";
    }
  }
  catch (NoSuchParam& ) {
    SELOGINIT;
    LERROR << "No param 'monthsDays' in NormalizeDateTimeResources group for language " 
           << (int)m_language;
    throw InvalidConfiguration();
  }
 
}

bool NormalizeDateTimeResources::
readMonthDays(const std::string& monthsDaysFile) 
{

  m_wordCardinalSeparator[Common::Misc::utf8stdstring2limastring(" ")]=0;
  ifstream file(monthsDaysFile.c_str(), std::ifstream::binary);
  if (!file.good()) {
    return false;
  }
  string utf8line;
  LimaString line;
  while (file.good()) {
    utf8line = Lima::Common::Misc::readLine(file);
    if (!utf8line.empty()) {
      line=Common::Misc::utf8stdstring2limastring(utf8line);
      std::vector<std::string> elements;
      split(elements,utf8line,is_any_of(MONTHSDAYS_MAIN_SEP));
      // three elements in line: (month|day|ordinal|cardinal|suffix) num list-of-strings
      if (elements.size()!=3) { 
        SELOGINIT;
        LWARN << "MonthsDaysResources: cannot parse line " << utf8line;
        continue;
      }
      map<LimaString,unsigned short>* names(0);
      if (elements[0] == MONTHSDAYS_MONTH_ID) { names=&m_months; }
      else if (elements[0] == MONTHSDAYS_DAY_ID) { names=&m_days; }
      else if (elements[0] == WORD_CARDINAL_SEPARATOR_ID) { names=&m_wordCardinalSeparator; }
      else if (elements[0] == WORD_CARDINAL_ID) { names=&m_wordCardinal; }
      else if (elements[0] == WORD_ORDINAL_SUFFIX_ID) { names=&m_wordOrdinalSuffixes; }
      else if (elements[0] == NUMBER_ORDINAL_SUFFIX_ID) { names=&m_numberOrdinalSuffixes; }
      else {
        SELOGINIT;
        LWARN << "MonthsDaysResources: cannot parse line " << utf8line 
              << ": first element must be 'm' 'd', 'c', 'w', 'n' or 's'";
        continue;
      }

      uint64_t n=atoi(elements[1].c_str());
      vector<std::string> namesToAdd;
      split(namesToAdd,elements[2],is_any_of(MONTHSDAYS_NAMELIST_SEP));
      for (vector<std::string>::const_iterator it=namesToAdd.begin(),
             it_end=namesToAdd.end();it!=it_end; it++) {
        (*names)[Common::Misc::utf8stdstring2limastring(*it)]=n;
      }
    }
  }
  return true;
}

// const boost::local_time::tz_database& NormalizeDateTimeResources::
// getTimezoneDatabase() const
// {
//   if (m_timezoneDatabase==0) {
//     SELOGINIT;
//     LERROR << "No timezone available";
//     throw(LinguisticProcessingException());
//   }
//   return *m_timezoneDatabase;
// }

unsigned short NormalizeDateTimeResources::
getMonthNumber(const LimaString& monthName) const
{
  map<LimaString,unsigned short>::const_iterator 
    it=m_months.find(monthName);
  if (it==m_months.end()) {
    return NormalizeDateTimeResources::no_month;
  }
  return (*it).second;
}

unsigned short NormalizeDateTimeResources::
getDayNumber(const LimaString& dayName) const
{
  map<LimaString,unsigned short>::const_iterator 
    it=m_days.find(dayName);
  if (it==m_days.end()) {
    return NormalizeDateTimeResources::no_day;
  }
  return (*it).second;
}

unsigned short NormalizeDateTimeResources::
getValueFromWordCardinalOrOrdinal(const LimaString& dayName) const
{
  SELOGINIT;
  unsigned short day(0);
  // trim suffix first, second or th, or (ème, ième, ieme, eme)
  LimaString numberAsString(dayName);
  LDEBUG << "NormalizeDateTimeResources::getValueFromWordCardinalOrOrdinal() numberAsString=" 
         << numberAsString;
  map<LimaString,unsigned short>::const_iterator suffixIt=m_wordOrdinalSuffixes.begin();
  for( ; suffixIt!=m_wordOrdinalSuffixes.end() ; suffixIt++ )
  {
    const LimaString& suffix = (*suffixIt).first;
    int index = dayName.indexOf(suffix, 0, Qt::CaseInsensitive);
    if (index >= 0) {
      numberAsString = LimaString(dayName.constData(),index);
      day += (*suffixIt).second;
      break;
    }
  }
  LDEBUG << "NormalizeDateTimeResources::getValueFromWordCardinalOrOrdinal: after trim numberAsString=" 
         << numberAsString << ", day=" << day;
  if( numberAsString.isEmpty() )
    return day;
  // compute value from left to right
  int parsingPosition(0);
  LDEBUG << "NormalizeDateTimeResources::getValueFromWordCardinalOrOrdinal: parsingPosition=" << parsingPosition;
  for( ; ; )
  {
    int index(-1);
    // identify component of number
    map<LimaString,unsigned short>::const_iterator cardinalIt=m_wordCardinal.begin();
    for( ; cardinalIt!=m_wordCardinal.end() ; cardinalIt++ )
    {
      const LimaString& word = (*cardinalIt).first;
      int index = numberAsString.indexOf(word, parsingPosition, Qt::CaseInsensitive);
      if (index >= 0) {
	day += (*cardinalIt).second;
	parsingPosition += word.length();
	LDEBUG << "NormalizeDateTimeResources::getValueFromWordCardinalOrOrdinal: found" 
               << word << ", day=" << day << ", parsingPosition=" << parsingPosition;
	break;
      }
    }
    // skip separator
    int skipIndex(-1);
    do
    {
      map<LimaString,unsigned short>::const_iterator separatorIt=m_wordCardinalSeparator.begin();
      for( ; separatorIt!=m_wordCardinalSeparator.end() ; separatorIt++ )
      {
	const LimaString& separator = (*separatorIt).first;
	int skipIndex = numberAsString.indexOf(separator, parsingPosition, Qt::CaseInsensitive);
	if (skipIndex == 0) {
	  parsingPosition += separator.length();
	  LDEBUG << "NormalizeDateTimeResources::getValueFromWordCardinalOrOrdinal: found" 
                 << separator << ", day=" << day << ", parsingPosition=" << parsingPosition;
	  break;
	}
      }
    } while( skipIndex == 0 );
    if( index == -1 )
      break;
  }
  return day;
}

unsigned short NormalizeDateTimeResources::
getValueFromNumberOrdinal(const LimaString& dayName) const
{
  // try to extract number as int from string <number><ordinalSuffix> like 4th, 22nd, 1st or 17
  map<LimaString,unsigned short>::const_iterator it=m_numberOrdinalSuffixes.begin();
  for( ; it!=m_numberOrdinalSuffixes.end() ; it++ )
  {
    // try to trim suffix th, nd, st or rd
    const LimaString& suffix = (*it).first;
    int index = dayName.indexOf(suffix, 0, Qt::CaseInsensitive);
    LimaString numberAsString(dayName);
    if (index > 0)
      numberAsString = LimaString(dayName.constData(),index);
    bool ok(false);
    // try to convert trimmed string to int
    unsigned short day = numberAsString.toUShort(&ok);
    if( ok) 
      return day;
  }
  return NormalizeDateTimeResources::no_day;
}


} // end namespace
} // end namespace
} // end namespace
