// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  auto eventData = std::dynamic_pointer_cast<EventTemplateData>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    eventData = std::make_shared<EventTemplateData>();
    analysis.setData("EventTemplateData", eventData);
  }
  // get the cardinality for the role in the template definition
  unsigned int cardinality(1);
  auto defData = std::dynamic_pointer_cast<EventTemplateDefinitionData>(analysis.getData("EventTemplateFillingTemplateDefinition"));
  if (defData==0) {
    LDEBUG << "AddTemplateElement: no template definition provided";
  }
  else {
    cardinality=defData->resource->getCardinality(m_role);
  }
  // !!!! TODO: add cardinality as argument for addElementInCurrentTemplate !!!
    
  if (! m_type.isNull()) {
    EventTemplateElement elt(v,&graph,m_type);
    LDEBUG << "AddTemplateElement("<< m_type <<"): add " << elt << " as " << m_role << ", cardinality =" << cardinality;
    eventData->addElementInCurrentTemplate(m_role,elt,cardinality);
  }
  else {
    //get type from specific entity associated to the vertex
    const auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
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
        LDEBUG << "AddTemplateElement: add " << elt << " as " << m_role << ", cardinality =" << cardinality;
        eventData->addElementInCurrentTemplate(m_role,elt,cardinality);
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
Automaton::ConstraintFunction(language,complement),
m_eventType()
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "Complement " << complement;
  m_eventType=Common::Misc::limastring2utf8stdstring(complement);
  LDEBUG << "m_event_type " << m_eventType;
}

bool CreateEventTemplate::operator()(AnalysisContent& analysis) const
{
  auto eventData = std::dynamic_pointer_cast<EventTemplateData>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    LOGINIT("LP::EventAnalysis");
    LERROR << "CreateEventTemplate: Missing data EventTemplateData";
    return false;
  }
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "CreateEventTemplate";
  
  // validate current template by creating a new empty template which will be new current template
  LDEBUG << "setTypeInCurrentTemplate" << m_eventType;
  eventData->setTypeInCurrentTemplate(m_eventType);

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
  auto eventData = std::dynamic_pointer_cast<EventTemplateData>(analysis.getData("EventTemplateData"));
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
