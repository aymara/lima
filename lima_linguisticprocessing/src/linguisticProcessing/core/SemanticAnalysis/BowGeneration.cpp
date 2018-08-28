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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "BowGeneration.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/genericDocumentProperties.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/BoWPredicate.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/Compounds/CompoundTokenAnnotation.h"
#include "linguisticProcessing/core/Compounds/CompoundTokenAnnotation.h"
#include "linguisticProcessing/core/AnalysisDumpers/BowGeneration.h"
#include "linguisticProcessing/core/SemanticAnalysis/SemanticRelationAnnotation.h"

#include <boost/graph/properties.hpp>

#include <fstream>
#include <deque>
#include <iostream>
#include <QStringList>


using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
// using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDumpers;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;

namespace Lima
{

namespace LinguisticProcessing
{

namespace Compounds
{


boost::shared_ptr< BoWPredicate > createPredicate(
    BowGenerator* generator,
    const LinguisticGraphVertex& lgvs,
    const AnnotationGraphVertex& agvs,
    const AnnotationGraphVertex& agvt,
    const SemanticRelationAnnotation& annot ,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    uint64_t offset,
    std::set< LinguisticGraphVertex >& visited,
    bool keepAnyway)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createPredicate " << lgvs << agvs 
          << agvt << annot.type().c_str();
#endif
  boost::shared_ptr< BoWPredicate > bowP( new BoWPredicate() );

  EntityType predicateEntity = MediaticData::single().getEntityType(
    QString::fromUtf8(annot.type().c_str()));
  bowP->setPredicateType(predicateEntity);

  bowP->setPosition(0);
  bowP->setLength(0);

  std::vector<AnnotationGraphVertex> vertices;
  vertices.push_back(agvs);
  vertices.push_back(agvt);
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::createPredicate  The role(s) related to "
          << annot.type().c_str() << " is/are ";
#endif
  QMultiMap<Common::MediaticData::EntityType, 
            boost::shared_ptr< AbstractBoWElement > > roles;
//   const LimaString typeAnnot="SemanticRole";
  for (auto verticesIt = vertices.begin(); 
       verticesIt != vertices.end(); 
       verticesIt++)
  {
    const AnnotationGraphVertex semRoleVx = *verticesIt;
    std::set< LinguisticGraphVertex > anaGraphSemRoleVertices = 
        annotationData->matches("annot", semRoleVx,  "AnalysisGraph");
    if (!anaGraphSemRoleVertices.empty())
    {
      auto anaGraphSemRoleVertex = *anaGraphSemRoleVertices.begin();
      auto posGraphSemRoleVertices = annotationData->matches(
          "AnalysisGraph", anaGraphSemRoleVertex,  "PosGraph");
      if (!posGraphSemRoleVertices.empty())
      {
        auto posGraphSemRoleVertex = *(posGraphSemRoleVertices.begin());
        if (posGraphSemRoleVertex == lgvs)
        {
#ifdef DEBUG_LP
          LERROR << "BowGenerator::createPredicate role vertex is the same as"
                  << "the trigger vertex. Abort this role.";
#endif
          continue;
        }
#ifdef DEBUG_LP
        LDEBUG << "BowGenerator::createPredicate Calling"
                << "createAbstractBoWElement on PoS graph vertex" 
                << posGraphSemRoleVertex;
#endif
        auto semRoleTokens = generator->createAbstractBoWElement(
                                posGraphSemRoleVertex, 
                                anagraph,
                                posgraph, 
                                offset, 
                                annotationData, 
                                visited, 
                                keepAnyway);
#ifdef DEBUG_LP
        LDEBUG << "BowGenerator::createPredicate Created "
                << semRoleTokens.size()
                << "token for the role associated to " 
                << annot.type().c_str();
#endif
        if (!semRoleTokens.empty())
        {
          EntityType semRoleEntity;
          roles.insert(semRoleEntity, semRoleTokens[0].second);
        }
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "BowGenerator::createPredicate Found no" 
                << "matching for the semRole in the annot graph";
#endif
      }
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::createPredicate Created a Predicate for the semantic relation"
          << predicateEntity 
          << MediaticData::single().getEntityName(predicateEntity);
#endif
  if (!roles.empty())
  {
    bowP->setRoles(roles);
#ifdef DEBUG_LP
    for (auto it = roles.begin(); it != roles.end(); it++)
    {
      auto outputRoles = boost::dynamic_pointer_cast<BoWToken>(it.value());
      if (outputRoles != 0)
      {
        LimaString roleLabel = it.key().isNull() ? QString() : Common::MediaticData::MediaticData::single().getEntityName(it.key());
        LDEBUG << "BowGenerator::createPredicate Associated "
                << QString::fromUtf8(outputRoles->getOutputUTF8String().c_str()) 
                << " to it" << "via the semantic role label "<< roleLabel ;
      }
    }
#endif
  }
  return bowP;
}


} // AnalysisDumper

} // LinguisticProcessing

} // Lima
