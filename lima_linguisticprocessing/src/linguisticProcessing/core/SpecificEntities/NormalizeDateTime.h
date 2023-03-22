// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       NormalizeDateTime.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief      actions for the normalization of dates and times
 * 
 ***********************************************************************/

#ifndef NORMALIZEDATETIME_H
#define NORMALIZEDATETIME_H

#include "SpecificEntitiesExport.h"
#include "NormalizeDateTimeResources.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

//**********************************************************************
// ids of actions defined in this file

#define NormalizeDateId "NormalizeDate"
#define NormalizeRelativeDateId "NormalizeRelativeDate"
#define NormalizeLocalTimeId "NormalizeLocalTime"
#define NormalizeUTCTimeId "NormalizeUTCTime"

// a class to store reference objects for relative normalization
class LIMA_SPECIFICENTITIES_DATETIME_EXPORT ReferenceData {
 public:
  ReferenceData();
  ~ReferenceData();

  void parseComplement(const LimaString& complement);
  void parseReference(const LimaString& str);
  bool getReferenceDate(const AnalysisContent& analysis,
                        QDate& date) const;
  bool getReferenceTime(const AnalysisContent& analysis,
                        QTime& time) const;
  bool getReferenceLocation(const AnalysisContent& analysis,
                            std::string& location) const;

  const std::string& getDateRefName() const { return m_dateRefName; }
  const std::string& getTimeRefName() const { return m_timeRefName; }
  const std::string& getLocRefName() const { return m_locRefName; }

  void setDateRefName(const std::string& name) { m_dateRefName=name; }
  void setTimeRefName(const std::string& name) { m_timeRefName=name; }
  void setLocRefName(const std::string& name) { m_locRefName=name; }

 private:
  std::string m_dateRefName;
  std::string m_timeRefName;
  std::string m_locRefName;
};

class LIMA_SPECIFICENTITIES_DATETIME_EXPORT NormalizeDate : public Automaton::ConstraintFunction
{
public:
  NormalizeDate(MediaId language,
                const LimaString& complement=LimaString());
  ~NormalizeDate() {}

  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

protected:
  MediaId m_language;
  std::shared_ptr<NormalizeDateTimeResources> m_resources;
  ReferenceData m_referenceData;
  const std::set<LinguisticCode>* m_microsForMonth;
  const std::set<LinguisticCode>* m_microsForDays;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  bool m_isInterval;

  void updateCurrentDate(AnalysisContent& analysis,
                         const QDate& currentDate) const;
  unsigned short getDayFromString(const LimaString& numdayString) const;
  unsigned short getDay(Automaton::RecognizerMatch& m, const std::string& featureName) const;
  unsigned short getWeekDay(Automaton::RecognizerMatch& m, const std::string& featureName) const;
  unsigned short getMonth(Automaton::RecognizerMatch& m, const std::string& featureName, const std::string& alt="") const;
  unsigned short getYear(Automaton::RecognizerMatch& m, const std::string& featureName) const;
  unsigned short getDefaultYear(AnalysisContent& analysis) const;
  QString getDateSpan(unsigned short year, unsigned short month, unsigned short day) const;

};

class LIMA_SPECIFICENTITIES_DATETIME_EXPORT NormalizeRelativeDate : public NormalizeDate
{
public:
  NormalizeRelativeDate(MediaId language,
                        const LimaString& complement=LimaString());
  ~NormalizeRelativeDate() {}

  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }
private:
  bool m_getNext;
  int m_diff;
};

// abstract constraint that contains common functions for
// time normalization
class LIMA_SPECIFICENTITIES_DATETIME_EXPORT NormalizeTime : public Automaton::ConstraintFunction
{
public:
  NormalizeTime(MediaId language,
                const LimaString& complement=LimaString());
  virtual ~NormalizeTime() {}
  
  virtual bool operator()(Automaton::RecognizerMatch& m,
                          AnalysisContent& analysis) const override=0;
  virtual bool actionNeedsRecognizedExpression() override { return true; }

  const ReferenceData& getReferenceData() const { return m_referenceData; }
  bool hasResources() const { return (m_resources!=0); }
  const std::shared_ptr<NormalizeDateTimeResources> getResources() const { return m_resources; }
  
protected:
  //private member functions
  QTime 
    getUTCTime(const QDate& d,
               const QTime& duration/*,
               const std::string& location,
               const boost::local_time::tz_database& tz_db*/) const;
  QTime getTimeDuration(const Automaton::RecognizerMatch& m) const;

private:
  MediaId m_language;
  std::shared_ptr<NormalizeDateTimeResources> m_resources;
  ReferenceData m_referenceData;
};

class LIMA_SPECIFICENTITIES_DATETIME_EXPORT NormalizeLocalTime : public NormalizeTime
{
public:
  NormalizeLocalTime(MediaId language,
                const LimaString& complement=LimaString());
  ~NormalizeLocalTime() {}

  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;
private:
};

class LIMA_SPECIFICENTITIES_DATETIME_EXPORT NormalizeUTCTime : public NormalizeTime
{
public:
  NormalizeUTCTime(MediaId language,
                     const LimaString& complement=LimaString());
  ~NormalizeUTCTime() {}
  
  bool operator()(Automaton::RecognizerMatch& m,
                  AnalysisContent& analysis) const override;
private:
};

} // end namespace
} // end namespace
} // end namespace

#endif
