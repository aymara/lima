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
 * @file       EventTemplateFilling.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Thu Sep 01 2011
 * copyright   Copyright (C) 2011 by CEA - LIST 
 * Project     LIMA
 * 
 * @brief 
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEFILLING_H
#define LIMA_LINGUISTICPROCESSING_EVENTTEMPLATEFILLING_H

#include "EventAnalysisExport.h"
#include "EventTemplateStructure.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

//**********************************************************************
// action function for entity template filling in rules
#define AddTemplateElementId "AddTemplateElement"
#define CreateEventTemplateId "CreateEventTemplate"
#define ClearEventTemplateId "ClearEventTemplate"

class LIMA_EVENTANALISYS_EXPORT AddTemplateElement : public Automaton::ConstraintFunction
{
public:
  AddTemplateElement(MediaId language,
                     const LimaString& complement);
  ~AddTemplateElement() {}
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v,
                  AnalysisContent& analysis) const override;
  bool operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                  const LinguisticGraphVertex& v1,
                  const LinguisticGraphVertex& v2,
                  AnalysisContent& analysis) const override;
private:
  std::string m_role;
  Common::MediaticData::EntityType m_type;
};

class LIMA_EVENTANALISYS_EXPORT CreateEventTemplate : public Automaton::ConstraintFunction
{
public:
  CreateEventTemplate(MediaId language,
                      const LimaString& complement);
  ~CreateEventTemplate() {}
  bool operator()(AnalysisContent& analysis) const override;
  
private:
  std::string m_eventType;
};

class LIMA_EVENTANALISYS_EXPORT ClearEventTemplate : public Automaton::ConstraintFunction
{
public:
  ClearEventTemplate(MediaId language,
                      const LimaString& complement);
  ~ClearEventTemplate() {}
  bool operator()(AnalysisContent& analysis) const override;
  
  //bool actionNeedsRecognizedExpression() { return true; }
private:
};

} // end namespace
} // end namespace
} // end namespace

#endif
