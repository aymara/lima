// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGLINGUISTICMETADATA_H
#define LIMA_LINGUISTICPROCESSINGLINGUISTICMETADATA_H

#include "LinguisticProcessorsExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

#include <QDate>
namespace Lima
{

namespace LinguisticProcessing
{

/**
 * @brief contains metadata to be kept during the analysis:
 * global generic metadata: file name, language,...
 *
 * @author Benoit Mathieu
 */
class LIMA_LINGUISTICPROCESSORS_EXPORT LinguisticMetaData : public AnalysisData
{
public:
  LinguisticMetaData();
  virtual ~LinguisticMetaData();

  bool hasMetaData(const std::string& id) const ;
  const std::string& getMetaData(const std::string& id) const ;
  const std::map<std::string,std::string>& getAllMetaData(void) const;
  void setMetaData(const std::string& id,const std::string& value);
  void setMetaData(const std::map<std::string,std::string>& metaDatas);

  uint64_t getStartOffset() const;
  void setStartOffset(uint64_t startOffset);

  const QDate& getDate(const std::string& dateName) const;
  const QTime& getTime(const std::string& timeName) const;
  const std::string& getLocation(const std::string& locName) const;
  void setDate(const std::string& dateName,const QDate&);
  void setTime(const std::string& timeName,const QTime&);
  void setLocation(const std::string& locName,const std::string&);

private:

  std::map<std::string,std::string> m_metaData;
  uint64_t m_startOffset;
  // add named information about dates, times and locations: 
  // corresponding to the document, or current 
  std::map<std::string,QDate> m_dates;
  std::map<std::string,QTime> m_times;
  std::map<std::string,std::string> m_locations;
};

} // LinguisticProcessing

} // Lima

#endif
