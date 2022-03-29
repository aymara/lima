/*
    Copyright 2002-2021 CEA LIST

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
