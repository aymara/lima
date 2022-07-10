// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       EventTemplateElement.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "EventTemplateElement.h"
#include "common/Data/strwstrtools.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

EventTemplateElement::EventTemplateElement():
m_type(),
m_graph(0),
m_vertex(0)
{
}

EventTemplateElement::EventTemplateElement(LinguisticGraphVertex v, 
                                           const LinguisticAnalysisStructure::AnalysisGraph* graph, 
                                           const Common::MediaticData::EntityType& type):
m_type(type),
m_graph(graph),
m_vertex(v)
{
}

 
EventTemplateElement::~EventTemplateElement() {
}

LinguisticAnalysisStructure::Token* EventTemplateElement::getToken() const
{
  LinguisticAnalysisStructure::Token* tok=get(vertex_token, *(m_graph->getGraph()), m_vertex);
  return tok;
}

uint64_t EventTemplateElement::getPositionBegin() const
{
  LinguisticAnalysisStructure::Token* tok=getToken();
  if (tok) { 
    return tok->position();
  }
  return 0;
}

uint64_t EventTemplateElement::getPositionEnd() const
{
  LinguisticAnalysisStructure::Token* tok=getToken();
  if (tok) { 
    return tok->position()+tok->length();
  }
  return 0;
}


bool EventTemplateElement::isSimilar(const EventTemplateElement& elt) const
{
  LinguisticAnalysisStructure::Token* tok=getToken();
  LinguisticAnalysisStructure::Token* otherTok=elt.getToken();
  // for the moment, simple equality of string
  return (tok->stringForm()==otherTok->stringForm());
}


// output utility
std::ostream& operator<<(std::ostream& os, const EventTemplateElement& elt)
{
  LinguisticAnalysisStructure::Token* tok=elt.getToken();
  if (tok!=0) {
    os << Common::Misc::limastring2utf8stdstring(tok->stringForm());
  }
  return os;
}
QDebug& operator<<(QDebug& os, const EventTemplateElement& elt)
{
  LinguisticAnalysisStructure::Token* tok=elt.getToken();
  if (tok!=0) {
    os << tok->stringForm();
  }
  return os;
}



} // end namespace
} // end namespace
} // end namespace
