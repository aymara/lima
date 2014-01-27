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
