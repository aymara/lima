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
MONTHSDAYS_ORDINAL_ID=std::string("o");
const std::string NormalizeDateTimeResources::
MONTHSDAYS_SUFFIX_ID=std::string("s");


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
    tzDbFile = resourcesPath + "/" + tzDbFile;
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
    monthsDaysFile = resourcesPath + "/" + monthsDaysFile;
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

  ifstream file(monthsDaysFile.c_str(), std::ifstream::binary);
  if (!file.good()) {
    return false;
  }
  string utf8line;
  LimaString line;
  while (file.good()) {
    getline(file,utf8line);
    if (!utf8line.empty()) {
      line=Common::Misc::utf8stdstring2limastring(utf8line);
      std::vector<std::string> elements;
      split(elements,utf8line,is_any_of(MONTHSDAYS_MAIN_SEP));
      // three elements in line: (month|day|ordinal|suffix) num list,of,strings
      if (elements.size()!=3) {
        SELOGINIT;
        LWARN << "MonthsDaysResources: cannot parse line " << utf8line;
        continue;
      }
      map<LimaString,unsigned short>* names(0);
      if (elements[0] == MONTHSDAYS_MONTH_ID) { names=&m_months; }
      else if (elements[0] == MONTHSDAYS_DAY_ID) { names=&m_days; }
      else if (elements[0] == MONTHSDAYS_ORDINAL_ID) { names=&m_ordinal; }
      else if (elements[0] == MONTHSDAYS_SUFFIX_ID) { names=&m_ordinalSuffixes; }
      else {
        SELOGINIT;
        LWARN << "MonthsDaysResources: cannot parse line " << utf8line 
              << ": first element must be 'm' 'd', 'o' or 's'";
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
getDayNumberFromWordOrdinal(const LimaString& dayName) const
{
  map<LimaString,unsigned short>::const_iterator 
    it=m_ordinal.find(dayName);
  if (it==m_ordinal.end()) {
    return NormalizeDateTimeResources::no_day;
  }
  return (*it).second;
}

unsigned short NormalizeDateTimeResources::
getCardinalFromNumberOrdinal(const LimaString& dayName) const
{
  // try to extract number as int from string <number><ordinalSuffix>
  map<LimaString,unsigned short>::const_iterator it=m_ordinalSuffixes.begin();
  for( ; it!=m_ordinalSuffixes.end() ; it++ )
  {
    const LimaString& suffix = (*it).first;
    int index = dayName.indexOf(suffix, 0, Qt::CaseInsensitive);
    if (index < 0)
      continue;
    LimaString numberAsString(dayName.constData(),index);
    bool ok(false);
    unsigned short day = numberAsString.toUShort(&ok);
    if( ok) 
      return day;
  }
  return NormalizeDateTimeResources::no_day;
}


} // end namespace
} // end namespace
} // end namespace
