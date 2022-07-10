// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESCOMPLETION_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESCOMPLETION_H

#include "SpecificEntitiesConstraints.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaticData/EntityType.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define SPECIFICENTITIESCOMPLETION_CLASSID "SpecificEntitiesCompletion"

/**
* @brief complete specific entities recognizer: search all occurrences of already found entities (e.g. find a person name because of M. before and complete recognition by annotating other occurrences of the same person name)
*/
class SpecificEntitiesCompletion : public MediaProcessUnit
{
public:
  SpecificEntitiesCompletion();

  virtual ~SpecificEntitiesCompletion();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(AnalysisContent& analysis) const;

private:
  MediaId m_language;
  std::string m_graph;
  std::set<Lima::Common::MediaticData::EntityType> m_entityTypes;
  
  // private utility classes and functions
  class Entities;
  class EntityOccurrence;
  void getEntities(AnalysisContent& analysis, Entities& foundEntities,
                   const VertexTokenPropertyMap& tokenMap) const;
  void findOccurrences(Entities& foundEntities,
                       AnalysisContent& analysis,
                       std::vector<EntityOccurrence>& newEntities) const;
  void updateAnalysis(std::vector<EntityOccurrence>& entities,
                      AnalysisContent& analysis) const;
  void processVertex(LinguisticGraphVertex currentVertex, 
                     const VertexTokenPropertyMap& tokenMap,
                     std::vector<EntityOccurrence>& occurrences,
                     int& currentOccurrence,
                     Automaton::RecognizerMatch& currentMatch,
                     std::map<Common::MediaticData::EntityType,std::unique_ptr<CreateSpecificEntity> >& entityCreator,
                     LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                     AnalysisContent& analysis) const;
};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif
