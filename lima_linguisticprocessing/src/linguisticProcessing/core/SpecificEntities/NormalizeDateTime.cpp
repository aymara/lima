/*
    Copyright 2002-2019 CEA LIST

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
#define MONTH_FEATURE_NAME "month"
#define YEAR_FEATURE_NAME "year"
#define NUMDAY_FEATURE_NAME "numday"
#define NUMMONTH_FEATURE_NAME "nummonth"
#define NUMYEAR_FEATURE_NAME "numyear"

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
  const AnalysisData* data=analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  const LinguisticMetaData* metadata=dynamic_cast<const LinguisticMetaData*>(data);

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
  const AnalysisData* data=analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  const LinguisticMetaData* metadata=dynamic_cast<const LinguisticMetaData*>(data);

  time=metadata->getTime(m_timeRefName);
  return (time.isValid());
}

bool ReferenceData::
getReferenceLocation(const AnalysisContent& analysis,
                     std::string& location) const
{
  // get posix time from current text metadata
  const AnalysisData* data=analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return false;
  }
  const LinguisticMetaData* metadata=dynamic_cast<const LinguisticMetaData*>(data);

  location=metadata->getLocation(m_locRefName);
  return (! location.empty());
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
  AnalysisData* data=analysis.getData("LinguisticMetaData");
  if (data == 0) {
    SELOGINIT;
    LERROR << "missing data 'LinguisticMetaData'";
    return;
  }
  LinguisticMetaData* metadata=dynamic_cast<LinguisticMetaData*>(data);

  metadata->setDate("current",currentDate);
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

bool NormalizeDate::operator()(RecognizerMatch& m,
                               AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "NormalizeDate::operator()"<<m;
#endif

  // assume all information for normalization is in recognized
  // expression: do not use external information

  unsigned short day(0);
  if (m.features().find(NUMDAY_FEATURE_NAME) != m.features().end())
  {
    LimaString numdayString = (*m.features().find(NUMDAY_FEATURE_NAME)).getValueLimaString();
    day = getDayFromString(numdayString);
  }
  unsigned short day_end(0);
  if (m.features().find("numdayend") != m.features().end())
  {
    LimaString numdayString = (*m.features().find("numdayend")).getValueLimaString();
    day_end = getDayFromString(numdayString);
  }
  unsigned short month(0);
  if (m.features().find(NUMMONTH_FEATURE_NAME) != m.features().end())
  {
    bool ok = true;
    month = (*m.features().find(NUMMONTH_FEATURE_NAME)).getValueLimaString().toUShort(&ok);
  }
  else if (m.features().find(MONTH_FEATURE_NAME) != m.features().end()
            && m_resources)
  {
    month = m_resources->getMonthNumber((*m.features().find(MONTH_FEATURE_NAME)).getValueLimaString());
  }
  unsigned short month_end(0);
  if (m.features().find("nummonthend") != m.features().end())
  {
    bool ok = true;
    month_end = (*m.features().find("nummonthend")).getValueLimaString().toUShort(&ok);
  }
  else if (m.features().find("monthend") != m.features().end() && m_resources) {
    month_end = m_resources->getMonthNumber((*m.features().find("monthend")).getValueLimaString());
  }
  unsigned short year(0);
  if (m.features().find(NUMYEAR_FEATURE_NAME) != m.features().end()) {
    year = (*m.features().find(NUMYEAR_FEATURE_NAME)).getValueLimaString().toUShort();
  }

/////////////////////////////////////////////////////////////////////////////
  for (RecognizerMatch::const_iterator i(m.begin()); i!=m.end(); i++)
  {
    if (! (*i).isKept())
    {
      continue;
    }
    Token* t = m.getToken(i);
    MorphoSyntacticData* data = m.getData(i);
    LIMA_UNUSED(data);
    const TStatus& status=t->status();
#ifdef DEBUG_LP
    LDEBUG << "Token"<<t->stringForm()<<"status is:" << status.toString();
#endif
    if (status.getStatus()==T_NUMERIC)
    {
//     else {
      if (status.getNumeric()==T_FRACTION)
      {
        uint64_t pos(t->stringForm().indexOf(LimaChar('/')));
        uint64_t val1=t->stringForm().leftRef(pos).toUInt();
        uint64_t val2=t->stringForm().midRef(pos+1).toUInt();
        if (val1 > 31)
        {
          //assume year
          if (year == 0)
          {
            year=val1;
            m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
          }
          //assume next is month
          if (month == 0 && val2 <= 12)
          {
            month=val2;
            m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
          }
          else
          { // should not happen => it may not be a date
          }
        }
        // otherwhise, suppose day before month, but test it
        else if (val2 > 12)
        {
          if (day == 0)
          {
            day=val2;
            m.features().setFeature(NUMDAY_FEATURE_NAME, day);
          }
          if (month == 0)
          {
            month=val1;
            m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
          }
        }
        else
        {
          if (day == 0)
          {
            day=val1;
            m.features().setFeature(NUMDAY_FEATURE_NAME, day);
          }
          if (month == 0)
          {
            month=val2;
            m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
          }
        }
      }
      else if (isInteger(t))
      {
        uint64_t value=LimaStringToInt(t->stringForm());
        if (value < 31)
        {
          if (day==0)
          { // suppose day before month
            day = value;
            m.features().setFeature(NUMDAY_FEATURE_NAME, day);
          }
          else if (m_isInterval && day_end==0)
          {
            day_end=value;
          }
          else if (month==0)
          {
            if (value > 12)
            { // swap : month cant be > 12
              month = day;
              m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
              if (day == 0)
              {
                day = value;
                m.features().setFeature(NUMDAY_FEATURE_NAME, day);
              }
            }
            else
            { // assume month
              if (month == 0)
              {
                month = value;
                m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
              }
            }
          }
          else
          { // month and day are assigned -> assume year
            if (year == 0)
            {
              year = value;
              m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
            }
          }
        }
        else
        {
          if (value > 1000 && value < 3000)
          {
            if (year == 0)
            {
              year = value;
              m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
            }
          }
          else if (month!=0)
          {
            // can be a year on two digits -> year assumed if
            // day and month are already assigned
            if (year == 0)
            {
              year = value;
              m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
            }
          }
        }
      }
    }
    else
    {
//     if (testMicroCategory(m_microsForMonth,m_microAccessor,data)) {
      if (isInteger(t))
      {
        if (month == 0)
        {
          month=LimaStringToInt(t->stringForm());
          m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
        }
      }
      else if (m_resources)
      {
        LimaString monthString = m.features().find("month")==m.features().end() ? t->stringForm() : (*m.features().find("month")).getValueLimaString();
        unsigned short monthNum=m_resources->getMonthNumber(monthString);
        if (monthNum==NormalizeDateTimeResources::no_month)
        {
          // failed to recognize month => no normalization
          SELOGINIT;
          LWARN << "NormalizeDate: '" << monthString << "' not recognized as month";
          m.features().addFeature(DATESTRING_FEATURE_NAME,m.getString());
        }
        else
        {
          if (month!=0 && m_isInterval)
          {
            if (month_end == 0)
            {
              month_end=monthNum;
            }
          }
          else
          {
            if (month == 0)
            {
              month=monthNum;
              m.features().setFeature(NUMMONTH_FEATURE_NAME, month);
            }
          }
        }
      }
    }
  }
/////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_LP
  LDEBUG << "NormalizeDate operator(): day=" << day << ", day_end=" << day_end;
  LDEBUG << "NormalizeDate operator(): month=" << month << ", month_end=" << month_end;
  LDEBUG << "NormalizeDate operator(): year=" << year;
#endif
  //ad hoc correction of year on two digits
  if (year!=0 && year<99)
  {
    if (year < 10)
    {
      year+=2000;
      m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
    }
    else
    {
      year+=1900;
      m.features().setFeature(NUMYEAR_FEATURE_NAME, year);
    }
  }

  QDate newCurrentDate;
  try { // catch date conversion exceptions
    if (day==0 && month==0 && year==0)
    {
      //const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
      SELOGINIT;
      LWARN << "NormalizeDate: no day, month or year identified in "
             << Common::Misc::limastring2utf8stdstring(m.getString());
      m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
    }
    else
    {
      // at least one is no null in day / month / year
      if (day==0)
      {
        // 0 / _ / _
        if (month==0)
        {
          // 0 / _ / Y
          // only year : do not set interval of dates from first to last day of year
          // set only year : cast to int in features
          m.features().setFeature(NUMYEAR_FEATURE_NAME,static_cast<int>(year));
        }
        else
        {
          // 0 / M / ?
          // set interval
          QDate firstDayOfMonth(year,month,1);
#ifdef DEBUG_LP
          LDEBUG << "NormalizeDate operator(): day=0 and month != 0 => date_begin=" << firstDayOfMonth;
#endif
          m.features().setFeature(DATE_BEGIN_FEATURE_NAME,firstDayOfMonth);
          if (month_end==0)
          {
            QDate date_end = firstDayOfMonth.addMonths(1).addDays(-1);
#ifdef DEBUG_LP
          LDEBUG << "NormalizeDate operator(): day=0 and month != 0 => date_end=" << date_end;
#endif
            m.features().setFeature(DATE_END_FEATURE_NAME,date_end);
          }
          else {
            QDate date_end(year,month_end,1);
            m.features().setFeature(DATE_END_FEATURE_NAME,date_end);
          }
        }
      }
      else
      {
        // D / _ / _
        if (month==0)
        {
          // D / 0 / _
          if (year==0)
          {
            // D / 0 / 0
            //day only => take month and year from reference
            QDate referenceDate;
            if (! m_referenceData.getReferenceDate(analysis,referenceDate))
            {
              m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
            }
            else
            {
              int refYear=referenceDate.year();
              int refMonth=referenceDate.month();
              newCurrentDate=QDate(refYear,refMonth,day);
              m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
            }
          }
          else
          {
            // D / 0 / Y
            // day and year => should not happen: failed to normalize: set only string
            SELOGINIT;
            LWARN << "NormalizeDate: only day and year in "
                   << Common::Misc::limastring2utf8stdstring(m.getString())
                  ;
            m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
          }
        }
        else
        {
          // D / M / _
          if (year==0)
          {
            // D / M / 0
            // get year from reference
            QDate referenceDate;
            if (! m_referenceData.getReferenceDate(analysis,referenceDate))
            {
              m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
            }
            else
            {
              int refYear=referenceDate.year();
              newCurrentDate=QDate(refYear,month,day);
              if (day_end==0)
              {
                m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
              }
              else
              {
                m.features().setFeature(DATE_BEGIN_FEATURE_NAME,QDate(refYear,month,day));
                m.features().setFeature(DATE_END_FEATURE_NAME,QDate(refYear,month,day_end));
              }
            }
          }
          else
          {
            // D / M / Y
            // complete !!
            if (day_end==0)
            {
              newCurrentDate=QDate(year,month,day);
              m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
            }
            else
            {
              m.features().setFeature(DATE_BEGIN_FEATURE_NAME,QDate(year,month,day));
              m.features().setFeature(DATE_END_FEATURE_NAME,QDate(year,month,day_end));
            }
          }
        }
      }
    }
  }
  catch (std::exception& e) {
    SELOGINIT;
    LWARN << "Error trying to normalize date "
          << Common::Misc::limastring2utf8stdstring(m.getString())
          << ":" << e.what()
         ;
    m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
  }

  QString dateSpan = "XXXX";
  if( year != 0 )
    dateSpan = QString::number(year);
#ifdef DEBUG_LP
  LDEBUG << "NormalizeDate operator(): year: dateSpan=" << dateSpan;
#endif
  dateSpan.append("-");
  if( month == 0 )
  {
    dateSpan.append("XX-XX");
#ifdef DEBUG_LP
    LDEBUG << "NormalizeDate operator(): year + xx-xx dateSpan=" << dateSpan;
#endif
  }
  else
  {
    // dateSpan.append(QString::number(month));
    QString monthString = QString(QLatin1String("%1")).arg(month, 2, 10, QLatin1Char('0'));
    dateSpan.append(monthString);
#ifdef DEBUG_LP
    LDEBUG << "NormalizeDate operator(): year + month dateSpan=" << dateSpan;
#endif
    dateSpan.append("-");
    if( day == 0 )
    {
      dateSpan.append("XX");
#ifdef DEBUG_LP
      LDEBUG << "NormalizeDate operator(): year + month + xx dateSpan=" << dateSpan;
#endif
    }
    else
    {
      // QString QString::arg(int integerVar, int fieldWidth = 0, int base = 10, const QChar & fillChar = QLatin1Char( ' ' )) const
      QString dayString = QString(QLatin1String("%1")).arg(day, 2, 10, QLatin1Char('0'));
      dateSpan.append(dayString);
#ifdef DEBUG_LP
      LDEBUG << "NormalizeDate operator(): year + month + day dateSpan=" << dateSpan;
#endif
    }
  }

  m.features().setFeature(DATE_SPAN_FEATURE_NAME,dateSpan);
  if (!newCurrentDate.isNull())
  {
#ifdef DEBUG_LP
    LDEBUG << "NormalizeDate::operator() set value feature to" << newCurrentDate.toString("dd/MM/yyyy");
#endif
    m.features().setFeature("value", newCurrentDate.toString("dd/MM/yyyy"));
  }
  else
  {
    if (m.features().find(DATE_BEGIN_FEATURE_NAME) != m.features().end())
    {
      QDate beginFeature = boost::any_cast<QDate>((*m.features().find(DATE_BEGIN_FEATURE_NAME)).getValue());
#ifdef DEBUG_LP
      LDEBUG << "NormalizeDate::operator() set value feature to" << beginFeature.toString("dd/MM/yyyy");
#endif
      m.features().setFeature("value",
                              beginFeature.toString("dd/MM/yyyy"));
    }
  }
  if (newCurrentDate.isValid())
  {
    updateCurrentDate(analysis,newCurrentDate);
  }
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
  auto i=complement.indexOf(diffString);
  if (i!=-1) {
    i+=5; // at end of diff_
    uint64_t end=complement.indexOf(days,i);
    LimaString str = complement.mid(i,end-i);
    uint64_t days=atoi(Common::Misc::limastring2utf8stdstring(str).c_str());
    m_diff=days;
  }
}

bool NormalizeRelativeDate::
operator()(RecognizerMatch& m,
           AnalysisContent& analysis) const
{
  // use a reference to normalize the relative date

  if (m_resources == 0) {
    // no resources: cannot normalize date
#ifdef DEBUG_LP
    SELOGINIT;
    LDEBUG << "NormalizeRelativeDate: no resources" ;
#endif
    m.features().addFeature(DATESTRING_FEATURE_NAME,m.getString());
    return true;
  }

  QDate referenceDate;
  QDate newCurrentDate;


  if (! m_referenceData.getReferenceDate(analysis,referenceDate)) {
    m.features().setFeature(DATESTRING_FEATURE_NAME,m.getString());
    return true;
  }

// "datemod"
// "century"
  unsigned short day(0);
  if (m.features().find("day") != m.features().end() && m_resources != 0) {
    day=m_resources->getDayNumber((*m.features().find("day")).getValueLimaString());
  }
  // test if it is a day name
  if (day!=NormalizeDateTimeResources::no_day) {
    if (day == referenceDate.dayOfWeek()) {
      // same day of week as reference: assume it's the same date
      newCurrentDate=referenceDate;
      m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
    }
    else if (m_getNext) {
      // get next day according to reference
      newCurrentDate=referenceDate.addDays(1);
//         first_day_of_the_week_after nextDay(day);
//         newCurrentDate=nextDay.get_date(referenceDate);
      m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
    }
    else {
      // get previous day according to reference
      newCurrentDate=referenceDate.addDays(-1);
//         first_day_of_the_week_before prevDay(day);
//         newCurrentDate=prevDay.get_date(referenceDate);
      m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
    }
  }

  unsigned short dayOfMonth(0);
  if (m.features().find(NUMDAY_FEATURE_NAME) != m.features().end()) {
    bool ok = true;
    dayOfMonth = (*m.features().find(NUMDAY_FEATURE_NAME)).getValueLimaString().toUShort(&ok);
    if (!ok && m_resources) {
      dayOfMonth = m_resources->getDayNumber((*m.features().find(NUMDAY_FEATURE_NAME)).getValueLimaString());
    }
  }
  if (dayOfMonth>31) { dayOfMonth=0; }

  unsigned short month(0);
  if (m.features().find("month") != m.features().end()) {
    bool ok = true;
    month = (*m.features().find("month")).getValueLimaString().toUShort(&ok);
    if (!ok && m_resources) {
      month = m_resources->getMonthNumber((*m.features().find("month")).getValueLimaString());
    }
  }
  unsigned short year=referenceDate.year();
  if (m.features().find(NUMYEAR_FEATURE_NAME) != m.features().end()) {
    year = (*m.features().find(NUMYEAR_FEATURE_NAME)).getValueLimaString().toUShort();
  }

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

  // test if it is a month name
  if (month!=NormalizeDateTimeResources::no_month) {
    int refmonth=referenceDate.month();
    // possible change of year
    if (m_getNext) {
      if (refmonth>month) {
        year++;
      }
    }
    else {
      if (refmonth<month) {
        year--;
      }
    }
    // if a day is specified
    if (dayOfMonth!=0) {
      newCurrentDate=QDate(year,month,dayOfMonth);
      m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
    }
    else {
      // set an interval value
      QDate firstDayOfMonth(year,month,1);
      m.features().setFeature(DATE_BEGIN_FEATURE_NAME,firstDayOfMonth);
      m.features().setFeature(DATE_END_FEATURE_NAME,firstDayOfMonth.addMonths(1).addDays(-1));
    }
  }

  // other cases: maybe a diff (hier,ajourd'hui...)
  if (m_diff != 0) {
    newCurrentDate=referenceDate.addDays(m_diff);
    m.features().setFeature(DATE_FEATURE_NAME,newCurrentDate);
  }

  if (newCurrentDate.isValid()) {
    updateCurrentDate(analysis,newCurrentDate);
  }
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
    string::size_type i = timeString.find(':');
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
