// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROCESSUNITFRAMEWORK_ANALYSISCONTENT_H
#define LIMA_COMMON_PROCESSUNITFRAMEWORK_ANALYSISCONTENT_H

#include "common/LimaCommon.h"

#include <map>
#include <string>

namespace Lima {

/**
  * @brief just for semantics: base class for analysisData
  */
class LIMA_PROCESSUNITFRAMEWORK_EXPORT AnalysisData 
{ 
public:
  virtual ~AnalysisData() {}
};

/**
  * @brief Holds all data that pass through the ProcessUnits
  * Warning : destructor of AnalysisContent call destructor \
  * on each AnalysisData
  */
class LIMA_PROCESSUNITFRAMEWORK_EXPORT AnalysisContent 
{
public:
  
  AnalysisContent();
  
  /**
   * Destroy all AnalysisData in AnalysisContent
   * @return 
   */
  virtual ~AnalysisContent();

  /**
   * @brief return AnalysisData by id
   * @param id Id of analysis to return
   * @return AnalysisData*
   * @retval pointer to valid AnalysisData if exists
   * @retval 0 if specified AnalysisData doesn't exists
   */
  AnalysisData* getData(const std::string& id);
  
  /**
   * @brief return AnalysisData by id
   * @param id Id of analysis to return
   * @return AnalysisData*
   * @retval pointer to valid AnalysisData if exists
   * @retval 0 if specified AnalysisData doesn't exists
   */
  const AnalysisData* getData(const std::string& id) const;
  
  /**
   * @brief set an analysisData with the given id.
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store
   */
  void setData(const std::string& id,AnalysisData* data);
  
  /** 
   * @brief remove the analysisData with the given id
   * (call destructor of the data)
   * @param id the identifier of the AnalysisData to remove
   */
  void removeData(const std::string& id);

  /**
  * @brief remove all the analysisData
  * (call destructor of the data)
  */
  void clear();
  
  /**
   * @brief remove the analysisData with the given id without destructing it
   * (do NOT call the destructor of the data)
   * @param id the identifier of the AnalysisData to release
   */
  void releaseData(const std::string& id);

  private:

  std::map<std::string,AnalysisData*> m_analysisData;
  
};

} // Lima

#endif
