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
 * @file       EventTemplateElement.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Sep  1 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
 * 
 * @brief      element of an event template, defined by an entity occurrence
 * 
 * 
 ***********************************************************************/

#ifndef EVENTTEMPLATEELEMENT_H
#define EVENTTEMPLATEELEMENT_H

#include "EventAnalysisExport.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

class LIMA_EVENTANALISYS_EXPORT EventTemplateElement
{
 public:
   EventTemplateElement(); 
   EventTemplateElement(LinguisticGraphVertex v, 
                        const LinguisticAnalysisStructure::AnalysisGraph* graph, 
                        const Common::MediaticData::EntityType& type); 
   ~EventTemplateElement();
   
   const Common::MediaticData::EntityType&  getType() const { return m_type; }
   const LinguisticAnalysisStructure::AnalysisGraph* getGraph() const { return m_graph; }
   LinguisticGraphVertex getVertex() const { return m_vertex; }
   LinguisticAnalysisStructure::Token* getToken() const;
   uint64_t getPositionBegin() const;
   uint64_t getPositionEnd() const;

   //comparison function: do not use equality operator because it's not strict equality
   bool isSimilar(const EventTemplateElement& elt) const;

   LIMA_EVENTANALISYS_EXPORT  friend std::ostream& operator<<(std::ostream& os, const EventTemplateElement& elt);
   LIMA_EVENTANALISYS_EXPORT  friend QDebug& operator<<(QDebug& os, const EventTemplateElement& elt);
   
 private:
  Common::MediaticData::EntityType m_type;
  const LinguisticAnalysisStructure::AnalysisGraph* m_graph;
  LinguisticGraphVertex m_vertex;
};

} // end namespace
} // end namespace
} // end namespace

#endif
