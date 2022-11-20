// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       NormalizeDateTime.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2019 by CEA LIST
 *
 ***********************************************************************/

#include "NormalizeDateTime.h"
#include "NormalizationUtils.h"
#include "MicrosForNormalization.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <QtCore/QDate>

#include <limits>
#ifdef WIN32
#undef min
#undef max
#endif

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;

// using namespace boost;
// using namespace boost::local_time;
// using namespace boost::posix_time;


using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define DATESTRING_FEATURE_NAME "date_string" // date as a string
#define DATE_FEATURE_NAME "date" // date as a QDate
#define DATE_BEGIN_FEATURE_NAME "date_begin"
#define DATE_END_FEATURE_NAME "date_end"
#define DATE_SPAN_FEATURE_NAME "date_span"
#define DAY_FEATURE_NAME "day"
#define DAYEND_FEATURE_NAME "dayend"
#define MONTH_FEATURE_NAME "month"
#define MONTHEND_FEATURE_NAME "monthend"
#define YEAR_FEATURE_NAME "year"
#define NUMDAY_FEATURE_NAME "numday"
#define NUMDAYEND_FEATURE_NAME "numdayend"
#define NUMMONTH_FEATURE_NAME "nummonth"
#define NUMMONTHEND_FEATURE_NAME "nummonthend"
#define NUMDAYMONTH_FEATURE_NAME "numdaymonth" // when 10/04 is a single token as fraction
#define NUMYEAR_FEATURE_NAME "numyear"
#define NUMYEAREND_FEATURE_NAME "numyearend"

#define LOCALTIME_FEATURE_NAME "local_time"
#define UTCTIME_FEATURE_NAME "utc_time"
#define TIMESTRING_FEATURE_NAME "time_string"

//**********************************************************************
// factories for actions defined in this file
Automaton::ConstraintFunctionFactory<NormalizeDate>
NormalizeDateFactory(NormalizeDateId);

Automaton::ConstraintFunctionFactory<NormalizeRelativeDate>
NormalizeRelativeDateFactory(NormalizeRelativeDateId);

Automaton::ConstraintFunctionFactory<NormalizeLocalTime>
NormalizeLocalTimeFactory(NormalizeLocalTimeId);

Automaton::ConstraintFunctionFactory<NormalizeUTCTime>
NormalizeUTCTimeFactory(NormalizeUTCTimeId);


ReferenceData::ReferenceData():
m_dateRefName("document"),
m_timeRefName("document"),
m_locRefName("document")
{}

ReferenceData::~ReferenceData() {}

void ReferenceData::parseComplement(const LimaString& complement)
{
  if (! complement.isEmpty()) {
    //uint64_t i(0),prev(0); portage 32 64
    int i(0),prev(0);
    do {
      i=complement.indexOf(LimaChar(','),prev);
      parseReference(complement.mid(prev,i-prev));
      prev=i+1;
    } while (i!=-1);
  }
}

void ReferenceData::parseReference(const LimaString& str)
{
  static const LimaString datePrefix=Common::Misc::utf8stdstring2limastring("date_");
  static const LimaString locPrefix=Common::Misc::utf8stdstring2limastring("loc_");
  static const LimaString timePrefix=Common::Misc::utf8stdstring2limastring("time_");

  if (str.indexOf(datePrefix)==0) {
    m_dateRefName=Common::Misc::limastring2utf8stdstring(str.mid(5));
  }
  else if (str.indexOf(locPrefix)==0) {
    m_locRefName=Common::Misc::limastring2utf8stdstring(str.mid(4));
  }
  else if (str.indexOf(timePrefix)==0) {
    m_timeRefName=Common::Misc::limastring2utf8stdstring(str.mid(5));
  }
}

bool ReferenceData::
getReferenceDate(const AnalysisContent& analysis,
                 QDate& date) const
{
  // get posix time from current text metadata
  auto data = analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  auto metadata = std::dynamic_pointer_cast<const LinguisticMetaData>(data);

  date=metadata->getDate(m_dateRefName);
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "m_dateRefName =" << m_dateRefName <<" , date = " << date.toString();
#endif
  if (!date.isValid()) {
    //try backoff on document date
    SELOGINIT;
    LWARN << "no reference date '"<< m_dateRefName << "'";

    date=metadata->getDate("document");
    if (!date.isValid()) {
      SELOGINIT;
      LWARN << "no reference date 'document'";
      return false;
    }
    return true;
  }
  return true;
}

bool ReferenceData::
getReferenceTime(const AnalysisContent& analysis,
                 QTime& time) const
{
  // get posix time from current text metadata
  auto data = analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  auto metadata = std::dynamic_pointer_cast<const LinguisticMetaData>(data);

  time=metadata->getTime(m_timeRefName);
  return (time.isValid());
}

bool ReferenceData::
getReferenceLocation(const AnalysisContent& analysis,
                     std::string& location) const
{
  // get posix time from current text metadata
  auto data = analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  auto metadata = std::dynamic_pointer_cast<const LinguisticMetaData>(data);

  location=metadata->getLocation(m_locRefName);
  return (! location.empty());
}

//**********************************************************************
void updateNormalizedForm(RecognizerMatch& m) 
{
  // change features so that the DEFAULT_ATTRIBUTE contains the true normalization
  // 
  if (m.features().find(DATE_FEATURE_NAME) != m.features().end()) {
    m.features().setFeature(DATESTRING_FEATURE_NAME,m.features().find(DEFAULT_ATTRIBUTE)->getValueLimaString());
    m.features().setFeature(DEFAULT_ATTRIBUTE,m.features().find(DATE_FEATURE_NAME)->getValueLimaString());
  }
  // set value only for exact dates: otherwise, the default normalization of years (2014 -> [2014-01-01,2014-12-31])
  // is a bit too much
//   else if (m.features().find(DATE_BEGIN_FEATURE_NAME) != m.features().end()) {
//     m.features().setFeature(DATESTRING_FEATURE_NAME,m.features().find(DEFAULT_ATTRIBUTE)->getValueLimaString());
//     LimaString norm="["
//       +m.features().find(DATE_BEGIN_FEATURE_NAME)->getValueLimaString()+","
//       +m.features().find(DATE_END_FEATURE_NAME)->getValueLimaString()+"]";
//     m.features().setFeature(DEFAULT_ATTRIBUTE,norm);
//   }
}


//**********************************************************************
NormalizeDate::
NormalizeDate(MediaId language,
              const LimaString& complement):
Automaton::ConstraintFunction(language,complement),
m_language(language),
m_resources(0),
m_referenceData(),
m_microsForMonth(0),
m_microsForDays(0),
m_microAccessor(0),
m_isInterval(false)
{
  m_referenceData.parseComplement(complement);

  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  if (language != UNDEFLANG) {
    try {
      AbstractResource* res = LinguisticResources::single().getResource(language,"microsForDateTimeNormalization");
      MicrosForNormalization* micros=static_cast<MicrosForNormalization*>(res);
      m_microsForMonth=micros->getMicros("MonthMicros");
      m_microsForDays=micros->getMicros("DayMicros");
    }
    catch (std::exception& e) {
      SELOGINIT;
      LWARN << "Exception caught: " << e.what();
      LWARN << "-> micros for person name normalization are not initialized";
    }
  }

  try {
    AbstractResource* res=LinguisticResources::changeable().
      getResource(language,"DateTimeNormalization");
    if (res==0) {
      SELOGINIT;
      LWARN << "no resource 'DateTimeNormalization'";
    }
    else {
      m_resources=static_cast<const NormalizeDateTimeResources*>(res);
    }
  }
  catch (std::exception& e) {
    // resources are not loaded when compiling the rules
      SELOGINIT;
      LWARN << "Exception caught: " << e.what();
  }

  static const LimaString intervalString=Common::Misc::utf8stdstring2limastring("isInterval");
  if (complement.indexOf(intervalString)!=-1) {
    m_isInterval=true;
  }
}

void NormalizeDate::
updateCurrentDate(AnalysisContent& analysis,
                  const QDate& currentDate) const
{
  // get metadata to update
  auto data = analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return;
  }
  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(data);

  metadata->setDate("current", currentDate);
}

unsigned short NormalizeDate::getDayFromString(const LimaString& numdayString) const
{
#ifdef DEBUG_LP
  SELOGINIT;
#endif
  // try to extract number as int from string <number><ordinalSuffix> like 4th, 22nd, 1st or <number> like 17
  unsigned short day =  m_resources->getValueFromNumberOrdinal(numdayString);
#ifdef DEBUG_LP
  LDEBUG << "NormalizeDate::getDayFromString: testConversion 1 of " << numdayString << "1 day=" << day;
#endif
  // try first conversion of type "premier" -> 1
  // then try conversion of type "10th" -> 10
  if( day == NormalizeDateTimeResources::no_day ) {
    day =  m_resources->getValueFromWordCardinalOrOrdinal(numdayString);
#ifdef DEBUG_LP
    LDEBUG << "NormalizeDate::getDayFromString: testConversion 2 of " << numdayString << "1 day=" << day;
#endif
  }
  // then try conversion of type "10" -> 10
  if( day == NormalizeDateTimeResources::no_day ) {
    bool ok;
    day = numdayString.toUShort(&ok);
#ifdef DEBUG_LP
    LDEBUG << "NormalizeDate::getDayFromString: testConversion 3 of " << numdayString << "1 day=" << day;
#endif
    if( !ok )
      day = NormalizeDateTimeResources::no_day;
  }
  return day;
}

unsigned short NormalizeDate::getDay(RecognizerMatch& m, 
                                     const std::string& featureName) const
{
  // returns the day of month if found in the featureName (numeric value contained in the feature)
  // otherwise, returns the day of week in the alternative (name of day)
  unsigned short day(0);
  if (m.features().find(featureName) != m.features().end())
  {
    LimaString numdayString = (*m.features().find(featureName)).getValueLimaString();
    day = getDayFromString(numdayString);
  }
  if (day>31) { 
    SELOGINIT;
    LWARN << "Error in date normalization: wrong value for day (" << day << ")";
    day=0;
  }
  return day;
}

unsigned short NormalizeDate::getWeekDay(RecognizerMatch& m, 
                                         const std::string& featureName) const
{
  unsigned short day(0);
  if (m.features().find(featureName) != m.features().end() && m_resources != 0) {
    day=m_resources->getDayNumber((*m.features().find(featureName)).getValueLimaString());
  }
  return day;
}

unsigned short NormalizeDate::getMonth(RecognizerMatch& m, 
                                       const std::string& featureName,
                                       const std::string& alternative) const
{
  unsigned short month(0);
  if (m.features().find(featureName) != m.features().end())
  {
    bool ok = true;
    month = (*m.features().find(featureName)).getValueLimaString().toUShort(&ok);
  }
  else if ((alternative != "") && 
           (m.features().find(alternative) != m.features().end()
           && m_resources))
  {
    month = m_resources->getMonthNumber((*m.features().find(alternative)).getValueLimaString());
  }
  return month;
}

unsigned short NormalizeDate::getYear(RecognizerMatch& m, 
                                      const std::string& featureName) const
{
  unsigned short year(0);
  if (m.features().find(featureName) != m.features().end()) {
    year = (*m.features().find(featureName)).getValueLimaString().toUShort();
    // ad hoc correction for two-digit years
    if (year!=0 && year<99)
    {
      // consider that we refer to the current century up to 10 years in the future
      // otherwise we refer to the past century (completely arbitrary rule)
      unsigned short currentYear=QDate::currentDate().year();
      // century (should be 2000)
      unsigned short century=int(float(currentYear)/100)*100;
      if (century+year<=currentYear+10) {
        year=century+year;
      }
      else {
        year=(century-100)+year;
      }
    }
  }
  return year;
}

QString NormalizeDate::getDateSpan(unsigned short year, unsigned short month, unsigned short day) const
{
  QString dateSpan = "XXXX";
  if( year != 0 )
    dateSpan = QString::number(year);
  dateSpan.append("-");
  if( month == 0 )
  {
    dateSpan.append("XX-XX");
  }
  else
  {
    QString monthString = QString(QLatin1String("%1")).arg(month, 2, 10, QLatin1Char('0'));
    dateSpan.append(monthString);
    dateSpan.append("-");
    if( day == 0 )
    {
      dateSpan.append("XX");
    }
    else
    {
      QString dayString = QString(QLatin1String("%1")).arg(day, 2, 10, QLatin1Char('0'));
      dateSpan.append(dayString);
    }
  }
  return dateSpan;
}


bool NormalizeDate::operator()(RecognizerMatch& m,
                               AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "NormalizeDate::operator()"<<m;
#endif
  // cerr << "NormalizeDate::operator() features="<< m.features() << endl;

  // assume all information for normalization is in recognized
  // expression: do not use external information

  unsigned short day=getDay(m, NUMDAY_FEATURE_NAME);
  unsigned short day_end=getDay(m, NUMDAYEND_FEATURE_NAME);
  
  unsigned short month=getMonth(m, NUMMONTH_FEATURE_NAME, MONTH_FEATURE_NAME);
  unsigned short month_end=getMonth(m, NUMMONTHEND_FEATURE_NAME, MONTHEND_FEATURE_NAME);
  
  unsigned short year=getYear(m,NUMYEAR_FEATURE_NAME);
  unsigned short year_end=getYear(m,NUMYEAREND_FEATURE_NAME);
  
  // cerr << "NormalizeDate: day=" << day << ", month=" << month << ", year=" << year 
  //      << ", day_end=" << day_end << ", month_end=" << month_end << ", year_end=" << year_end << endl;
  
  // ad hoc single feature for day and month, when 10/10 is a single t_fraction token
  if (day==0 && month==0 && m.features().find(NUMDAYMONTH_FEATURE_NAME) != m.features().end())
  {
    LimaString daymonth = (*m.features().find(NUMDAYMONTH_FEATURE_NAME)).getValueLimaString();
    QStringList l=daymonth.split("/");
    day=l[0].toUShort();
    month=l[1].toUShort();
    // ad hoc modif: if day and month values are not compatible with a date, try swapping them (english order)
    if (month >12 && day <=12 ) {
      swap(month,day);
    }
  }
 
  QDate referenceDate;
  unsigned short refyear(0), refmonth(0);
  if (m_referenceData.getReferenceDate(analysis,referenceDate))
  {
    refyear=referenceDate.year();
    refmonth=referenceDate.month();
  }

  if (year==0) {
    year=refyear;
    //cerr << "--set default year to " << year << endl;
  }
  if ( year_end == 0 && (day_end != 0 || month_end != 0) ) {
    year_end=refyear;
    //cerr << "--set default year_end to " << year_end << endl;
  }

  // check if explicit interval
  QDate dateEnd;
  if (year_end!=0) {
    if (month_end==0) {
      dateEnd=QDate(year_end,12,31);
    }
    else if (day_end==0) {
      // last day of month
      dateEnd=QDate(year_end,month_end,1).addMonths(1).addDays(-1);
    }
    else {
      dateEnd=QDate(year_end,month_end,day_end);
    }
  }

  QString dateSpan=getDateSpan(year,month,day);
  m.features().setFeature(DATE_SPAN_FEATURE_NAME,dateSpan);
  
  // if incomplete information, set interval values
  if (year!=0) {
    if (day==0) {
      if (month==0) {
        // set interval to whole year
        m.features().setFeature(DATE_BEGIN_FEATURE_NAME,QDate(year,01,01));
        m.features().setFeature(DATE_END_FEATURE_NAME,QDate(year,12,31));
      }
      else {
        // set interval to whole month
        QDate firstDayOfMonth(year,month,1);
        m.features().setFeature(DATE_BEGIN_FEATURE_NAME,firstDayOfMonth);
        m.features().setFeature(DATE_END_FEATURE_NAME,firstDayOfMonth.addMonths(1).addDays(-1));
      }
      // priority to explicit interval
      if (dateEnd.isValid()) { 
        m.features().setFeature(DATE_END_FEATURE_NAME,dateEnd);
      }
    }
    else if (month==0) { // numday set without month: take current month
      month=refmonth;
    }
  }

  // the normalized date
  QDate date=QDate(year,month,day);
  
  if (date.isValid())
  {
    // if interval
    if (dateEnd.isValid()) { 
      m.features().setFeature(DATE_BEGIN_FEATURE_NAME,date);
      m.features().setFeature(DATE_END_FEATURE_NAME,dateEnd);
    }
    else {
      m.features().setFeature(DATE_FEATURE_NAME,date);
    }
    updateCurrentDate(analysis,date);
  }
  else {
    // default normalization: keep string
    m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
  }
  
  updateNormalizedForm(m);
  return true;
}

//**********************************************************************
NormalizeRelativeDate::
NormalizeRelativeDate(MediaId language,
                      const LimaString& complement):
NormalizeDate(language,complement),
m_getNext(false), // default is previous date
m_diff(0)
{
  static const LimaString nextString=Common::Misc::utf8stdstring2limastring("next_");
  if (complement.indexOf(nextString)!=-1) {
    m_getNext=true;
  }
  static const LimaString diffString=Common::Misc::utf8stdstring2limastring("diff_");
  static const LimaString days=Common::Misc::utf8stdstring2limastring("d");
  //static const LimaString months=Common::Misc::utf8stdstring2limastring("m");
  //static const LimaString years=Common::Misc::utf8stdstring2limastring("y");
  //uint64_t i=complement.find(diffString); portage 32 64
  int i=complement.indexOf(diffString);
  if (i!=-1) {
    i+=5; // at end of diff_
    uint64_t end=complement.indexOf(days,i);
    LimaString str = complement.mid(i,end-i);
    uint64_t days=atoi(Common::Misc::limastring2utf8stdstring(str).c_str());
    m_diff=days;
  }
}

QDate adjustDate(unsigned short mention, unsigned short ref, const QDate& refDate, bool getNext, bool months=false) {
  // adjust to previous/next day/month given the day/month mention in the text and the reference day/month
  // modulo is 7 for days, 12 for months
  QDate newDate;
  int diff=mention - ref;
  unsigned short modulo=7;
  int add(0);
  if (months) {
    modulo=12;
  }
  if (getNext) {
    add=(modulo+diff)%modulo;
    if (diff==0) {
      add=modulo; // ad hoc change, if same value in 'next' context, force jump to next
    }
  }
  else {
    add=-((modulo-diff)%modulo);
  }
  //cerr << "adjust date: ref="<< ref <<", mentioned="<< mention 
  //     <<", diff=" << diff << ", add=" << add << endl;
  // ad hoc distinction between days and months by the modulo
  if (months) {
    newDate=refDate.addMonths(add);
  }
  else {
    newDate=refDate.addDays(add);
  }
  return newDate;
}

bool NormalizeRelativeDate::
operator()(RecognizerMatch& m,
           AnalysisContent& analysis) const
{
  // use a reference to normalize the relative date
  //cerr << "NormalizeRelativeDate: " << m.getString() << ", features=" << m.features() << endl;
  
  if (m_resources == 0) {
    // no resources: cannot normalize date
#ifdef DEBUG_LP
    SELOGINIT;
    LDEBUG << "NormalizeRelativeDate: no resources" ;
#endif
    m.features().addFeature(DATESTRING_FEATURE_NAME,m.getString());
    return true;
  }

// "datemod"
// "century"
  unsigned short dayOfMonth=getDay(m, NUMDAY_FEATURE_NAME);
  unsigned short dayOfWeek=getWeekDay(m, DAY_FEATURE_NAME);
  unsigned short month=getMonth(m, NUMMONTH_FEATURE_NAME, MONTH_FEATURE_NAME);
  // assume year is never fixed in relative dates (otherwise, is not relative)
  unsigned short year=0;
  
  // cerr << "NormalizeRelativeDate: dayOfWeek="<< dayOfWeek << ", dayOfMonth=" << dayOfMonth 
  //     << ", month=" << month << endl;
       
  // always sets the span
  QString dateSpan=getDateSpan(year,month,dayOfMonth);
  m.features().setFeature(DATE_SPAN_FEATURE_NAME,dateSpan);

  QDate referenceDate;
  if (! m_referenceData.getReferenceDate(analysis,referenceDate)) {
    m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
    return true;
  }
  unsigned short refday=referenceDate.dayOfWeek();
  unsigned short refmonth=referenceDate.month();
  
  QDate newCurrentDate;

  if (month==0) {
    // relative day
    if (dayOfMonth==0 && dayOfWeek!=NormalizeDateTimeResources::no_day) {
      // adjust according to day of week (lundi prochain)
      newCurrentDate= adjustDate(dayOfWeek, refday, referenceDate, m_getNext);       m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
    }
  }
  else { // there is a month
    // adjust according to month (novembre prochain)
    QDate newDate= adjustDate(month, refmonth, referenceDate, m_getNext, true); 
    // if a day is specified
    if (dayOfMonth!=0) {
      newCurrentDate=QDate(newDate.year(),newDate.month(),dayOfMonth);
      m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
      m.features().setFeature(DATE_SPAN_FEATURE_NAME,getDateSpan(newDate.year(),newDate.month(),dayOfMonth));
    }
    else {
      // set an interval value
      QDate firstDayOfMonth(newDate.year(),newDate.month(),1);
      m.features().setFeature(DATE_BEGIN_FEATURE_NAME,firstDayOfMonth);
      m.features().setFeature(DATE_END_FEATURE_NAME,firstDayOfMonth.addMonths(1).addDays(-1));
      m.features().setFeature(DATE_SPAN_FEATURE_NAME,getDateSpan(newDate.year(),newDate.month(),0));
    }
  }

  // other cases: specific diff (hier,ajourd'hui...)
  if (m_diff != 0) {
    newCurrentDate=referenceDate.addDays(m_diff);
    m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
  }
  
  // special case : centuries
  unsigned short century(0);
  if (m.features().find("century") != m.features().end()) {
    (*m.features().find("century")).getValue();
    try {
      century = boost::any_cast<unsigned short>((*m.features().find("century")).getValue());
    }
    catch(const boost::bad_any_cast &) {
      century = (*m.features().find("century")).getValueLimaString().toUShort();
    }
    // 20 (th century) changed to year 1900
    year = (century-1)*100;
  }

  if (newCurrentDate.isValid()) {
    updateCurrentDate(analysis,newCurrentDate);
  }
  updateNormalizedForm(m);
  return true;
}

//**********************************************************************
NormalizeTime::
NormalizeTime(MediaId language,
              const LimaString& complement):
Automaton::ConstraintFunction(language,complement),
m_language(language),
m_resources(0),
m_referenceData()
{
  // default reference date and location are the one indicated for
  // the document, if set...

  // parse complement to find reference location and reference date,
  // if indicated
  m_referenceData.parseComplement(complement);

  try {
    AbstractResource* res=LinguisticResources::changeable().
      getResource(language,"DateTimeNormalization");
    if (res==0) {
      SELOGINIT;
      LWARN << "no resource 'DateTimeNormalization'";
    }
    else {
      m_resources=static_cast<const NormalizeDateTimeResources*>(res);
    }
  }
  catch (std::exception& e) {
    // resources are not loaded when compiling the rules
  }
}

QTime NormalizeTime::
getTimeDuration(const RecognizerMatch& m) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "NormalizeTime::getTimeDuration...";
#endif
  QTime timeDuration;

  unsigned short hou(0),min(0),sec(0);
  if (m.features().find("hour") != m.features().end()) {
    try {
      hou = boost::any_cast<unsigned short>((*m.features().find("hour")).getValue());
    }
    catch(const boost::bad_any_cast &) {
      hou = (*m.features().find("hour")).getValueLimaString().toUShort();
    }
  }
  if (m.features().find("minute") != m.features().end()) {
    try {
      min = boost::any_cast<unsigned short>((*m.features().find("minute")).getValue());
    }
    catch(const boost::bad_any_cast &) {
      min = (*m.features().find("minute")).getValueLimaString().toUShort();
    }
  }
  if (m.features().find("second") != m.features().end()) {
    try {
      sec = boost::any_cast<unsigned short>((*m.features().find("second")).getValue());
    }
    catch(const boost::bad_any_cast &) {
      sec = (*m.features().find("second")).getValueLimaString().toUShort();
    }
  }

#ifdef DEBUG_LP
  LDEBUG << "NormalizeTime::getTimeDuration h=" << hou << ",m=" << min << ",s" << sec;
#endif

  if (m.features().find("time") != m.features().end()) {
    std::string timeString=(*m.features().find("time")).getValueLimaString().toUtf8().constData();
    //uint64_t i=timeString.find(':'); portage 32 64
    string::size_type i=timeString.find(':');
    if (i!=string::npos) {
      try {
        // has at least one ":" sep -> guess it has form hh:mm or hh:mm:ss,
        // try use direct construction from string
        timeDuration=QTime::fromString(QString::fromUtf8(timeString.c_str()));
      }
      catch (std::exception& e) {
        SELOGINIT;
        LWARN << "Error getting time duration from string '"
               << timeString << "'";
        return timeDuration;
      }
    }
    else {
      i=timeString.find_first_of("hH");
      if (i!=string::npos) {
        hou=atoi(string(timeString,0,i).c_str());
        min=atoi(string(timeString,i+1).c_str());
      }
    }
  }
  timeDuration=QTime(hou,min,sec);
  return timeDuration;
}


// normalizeTime helper functions
QTime NormalizeTime::
getUTCTime(const QDate& d,
           const QTime& duration/*,
           const std::string& location,
           const boost::local_time::tz_database& tz_db*/) const
{
  return QDateTime(d,duration).toUTC().time();
}

NormalizeLocalTime::
NormalizeLocalTime(MediaId language,
                   const LimaString& complement):
NormalizeTime(language,complement)
{
}

bool NormalizeLocalTime::
operator()(RecognizerMatch& m,
           AnalysisContent& analysis) const
{
  // normalize hour to UTC => needs to know the location
  // use 'DocumentLocation' if exists
  // otherwise, use adaptor from locale

  // get reference dates and location
  QDate referenceDate;
  std::string referenceLocation;
  bool hasReferenceLocation(true);

  if (! getReferenceData().getReferenceDate(analysis,referenceDate)) {
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
    return true;
  }

  if (! getReferenceData().getReferenceLocation(analysis,referenceLocation)) {
    SELOGINIT;
    LWARN << "no reference location '"<< getReferenceData().getLocRefName()
           << "' found";
    hasReferenceLocation=false;
  }

  // parse recognized expression to get duration
  QTime timeDuration=getTimeDuration(m);
  if (!timeDuration.isValid()) {
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
    return true;
  }

  try {
    QTime local(QDateTime(referenceDate,timeDuration).time());

    m.features().setFeature(LOCALTIME_FEATURE_NAME,local);

    if (! hasReferenceLocation) {
      return true;
    }

    if (hasResources() && getResources()->hasTimezoneDatabase()) {
      QTime utc=getUTCTime(referenceDate,timeDuration/*,
                                       referenceLocation,
                                       getResources()->getTimezoneDatabase()*/);
      if (!utc.isValid()) {
        SELOGINIT;
        LWARN << "failed to normalize time: ";
        LDEBUG << "NormalizeLocalTime::operator(): m.getString()=" << m.getString();
        m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
      }
      else {
        m.features().setFeature(UTCTIME_FEATURE_NAME,utc);
      }
    }
  }
  catch (std::exception& e) {
    SELOGINIT;
    LWARN << "exception caught in time normalization of '"
          << Common::Misc::limastring2utf8stdstring(m.getString())
          << "': " << e.what();
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
  }
  return true;
}

//**********************************************************************
NormalizeUTCTime::
NormalizeUTCTime(MediaId language,
                 const LimaString& complement):
NormalizeTime(language,complement)
{
}

bool NormalizeUTCTime::
operator()(RecognizerMatch& m,
           AnalysisContent& analysis) const
{
  // do not use a reference location, time is supposed to be UTC already
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "NormalizeUTCTime::operator()...";
#endif

  // get reference date
  QDate referenceDate;
  if (! getReferenceData().getReferenceDate(analysis,referenceDate)) {
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
#ifdef DEBUG_LP
    LDEBUG << "NormalizeUTCTime::operator: setFeature(TIMESTRING_FEATURE_NAME=" << m.getString();
#endif
    return true;
  }

  QTime timeDuration=getTimeDuration(m);
  if (!timeDuration.isValid()) {
    SELOGINIT;
    LWARN << "cannot compute time duration for '"
          << Common::Misc::limastring2utf8stdstring(m.getString())
          << "'";
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
    return true;
  }

  try {
    QTime utcTime(QDateTime(referenceDate,timeDuration).toUTC().time());
    m.features().setFeature(UTCTIME_FEATURE_NAME,utcTime);
  }
  catch (std::exception& e) {
    SELOGINIT;
    LWARN << "exception caught in time normalization of '"
          << Common::Misc::limastring2utf8stdstring(m.getString())
          << "': " << e.what();
    m.features().setFeature(TIMESTRING_FEATURE_NAME,m.getString());
  }

  return true;
}

} // end namespace
} // end namespace
} // end namespace
