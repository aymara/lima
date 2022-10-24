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
#include <memory>
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

class AnalysisContentPrivate;
/**
  * @brief Holds all data that pass through the ProcessUnits
  * Analysis data are shared pointers, simplifying memory management.
  */
class LIMA_PROCESSUNITFRAMEWORK_EXPORT AnalysisContent 
{
public:
  
  AnalysisContent();
  AnalysisContent(const AnalysisContent&) = delete;
  AnalysisContent& operator=(const AnalysisContent&) = delete;

  /**
   * Reduce the reference count of all AnalysisData in AnalysisContent causing them to be deleted if needed.
   * @return 
   */
  virtual ~AnalysisContent();

  /**
   * @brief return AnalysisData by id
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  std::shared_ptr< AnalysisData > getData(const QString& id);

  /**
   * @brief return constant AnalysisData by id
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  const std::shared_ptr< AnalysisData > getData(const QString& id) const;

  /**
   * @brief set an analysisData with the given id.
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store.
   */
  void setData(const QString& id, std::shared_ptr< AnalysisData > data);

  /**
   * @brief set an analysisData with the given id. Take ownership of the pointer.
   * @deprecated Use the shared_ptr-based API instead
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store. Take ownership of the pointer.
   */
  void setData(const QString& id, AnalysisData* data);

  /**
   * @brief remove the analysisData with the given id
   * @param id the identifier of the AnalysisData to remove
   */
  void removeData(const QString& id);

  /**
   * @brief return AnalysisData by id
   * @deprecated Use the QString-based API instead
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  std::shared_ptr< AnalysisData > getData(const std::string& id);

  /**
   * @brief return AnalysisData by id
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  std::shared_ptr< AnalysisData > getData(const char* id);

  /**
   * @brief return constant AnalysisData by id
   * @deprecated Use the QString-based API instead
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  const std::shared_ptr< AnalysisData > getData(const std::string& id) const;

  /**
   * @brief return constant AnalysisData by id
   * @param id Id of analysis to return
   * @return std::shared_ptr< AnalysisData >
   * @retval shared pointer to valid AnalysisData if exists
   * @retval empty shared pointer if specified AnalysisData doesn't exists
   */
  const std::shared_ptr< AnalysisData > getData(const char* id) const;

  /**
   * @brief set an analysisData with the given id.
   * @deprecated Use the QString-based API instead
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store.
   */
  void setData(const std::string& id, std::shared_ptr< AnalysisData > data);

  /**
   * @brief set an analysisData with the given id.
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store.
   */
  void setData(const char* id, std::shared_ptr< AnalysisData > data);

  /**
   * @brief set an analysisData with the given id. Take ownership of the pointer.
   * @deprecated Use the QString-based API instead
   * @deprecated Use the shared_ptr-based API instead
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store. Take ownership of the pointer.
   */
  void setData(const std::string& id, AnalysisData* data);

  /**
   * @brief set an analysisData with the given id. Take ownership of the pointer.
   * @deprecated Use the shared_ptr-based API instead
   * @param id Id to design the given analysisData
   * @param data AnalysisData to store. Take ownership of the pointer.
   */
  void setData(const char* id, AnalysisData* data);

  /**
   * @brief remove the analysisData with the given id
   * @param id the identifier of the AnalysisData to remove
   */
  void removeData(const char* id);

  /**
   * @brief remove the analysisData with the given id
   * @deprecated Use the QString-based API instead
   * @param id the identifier of the AnalysisData to remove
   */
  void removeData(const std::string& id);

  /**
  * @brief remove all the analysisData
  */
  void clear();
  
  /**
   * @brief remove the analysisData with the given id without destructing it
   * @deprecated This was useful when data was raw pointers to transfer ownership. Now that data are shared_pointers, the reference counting mechanism handles the destruction when necessary.
   * @param id the identifier of the AnalysisData to release
   */
  void releaseData(const std::string& id);

private:
  AnalysisContentPrivate* m_d;
  
};

} // Lima

#endif
