// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "posTaggingGraphWriter.h"
#include "dotGraphWriter.h"

#include <fstream>
#include "common/misc/graphviznowarn.hpp"

using namespace boost;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

void PosTaggingGraphWriter::writeToDotFile(const std::string& dotFileName,deque<string> vertexDisplay)
{
  if (dotFileName != "")
  {
    LimaGraphGraphvizGraphWriter graphWriter(
                                             m_graphDotOptions,
                                             m_nodeDotOptions,
                                             m_edgeDotOptions);
    
    std::ofstream ofs(dotFileName.c_str(), std::ofstream::binary);
    write_graphviz(
      ofs,
      *m_graph,
      PosTaggingVertexWriter<LinguisticGraph,LinguisticGraphVertex>(m_graph,m_language,vertexDisplay),
      PosTaggingEdgeWriter<LinguisticGraph,LinguisticGraphEdge>(m_language,m_graph,m_trigramMatrix,m_bigramMatrix),
          graphWriter);
  }

}

void PosTaggingGraphWriter::setOptions(const std::map<std::string,std::string>& graphDotOptions,
                  const std::map<std::string,std::string>& nodeDotOptions,
                  const std::map<std::string,std::string>& edgeDotOptions)
{
  m_graphDotOptions = graphDotOptions;
  m_nodeDotOptions = nodeDotOptions;
  m_edgeDotOptions = edgeDotOptions;
}

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // closing namespace Lima
