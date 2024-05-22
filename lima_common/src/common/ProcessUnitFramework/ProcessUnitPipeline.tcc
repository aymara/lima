// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

namespace Lima
{

template <typename ProcessUnit>
ProcessUnitPipeline<ProcessUnit>::ProcessUnitPipeline() :
    m_processUnitSequence()
{}

template <typename ProcessUnit>
ProcessUnitPipeline<ProcessUnit>::~ProcessUnitPipeline() {}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    typename ProcessUnit::Manager* manager)
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LINFO << "init pipeline '" << this->getId().c_str() << "'";
  try
  {
    auto sequence = unitConfiguration.getListsValueAtKey("processUnitSequence");
    for (const auto& puid: sequence)
    {
      LINFO << "get processUnit '" << puid.c_str() << "'";
      auto pu = manager->getObject(puid);
      m_processUnitSequence.push_back(pu);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ex)
  {
    LERROR << "No 'processUnitSequence' list defined in pipeline '"
            << this->getId().c_str() << "':" << ex.what();
    throw InvalidConfiguration();
  }
}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::push_back(
    const std::string& unitId,
    typename ProcessUnit::Manager* manager)
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LINFO << "push back unit '" << unitId.c_str() << "'";
  auto pu = manager->getObject(unitId);
  m_processUnitSequence.push_back(pu);
}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::push_back(std::shared_ptr<ProcessUnit> pu)
{
  m_processUnitSequence.push_back(pu);
}

template <typename ProcessUnit>
LimaStatusCode ProcessUnitPipeline<ProcessUnit>::process(
    AnalysisContent& analysis) const
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LINFO << "process pipeline '" << this->getId().c_str() << "'";
  LimaStatusCode status=SUCCESS_ID;

  debugPrintInactiveUnits();

  auto dataInactiveProcessUnits = std::dynamic_pointer_cast<InactiveUnitsData>(analysis.getData("InactiveUnits"));
  bool hasInactiveProcessUnits = !m_inactiveUnitsIds.empty()
                                  || (dataInactiveProcessUnits != 0
                                      && dataInactiveProcessUnits->size() != 0) ;
  for (auto it = m_processUnitSequence.begin();
       (status==SUCCESS_ID) && (it!=m_processUnitSequence.end());
       it++)
  {
    LINFO << "- process '" << (*it)->getId().c_str() << "'";
    if ( !hasInactiveProcessUnits
        || m_inactiveUnitsIds.find((*it)->getId()) == m_inactiveUnitsIds.end()
        || (dataInactiveProcessUnits != 0
            && dataInactiveProcessUnits->find((*it)->getId()) != dataInactiveProcessUnits->end()) )
    {
      LINFO << "    processing";
      status=(*it)->process(analysis);
      LINFO << "return status " << status;
    }
  }
  LINFO << "processing of pipeline return status " << status;
  return status;
}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::setActiveProcessUnit(
    const std::string& processUnitId)
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LWARN << "DEPRECATED ProcessUnitPipeline<ProcessUnit>::setActiveProcessUnit. See ProcessUnitPipeline.h";
  if (m_inactiveUnitsIds.find(processUnitId)!=m_inactiveUnitsIds.end())
  {
    m_inactiveUnitsIds.erase(m_inactiveUnitsIds.find(processUnitId));
  }
}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::setInactiveProcessUnit(
    const std::string& processUnitId)
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LWARN << "DEPRECATED ProcessUnitPipeline<ProcessUnit>::setInactiveProcessUnit. See ProcessUnitPipeline.h";
  m_inactiveUnitsIds.insert(processUnitId);
}

template <typename ProcessUnit>
void ProcessUnitPipeline<ProcessUnit>::debugPrintInactiveUnits() const
{
  PROCESSUNITFRAMEWORKLOGINIT;
  LWARN << "DEPRECATED ProcessUnitPipeline<ProcessUnit>::debugPrintInactiveUnits. See ProcessUnitPipeline.h";

  std::set<std::string>::const_iterator it, it_end;
  it = m_inactiveUnitsIds.begin();
  it_end = m_inactiveUnitsIds.end();

  for (;it!=it_end;it++)
  {
    LDEBUG << "  " << (*it).c_str();
  }

}

} // Lima
