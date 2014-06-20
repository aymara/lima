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
 * @file       DotDependencyGraphWriter.cpp
 * @author     besancon (besanconr@zoe.cea.fr) 

 *             copyright   Copyright (C) 2004 by CEA LIST
 * @date       Thu Oct 21 2004
 * @version    $Id$
 */

#include "DotDependencyGraphWriter.h"
#include "DependencyGraph.h"
#include "SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

// #include "linguisticProcessing/core/PosTagger/dotGraphWriter.h"
// #include "linguisticProcessing/core/PosTagger/posTaggingGraphWriter.h"


#include <fstream>
#include <queue>
#include <boost/regex.hpp>
#include "common/misc/graphviznowarn.hpp"

using namespace boost;
using namespace std;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {

namespace LinguisticProcessing {

namespace SyntacticAnalysis {

SimpleFactory<MediaProcessUnit,DotDependencyGraphWriter> 
dotDependencyGraphWriterFactory(DOTDEPENDENCYGRAPHWRITER_CLASSID);

DotDependencyGraphWriter::DotDependencyGraphWriter() :
    m_outputMode(FullGraph)
{}

DotDependencyGraphWriter::~DotDependencyGraphWriter()
{}

void DotDependencyGraphWriter::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DotGraphWriter::init(unitConfiguration,manager);

  m_onlyDepEdges=false;
  try 
  {
    string onlyDepEdges=
      unitConfiguration.getParamsValueAtKey("writeOnlyDepEdges");
    if (onlyDepEdges == "true" || 
        onlyDepEdges == "yes") 
    {
      m_onlyDepEdges=true;
    }
  }
  catch (NoSuchParam& ) // keep default value (false)
  {}
  
  try 
  {
    string outputMode =
        unitConfiguration.getParamsValueAtKey("outputMode");
    if (outputMode == "SentenceBySentence") 
    {
      m_outputMode = SentenceBySentence;
    }
  }
  catch (NoSuchParam& ) // keep default value (FullGraph)
  {
  }
  try 
  {
    m_graphDotOptions = unitConfiguration.getMapAtKey("graphDotOptions");
  }
  catch (NoSuchMap& ) {}
  
  try 
  {
    m_nodeDotOptions = unitConfiguration.getMapAtKey("nodeDotOptions");
  }
  catch (NoSuchMap& ) {}
  
  try 
  {
    m_edgeDotOptions = unitConfiguration.getMapAtKey("edgeDotOptions");
  }
  catch (NoSuchMap& ) {}
  try
  {
    m_vertexDisplay=unitConfiguration.getListsValueAtKey("vertexDisplay");
  }
  catch (NoSuchList& )
  {
    // empty display
  }
}


LimaStatusCode DotDependencyGraphWriter::process(AnalysisContent& analysis) const
{
  SALOGINIT;
  
  AnalysisGraph* anagraph=dynamic_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  AnalysisGraph* posgraph=dynamic_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  AnnotationData* annotationData=dynamic_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SALOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  if (m_outputMode == FullGraph)
  {
    string outputFileName=metadata->getMetaData("FileName") + m_outputSuffix;

    std::ofstream ofs(outputFileName.c_str(), std::ofstream::binary);

    if (m_onlyDepEdges) 
    {
      // write only the dependency graph
      // (but uses the PosTaggingVertexWriter to write the vertices
      // of the morphological graph)
      PosTagger::
        PosTaggingVertexWriter<LinguisticGraph, LinguisticGraphVertex> 
        phoenixGraphVertexWriter(posgraph->getGraph(),
                                m_language,
                                m_vertexDisplay);
      
      DependencyGraphVertexWriter<DependencyGraph,DependencyGraphVertex>
        vertexWriter(syntacticData->dependencyGraph(),
                    phoenixGraphVertexWriter);
      
      DependencyGraphEdgeWriter<DependencyGraph,DependencyGraphEdge> 
        edgeWriter(syntacticData->dependencyGraph(),
                  m_language);
        
      LimaGraphGraphvizGraphWriter graphWriter(
        m_graphDotOptions,
        m_nodeDotOptions,
        m_edgeDotOptions);
      boost::write_graphviz(ofs,
                  *(syntacticData->dependencyGraph()),
                    vertexWriter,
                    edgeWriter,
                    graphWriter);
    }
    else 
    {
      // write the morphological graph
      // and add the edges from the dependency graph
      PosTagger::
        PosTaggingVertexWriter<LinguisticGraph, LinguisticGraphVertex> 
        vertexWriter(posgraph->getGraph(),
                    m_language,
                    m_vertexDisplay);
      
      PosTaggingDepGraphEdgeWriter<LinguisticGraph,LinguisticGraphEdge> 
        edgeWriter(posgraph->getGraph(),m_language,
                  syntacticData->dependencyGraph(),
                   syntacticData);
      
      LimaGraphGraphvizGraphWriter graphWriter(
        m_graphDotOptions,
        m_nodeDotOptions,
        m_edgeDotOptions);
      
      boost::write_graphviz(ofs,
                            *(syntacticData->dependencyGraph()),
                    vertexWriter,
                    edgeWriter,
                    graphWriter);
    }
  }
  else if (m_outputMode == SentenceBySentence)
  {
    SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
    if (sb==0)
    {
      LERROR << "no sentence bounds ! abort";
      return MISSING_DATA;
    }
    if (sb->getGraphId() != "PosGraph") {
      LERROR << "SentenceBounds have been computed on " << sb->getGraphId() << " !";
      LERROR << "DotDependencyGraphWriter needs SentenceBounds on PosGraph";
      return INVALID_CONFIGURATION;
    }

    uint64_t i=0;
    // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
    //     boundItr!=sb->end();
    for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
         boundItr!=(sb->getSegments()).end();
         boundItr++, i++)
    {
      LinguisticGraphVertex beginSentence=boundItr->getFirstVertex();
      LinguisticGraphVertex endSentence=boundItr->getLastVertex();
//       string firstWord;
//       LinguisticAnalysisStructure::Token* token=get(vertex_token, *anagraph->getGraph(), (*(boundItr))+1);
//       if (token != 0)
//         firstWord = limastring2utf8stdstring(token->stringForm());
//       firstWord=boost::regex_replace(firstWord, boost::regex("\'"), "");
//       firstWord=boost::regex_replace(firstWord, boost::regex("é"), "e");
//       firstWord=boost::regex_replace(firstWord, boost::regex("è'"), "e");
//       firstWord=boost::regex_replace(firstWord, boost::regex("à'"), "a");
//       firstWord=boost::regex_replace(firstWord, boost::regex("ç'"), "c");
//       firstWord=boost::regex_replace(firstWord, boost::regex("ù'"), "u");
      std::ostringstream outputFileName;
      outputFileName << metadata->getMetaData("FileName") << ".sent"<< i /*<< firstWord*/ << m_outputSuffix;

      std::ofstream ofs(outputFileName.str().c_str(), std::ofstream::binary);

     
      ofs << "digraph sentence" << i << " {" << std::endl;
      ofs << "  graph [";
      for (std::map<std::string,std::string>::const_iterator graphDotOptionsIt = m_graphDotOptions.begin() ; graphDotOptionsIt != m_graphDotOptions.end(); graphDotOptionsIt++)
      {
        ofs << graphDotOptionsIt->first << "=" << graphDotOptionsIt->second << ",";
      }
      ofs <<"]"<<std::endl;
      ofs << "  node [";
      for (std::map<std::string,std::string>::const_iterator nodeDotOptionsIt = m_nodeDotOptions.begin() ; nodeDotOptionsIt != m_nodeDotOptions.end(); nodeDotOptionsIt++)
      {
        ofs << nodeDotOptionsIt->first << "=" << nodeDotOptionsIt->second << ",";
      }
      ofs << "]"<<std::endl;
      ofs << "  edge [";
      for (std::map<std::string,std::string>::const_iterator edgeDotOptionsIt = m_edgeDotOptions.begin() ; edgeDotOptionsIt != m_edgeDotOptions.end(); edgeDotOptionsIt++)
      {
        ofs << edgeDotOptionsIt->first << "=" << edgeDotOptionsIt->second << ",";
      }
      ofs << "]"<<std::endl;
      
      if (m_onlyDepEdges) 
      {
        // write only the dependency graph
        // (but uses the PosTaggingVertexWriter to write the vertices
        // of the morphological graph)
        
      }
      else 
      {
        write_graphviz(ofs,
                       i,
                       beginSentence,
                       endSentence,
                       anagraph,
                       posgraph,
                       syntacticData,
                       annotationData);
      }      
      ofs << "}"<<std::endl;
    }
  }
  else // output mode
  {
    LERROR << "Unknown output mode";
    return UNKNOWN_ERROR;
  }
  return SUCCESS_ID;
}

//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void DotDependencyGraphWriter::write_graphviz(
                                       std::ostream& os,
                                       uint64_t sentNum,
                                       const LinguisticGraphVertex begin,
                                       const LinguisticGraphVertex end,
                                       const AnalysisGraph* anagraph,
                                       const AnalysisGraph* posgraph,
                                       const SyntacticData* syntacticData,
                                       const Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  LIMA_UNUSED(sentNum);
  LIMA_UNUSED(anagraph);
  LIMA_UNUSED(annotationData);
  SALOGINIT;
  LDEBUG << "DotDependencyGraphWriter::write_graphviz  begin="<<begin<<" ; end=" << end;
//   CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *syntacticData->dependencyGraph());
  
  const LinguisticGraph& lposgraph=*(posgraph->getGraph());
  const DependencyGraph* depGraph=syntacticData->dependencyGraph();
  
  std::set<LinguisticGraphVertex> visited;
  std::queue<LinguisticGraphVertex> toVisit;
  toVisit.push(begin);

  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  while (!toVisit.empty())
  {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();

    os << v << " ";
    DependencyGraphVertexWriter<DependencyGraph,DependencyGraphVertex>(
        depGraph,PosTagger::PosTaggingVertexWriter<LinguisticGraph,LinguisticGraphVertex> (&lposgraph,m_language,m_vertexDisplay))(os,v);
        
    for (boost::tie(outItr,outItrEnd)=out_edges(v,lposgraph);
         outItr!=outItrEnd;
         outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,lposgraph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        if (next != end)
        {
          toVisit.push(next);
        }
        else
        {
          LDEBUG << "PosTaggingDepGraphEdgeWriter reached end";
        }
      }
      
      os << v << " -> " << next << " ";
      LDEBUG << "PosTaggingDepGraphEdgeWriter for "<<v<<" -> " << next;
      PosTaggingDepGraphEdgeWriter<LinguisticGraph,LinguisticGraphEdge>(&lposgraph,m_language,depGraph,syntacticData)(os,*outItr);
          
    }
  }
}



} // end namespace
} // end namespace
} // end namespace
