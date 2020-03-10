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
/** @brief      xml logger for coreferences
  *
  * @file       CorefSolvingXmlLogger.h
  * @author     Claire Mouton <claire.mouton@cea.fr>
  *             Copyright (c) 2007 by CEA
  * @version    $Id: CorefSolvingXmlLogger.h 2007-05-29 claire $
  */

#ifndef LIMA_LINGUISTICPROCESSING_CSXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_CSXMLLOGGER_H

#include "CorefSolvingExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace Coreferences
{

#define COREFSOLVINGXMLLOGGER_CLASSID "CorefSolvingLogger"


class LIMA_COREFSOLVING_EXPORT DumpXMLAnnotationVisitor : public boost::default_bfs_visitor
{
  std::ostream& m_ostream;
//   SyntacticAnalysis::SyntacticData* m_sd;
  Common::AnnotationGraphs::AnnotationData* m_ad;
  MediaId m_language;
  LinguisticGraphEdge m_lastEdge;
  std::string m_memo;
//   std::set<LinguisticGraphVertex> m_alreadyProcessedVertexs;
//   std::set<LinguisticGraphVertex> m_alreadyProcessedTargets;


public:
  DumpXMLAnnotationVisitor(std::ostream& os,
//   SyntacticAnalysis::SyntacticData* sd,
  Common::AnnotationGraphs::AnnotationData* ad,
  MediaId language):
  m_ostream(os),/*m_sd(sd),*/m_ad(ad),m_language(language), m_lastEdge(LinguisticGraphEdge()),m_memo("") /*m_alreadyProcessedVertexs(),m_alreadyProcessedTargets()*/{}



  void examine_edge(LinguisticGraphEdge e,
                    const LinguisticGraph& g);

  bool are_equivalent(
      LinguisticGraphEdge currentEdge,
      LinguisticGraphVertex vProcessed,
      LinguisticGraphVertex vNotProcessed,
      const LinguisticGraph& g);

//   void discover_vertex(DependencyGraphVertex v,
//                                      const DependencyGraph& g);
};


/** @brief A logger process unit to output coreference solving results in XML format
 * The process unit configuration parameters are:
 * - outputSuffix: the suffix to add to the analyzed file name. It will be completed by ".xml". Default is: ".coref"
 *
 * The output format is an XML with the analyzed text and tags identifying coreferences. Tags format is:
 *  <COREF ID="id" TYPE="coref_type" REF="ref_id" CATEG="categ">leur</COREF>
 * id value is ?
 * coref_type possible values are : IDENT, ???
 * ref_id is the id of the referred entity, can be absent if the current tag is the referred entity
 * categ possible values are : other, reflPron, undef, def, ???
 */
class LIMA_COREFSOLVING_EXPORT CorefSolvingXmlLogger : public AbstractLinguisticLogger
{

public:

  CorefSolvingXmlLogger();
  virtual ~CorefSolvingXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

  std::string m_outputSuffix;
  std::string m_graphId;
  MediaId m_language;
//   std::set<DependencyGraphVertex> m_alreadyProcessedVertexs;
//   std::set<DependencyGraphVertex> m_alreadyProcessedTargets;
  DependencyGraphEdge m_lastEdge;
  std::string m_memo;

  void dump(
    std::ostream& fileName,
    LinguisticAnalysisStructure::AnalysisGraph* g,/*
    SyntacticAnalysis::SyntacticData* sd,*/
    Common::AnnotationGraphs::AnnotationData* ad) const;

};


} // Coreferences
} // LinguisticProcessing
} // Lima

#endif
