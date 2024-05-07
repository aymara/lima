// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_PROCESSUNITFRAMEWORK_PROCESSUNITPIPELINE_H
#define LIMA_COMMON_PROCESSUNITFRAMEWORK_PROCESSUNITPIPELINE_H

#include "common/ProcessUnitFramework/AnalysisContent.h"
#include <list>
#include <set>

namespace Lima
{


class InactiveUnitsData : public std::set<std::string>, public AnalysisData
{
public:
    InactiveUnitsData() {}
    virtual ~InactiveUnitsData() {}
};

/**
@author Benoit Mathieu
*/
template <typename ProcessUnit>
class ProcessUnitPipeline : public ProcessUnit
{
public:
  ProcessUnitPipeline();
  virtual ~ProcessUnitPipeline();

  /**
  * @brief initialize all sub processunits with parameters from configuration file.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @throw InvalidConfiguration when parameters are invalids.
  * @throw UndefinedMethod if method is not implemented
  */
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    typename ProcessUnit::Manager* manager) override;

  /**
    * @brief Sequentially apply each processUnit on data in analysisContent.
    * This method should not return any exception but UndefinedMethod
    * @param analysis AnalysisContent object on which to process
    */
  virtual LimaStatusCode process(
    AnalysisContent& analysis) const override;

  /// @deprecated the inactive units must be sent as an AnalysisData in the AnalysisContent to allow various inactive units in various threads
  void setActiveProcessUnit(const std::string& processUnitId);
  /// @deprecated the inactive units must be sent as an AnalysisData in the AnalysisContent to allow various inactive units in various threads
  void setInactiveProcessUnit(const std::string& processUnitId);

  void push_back(
    const std::string& unitId,
    typename ProcessUnit::Manager* manager);

  void push_back(std::shared_ptr<ProcessUnit> pu);

private:
  void debugPrintInactiveUnits() const;

  typedef typename std::list<std::shared_ptr<ProcessUnit>> ProcessUnitSequence;
  typedef typename std::list<std::shared_ptr<ProcessUnit>>::const_iterator ProcessUnitSequenceCItr;
  ProcessUnitSequence m_processUnitSequence;

  /// @deprecated the inactive units must be sent as an AnalysisData in the AnalysisContent to allow various inactive units in various threads
  std::set<std::string> m_inactiveUnitsIds;
};

} // Lima

#include "ProcessUnitPipeline.tcc"

#endif
