// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSBOWDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSBOWDUMPER_H

#include "AnalysisDumpersExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "StopList.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{
  class AnnotationData;
}
namespace BagOfWords
{
  class BoWNamedEntity;
}
}
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{
  class SyntacticData;
}
namespace Compounds
{
  class BowGenerator;
}
namespace AnalysisDumpers
{

#define BOWDUMPER_CLASSID "BowDumper"

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDUMPERS_EXPORT BowDumper : public AbstractTextualAnalysisDumper
{
public:
  BowDumper();

  virtual ~BowDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:
  Compounds::BowGenerator* m_bowGenerator;
  std::string m_graph;
  bool m_allEntities;
  bool m_xml; // if true, output is not binary but text, in XML format

private:
  void buildBoWText(
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const SyntacticAnalysis::SyntacticData* syntacticData,
    Common::BagOfWords::BoWText& bowText,
    AnalysisContent& analysis,
    LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticAnalysisStructure::AnalysisGraph* posgraph,
    std::set<LinguisticGraphVertex>& addedEntities,
    const uint64_t offset) const;

  void addVerticesToBoWText(
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticAnalysisStructure::AnalysisGraph* posgraph,
    const SyntacticAnalysis::SyntacticData* syntacticData,
    const LinguisticGraphVertex begin,
    const LinguisticGraphVertex end,
    const uint64_t offset,
    Common::BagOfWords::BoWText& bowText,
    std::set<LinguisticGraphVertex>& addedEntities) const;

    void addAllEntities(
      const Common::AnnotationGraphs::AnnotationData* annotationData,
      const std::set<LinguisticGraphVertex>& addedEntities,
      Common::BagOfWords::BoWText& bowText,
      LinguisticAnalysisStructure::AnalysisGraph* anagraph,
      LinguisticAnalysisStructure::AnalysisGraph* posgraph,
      const uint64_t offset=0) const;

};

} // AnalysisDumpers

} // LinguisticProcessing

} // Lima

#endif
