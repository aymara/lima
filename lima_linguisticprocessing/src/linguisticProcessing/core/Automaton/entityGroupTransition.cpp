// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*************************************************************************
 *
 * @file       entityGroupTransition.cpp
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon oct 5 2015
 * copyright   (c) 2006-2015 by CEA
 *
 *************************************************************************/


#include "entityGroupTransition.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// initialization of static members
LimaString EntityGroupTransition::m_entityAnnotation=Common::Misc::utf8stdstring2limastring("SpecificEntity");

/***********************************************************************/
// constructors
/***********************************************************************/
EntityGroupTransition::EntityGroupTransition():
TransitionUnit(),
m_entityGroupId() 
{
}

EntityGroupTransition::EntityGroupTransition(Common::MediaticData::EntityGroupId groupId, bool keep):
TransitionUnit(keep),
m_entityGroupId(groupId) 
{
}

EntityGroupTransition::~EntityGroupTransition() {}

std::string EntityGroupTransition::printValue() const {
  ostringstream oss;
  oss << "entityGroupT_" << m_entityGroupId;
  return oss.str();
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool EntityGroupTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())
       && (m_entityGroupId == static_cast<const EntityGroupTransition&>(tright).entityGroupId())
       ) {
    return true;
  }
  else {
    return false;
  }
}

bool EntityGroupTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& v,
        AnalysisContent& analysis,
        const LinguisticAnalysisStructure::Token* /*token*/,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  // should compare to vertex ?
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0) {
    AULOGINIT;
    LDEBUG << "EntityGroupTransition::compare: no annotation graph available !";
    return false;
  }

  // find annotationGraphVertex matching the vertex of the current graph
  std::set<AnnotationGraphVertex> matches = annotationData->matches(graph.getGraphId(), v, "annot");
  if (matches.empty())
  {
    AULOGINIT;
    LDEBUG << "annotation ("<<graph.getGraphId()<<", "<<v<<", \"annot\") available";
    return false;
  }
  AnnotationGraphVertex annotVertex = *(matches.begin());

  if (!annotationData->hasAnnotation(annotVertex, m_entityAnnotation))
  {
    AULOGINIT;
    LDEBUG << "EntityGroupTransition::compare: No " << m_entityAnnotation << " annotation available on " << v;
    return false;
  }
  
  const SpecificEntityAnnotation* se =
    annotationData->annotation(annotVertex, m_entityAnnotation).
    pointerValue<SpecificEntityAnnotation>();
  Common::MediaticData::EntityType type = se->getType();
  AULOGINIT;
  LDEBUG << "EntityGroupTransition::compare: type = " << type << ", groupId = " << type.getGroupId();
  LDEBUG << "EntityGroupTransition::compare: m_entityGroupId = " << m_entityGroupId;
  LDEBUG << "EntityGroupTransition::compare: tests m_entityGroupId == type.getGroupId() = " << (m_entityGroupId == type.getGroupId());
  return( m_entityGroupId == type.getGroupId() );
}

} // namespace end
} // namespace end
} // namespace end
