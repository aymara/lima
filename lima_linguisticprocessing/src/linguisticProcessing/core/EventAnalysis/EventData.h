// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventData.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Mar 25 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     
 * 
 * @brief a class containing the analysis data to store event
 * information
 * 
 * 
 ***********************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_EVENTDATA_H
#define LIMA_LINGUISTICPROCESSING_EVENTDATA_H

#include "EventAnalysisExport.h"
#include "common/MediaticData/mediaticData.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "Entity.h"
#include <vector>

#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

//class Lima::LinguisticProcessing::SegmentationData;

namespace Lima {
namespace LinguisticProcessing{
namespace EventAnalysis {

class Events;
class Event;
class EventParagraph;

class LIMA_EVENTANALISYS_EXPORT EventData : public AnalysisData
{
public:
  EventData();
  ~EventData();

  void addEntity(const Common::MediaticData::EntityType& type, const Entity& e);
  std::map<Common::MediaticData::EntityType,std::vector<Entity> >& getEntities() { return m_entities; }
  const std::map<Common::MediaticData::EntityType,std::vector<Entity> >& getEntities() const { return m_entities; }
  
  std::vector<Common::MediaticData::EntityType> getEntityTypes() const;
  const Entity& getMainEntity(const Common::MediaticData::EntityType& type) const;
  Events* convertToEvents(const Lima::LinguisticProcessing::SegmentationData* segmData) const;
  EventParagraph* locate( Event* event, const Entity *entity ) const;

private:
  std::map<Common::MediaticData::EntityType,std::vector<Entity> > m_entities;
};


} //EventAnalysis
} // LinguisticProcessing
} // Lima

#endif
