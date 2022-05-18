// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
