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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef SPECIFICENTITIESXMLLOGGERSPECIFICENTITIESXMLLOGGER_H
#define SPECIFICENTITIESXMLLOGGERSPECIFICENTITIESXMLLOGGER_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SpecificEntities
{

/**
@author Benoit Mathieu
*/
#define SPECIFICENTITIESXMLLOGGER_CLASSID "SpecificEntitiesXmlLogger"

class LIMA_SPECIFICENTITIES_EXPORT SpecificEntitiesXmlLogger : public AbstractTextualAnalysisDumper
{
public:
  SpecificEntitiesXmlLogger();

  virtual ~SpecificEntitiesXmlLogger() = default;

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  bool m_compactFormat; // compact format is the same as the RecognizerResultLogger
  MediaId m_language;
  std::string m_graph;
  bool m_followGraph;

  //member private members
  const SpecificEntityAnnotation*
  getSpecificEntityAnnotation(LinguisticGraphVertex v,
                              const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  void outputEntity(Common::AnnotationGraphs::AnnotationData* annotationData,
                    std::ostream& out,
                    LinguisticGraphVertex v,
                    const SpecificEntityAnnotation* annot,
                    const VertexTokenPropertyMap& tokenMap,
                    uint64_t offset) const;

};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif
