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
 * @file       NormalizeDateTimeResources.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Wed Jun 14 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief      resources for the normalization of dates and times (timezones, names of months and days, ...
 * 
 * 
 ***********************************************************************/

#ifndef NORMALIZEDATETIMERESOURCES_H
#define NORMALIZEDATETIMERESOURCES_H

#include "SpecificEntitiesExport.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/misc/local_timenowarn.hpp"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define NORMALIZEDATETIMERESOURCES_CLASSID "NormalizeDateTimeResources"

class LIMA_SPECIFICENTITIES_EXPORT NormalizeDateTimeResources : public AbstractResource
{
 public:
  NormalizeDateTimeResources(); 
  ~NormalizeDateTimeResources();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  bool hasTimezoneDatabase() const { return false;/*(m_timezoneDatabase!=0);*/ }

//   const boost::local_time::tz_database& getTimezoneDatabase() const;
  unsigned short getMonthNumber(const LimaString& monthName) const;
  unsigned short getDayNumber(const LimaString& dayName) const;

  static const unsigned short no_month=static_cast<unsigned short>(-1);
  static const unsigned short no_day=static_cast<unsigned short>(-1);

 private:
  MediaId m_language;
//   boost::local_time::tz_database* m_timezoneDatabase;
  std::map<LimaString,unsigned short> m_months;
  std::map<LimaString,unsigned short> m_days;

  // private member functions
  bool readMonthDays(const std::string& monthsDaysFile);

  // separator and keywords for monthsdays file
  static const std::string MONTHSDAYS_MAIN_SEP;
  static const std::string MONTHSDAYS_NAMELIST_SEP; 
  static const std::string MONTHSDAYS_MONTH_ID; 
  static const std::string MONTHSDAYS_DAY_ID; 

};

} // end namespace
} // end namespace
} // end namespace

#endif
