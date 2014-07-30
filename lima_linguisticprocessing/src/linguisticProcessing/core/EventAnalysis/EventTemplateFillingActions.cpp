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
/************************************************************************
 *
 * @file       EventTemplateFillingActions.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Sep 01 2011
 * copyright   Copyright (C) 2011 by CEA - LIST
 *
 ***********************************************************************/

#include "EventTemplateFillingActions.h"
#include "EventTemplateData.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//----------------------------------------------------------------------
// factories for action function
ConstraintFunctionFactory<AddTemplateElement>
AddTemplateElementFactory(AddTemplateElementId);

ConstraintFunctionFactory<CreateEventTemplate>
CreateEventTemplateFactory(CreateEventTemplateId);

ConstraintFunctionFactory<ClearEventTemplate>
ClearEventTemplateFactory(ClearEventTemplateId);

//----------------------------------------------------------------------
AddTemplateElement::AddTemplateElement(MediaId language,
                                       const LimaString& complement):
Automaton::ConstraintFunction(language,complement),
m_role(),
m_type()
{
  int i=complement.indexOf(Common::Misc::utf8stdstring2limastring(","));
  if (i==-1) {
    m_role=Common::Misc::limastring2utf8stdstring(complement);
  }
  else {
    m_role=Common::Misc::limastring2utf8stdstring(complement.left(i));
    m_type=Common::MediaticData::MediaticData::single().getEntityType(complement.mid(i+1));
  }
}

bool AddTemplateElement::operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                                    const LinguisticGraphVertex& v,
                                    AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    eventData = new EventTemplateData();
    analysis.setData("EventTemplateData", eventData);
  }
  if (! m_type.isNull()) {
    EventTemplateElement elt(v,&graph,m_type);
    LDEBUG << "AddTemplateElement: add " << elt << " as " << m_role;
    eventData->addElementInCurrentTemplate(m_role,elt);
  }
  else {
    //get type from specific entity associated to the vertex
    const AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
    if (annotationData==0)
    {
      LOGINIT("LP::EventAnalysis");
      LERROR << "AddTemplateElement: no annotation graph available !";
      return false;
    }
    // check if vertex corresponds to a specific entity found 
    std::set< AnnotationGraphVertex > matches = annotationData->matches(graph.getGraphId(),v,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
    it != matches.end(); it++)
    {
      AnnotationGraphVertex vx=*it;
      if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        const SpecificEntityAnnotation* se =
        annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
        pointerValue<SpecificEntityAnnotation>();

        EventTemplateElement elt(v,&graph,se->getType());
        LDEBUG << "AddTemplateElement: add " << elt << " as " << m_role;
        eventData->addElementInCurrentTemplate(m_role,elt);
      }
    }
  }
  return true;
}

bool AddTemplateElement::operator()(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
                                    const LinguisticGraphVertex& /*v1*/,
                                    const LinguisticGraphVertex& /*v2*/,
                                    AnalysisContent& /*analysis*/) const
{
  LOGINIT("LP::EventAnalysis");
  LERROR << "Calling constraint AddTemplateElement with two vertices: not implemented yet";
  return true;
}

//----------------------------------------------------------------------
CreateEventTemplate::CreateEventTemplate(MediaId language,
                                         const LimaString& complement):
Automaton::ConstraintFunction(language,complement)
{
}

bool CreateEventTemplate::operator()(AnalysisContent& analysis) const
{
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "CreateEventTemplate: Missing data EventTemplateData";
    return false;
  }
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "CreateEventTemplate";
  
  // validate current template by creating a new empty template which will be new current template
  eventData->addTemplate();
  return true;
}

//----------------------------------------------------------------------
ClearEventTemplate::ClearEventTemplate(MediaId language,
                                       const LimaString& complement):
Automaton::ConstraintFunction(language,complement)
{
}

bool ClearEventTemplate::operator()(AnalysisContent& analysis) const
{
  EventTemplateData* eventData=static_cast<EventTemplateData*>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "CreateEventTemplate: Missing data EventTemplateData";
    return false;
  }
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "ClearEventTemplate";
  eventData->clearCurrentTemplate();
  return true;
}




} // end namespace
} // end namespace
} // end namespace
