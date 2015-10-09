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
  oss << "ENTITY_GROUP_" << m_entityGroupId;
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
    LERROR << "EntityGroupTransition::compare: No annotation ("<<graph.getGraphId()<<", "<<v<<", \"annot\") available";
    return false;
  }
  AnnotationGraphVertex annotVertex = *(matches.begin());

  if (!annotationData->hasAnnotation(annotVertex, m_entityAnnotation))
  {
    AULOGINIT;
    LERROR << "EntityGroupTransition::compare: No " << m_entityAnnotation << " annotation available on " << v;
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
