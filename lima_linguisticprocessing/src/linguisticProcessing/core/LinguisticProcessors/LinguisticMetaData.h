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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGLINGUISTICMETADATA_H
#define LIMA_LINGUISTICPROCESSINGLINGUISTICMETADATA_H

#include "LinguisticProcessorsExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

#include "common/misc/gregoriannowarn.hpp"
#include "common/misc/posix_timenowarn.hpp"

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

  const std::string& getMetaData(const std::string& id) const ;
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
