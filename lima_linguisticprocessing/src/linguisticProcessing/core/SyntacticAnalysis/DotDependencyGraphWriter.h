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
    Manager* manager)
  ;
  
  LimaStatusCode process(AnalysisContent& analysis) const;

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
