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
 * @file       entityTransition.cpp
 * @author     Romaric Besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jul 28 2006
 * copyright   (c) 2006 by CEA
 *
 *************************************************************************/


#include "entityTransition.h"
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
LimaString EntityTransition::m_entityAnnotation=Common::Misc::utf8stdstring2limastring("SpecificEntity");

/***********************************************************************/
// constructors
/***********************************************************************/
EntityTransition::EntityTransition():
TransitionUnit(),
m_entityType() 
{
}

EntityTransition::EntityTransition(Common::MediaticData::EntityType type, bool keep):
TransitionUnit(keep),
m_entityType(type) 
{
}

EntityTransition::~EntityTransition() {}

std::string EntityTransition::printValue() const {
  ostringstream oss;
  oss << "ENTITY_" << m_entityType;
  return oss.str();
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool EntityTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())
       && (m_entityType == static_cast<const EntityTransition&>(tright).entityType())
       ) {
    return compareProperties(tright);
  }
  else {
    return false;
  }
}

bool EntityTransition::
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
    LDEBUG << "no annotation graph available !" << LENDL;
    return false;
  }

  // find annotationGraphVertex matching the vertex of the current graph
  std::set<AnnotationGraphVertex> matches = annotationData->matches(graph.getGraphId(), v, "annot");
  if (matches.empty())
  {
    AULOGINIT;
    LERROR << "EntityTransition::compare: No annotation ("<<graph.getGraphId()<<", "<<v<<", \"annot\") available";
    return false;
  }
  AnnotationGraphVertex annotVertex = *(matches.begin());

  if (!annotationData->hasAnnotation(annotVertex, m_entityAnnotation))
  {
    return false;
  }
  
  const SpecificEntityAnnotation* se =
    annotationData->annotation(annotVertex, m_entityAnnotation).
    pointerValue<SpecificEntityAnnotation>();

  return (m_entityType == se->getType());
}

} // namespace end
} // namespace end
} // namespace end
