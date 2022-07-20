// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define NORMALIZEDATETIMERESOURCES_CLASSID "NormalizeDateTimeResources"

class LIMA_SPECIFICENTITIES_EXPORT NormalizeDateTimeResources : public AbstractResource
{
  Q_OBJECT
 public:
  NormalizeDateTimeResources(); 
  ~NormalizeDateTimeResources();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  bool hasTimezoneDatabase() const { return false;/*(m_timezoneDatabase!=0);*/ }

//   const boost::local_time::tz_database& getTimezoneDatabase() const;
  unsigned short getMonthNumber(const LimaString& monthName) const;
  unsigned short getDayNumber(const LimaString& dayName) const;
  unsigned short getValueFromWordCardinalOrOrdinal(const LimaString& dayName) const;
  unsigned short getValueFromNumberOrdinal(const LimaString& dayName) const;

  static const unsigned short no_month=static_cast<unsigned short>(-1);
  static const unsigned short no_day=static_cast<unsigned short>(-1);

 private:
  MediaId m_language;
//   boost::local_time::tz_database* m_timezoneDatabase;
  std::map<LimaString,unsigned short> m_months;
  std::map<LimaString,unsigned short> m_days;
  std::map<LimaString,unsigned short> m_wordCardinal;
  std::map<LimaString,unsigned short> m_wordCardinalSeparator;
  std::map<LimaString,unsigned short> m_wordOrdinalSuffixes;
  std::map<LimaString,unsigned short> m_numberOrdinalSuffixes;

  // private member functions
  bool readMonthDays(const std::string& monthsDaysFile);

  // separator and keywords for monthsdays file
  static const std::string MONTHSDAYS_MAIN_SEP;
  static const std::string MONTHSDAYS_NAMELIST_SEP; 
  static const std::string MONTHSDAYS_MONTH_ID; 
  static const std::string MONTHSDAYS_DAY_ID; 
  static const std::string WORD_CARDINAL_ID;
  static const std::string WORD_CARDINAL_SEPARATOR_ID;
  static const std::string WORD_ORDINAL_SUFFIX_ID;
  static const std::string NUMBER_ORDINAL_SUFFIX_ID;

};

} // end namespace
} // end namespace
} // end namespace

#endif
