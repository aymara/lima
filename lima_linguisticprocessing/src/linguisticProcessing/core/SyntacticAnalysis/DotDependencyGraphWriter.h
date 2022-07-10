// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @brief      output of dependency graph with dot format
 *
 * @file       DotDependencyGraphWriter.h
 * @author     besancon (besanconr@zoe.cea.fr) 

 *             copyright   Copyright (C) 2004 by CEA LIST
 * @date       Thu Oct 21 2004
 * @version    $Id$
 */

#ifndef DEPENDENCYGRAPHWRITER_H
#define DEPENDENCYGRAPHWRITER_H

#include "SyntacticAnalysisExport.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/PosTagger/dotGraphWriter.h"
#include "linguisticProcessing/core/PosTagger/posTaggingGraphWriter.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima {

namespace Common {
namespace AnnotationGraphs {
  class AnnotationData;
}
}
  
namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {
  class AnalysisGraph;
}

namespace SyntacticAnalysis {

class SyntacticData;

#define DOTDEPENDENCYGRAPHWRITER_CLASSID "DotDependencyGraphWriter"

/** @brief This class is a process units that dumps to file a dependency graph
 * in the GraphViz dot file format.
 */
class LIMA_SYNTACTICANALYSIS_EXPORT DotDependencyGraphWriter : public DotGraphWriter
{
 public:
   /** This enum defines the output modes. The values are read in the 
    * outputMode parameter of the writer config group in the configuration 
    * file. Possible values are FullGraph to display all the analyzed text 
    * graph in one graph file and SentenceBySentence to display each sentence
    * in a separate graph file.
    */
  enum DependencyGraphOutputMode 
  {
    SentenceBySentence,
    FullGraph
  };
  
  DotDependencyGraphWriter(); 
  virtual ~DotDependencyGraphWriter();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;
  
  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
   void write_graphviz(
       std::ostream& os,
       uint64_t sentNum,
       const LinguisticGraphVertex begin,
       const LinguisticGraphVertex end,
       const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
       const LinguisticAnalysisStructure::AnalysisGraph* posgraph,
       const SyntacticData* syntacticData,
       const Common::AnnotationGraphs::AnnotationData* annotationData) const;
  
       /** @brief If true, only the dependency edges will be dumped. Otherwise, 
   * morphological links between vertices (reflecting the neigboroud of words)
   * will be dumped too.
   * 
   * This parameter is initialized through the writeOnlyDepEdges parameter of 
   * the process unit configuration file.
   */
  bool m_onlyDepEdges;

  std::map<std::string,std::string> m_graphDotOptions;
  std::map<std::string,std::string> m_nodeDotOptions;
  std::map<std::string,std::string> m_edgeDotOptions;
  
  DependencyGraphOutputMode m_outputMode;
  std::deque<std::string> m_vertexDisplay;
};

#include "DotDependencyGraphWriter.tcc"

} // end namespace
} // end namespace
} // end namespace

#endif
