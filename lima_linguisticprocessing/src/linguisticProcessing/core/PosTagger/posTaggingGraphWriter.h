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


#ifndef LIMA_POSTAGGER_POSTAGGINGGRAPHWRITER_H
#define LIMA_POSTAGGER_POSTAGGINGGRAPHWRITER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "ngramMatrices.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include <string>

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

class LIMA_POSTAGGER_EXPORT PosTaggingGraphWriter
{

public:
  PosTaggingGraphWriter(
    const LinguisticGraph* graph,
    MediaId language,
    const Lima::LinguisticProcessing::PosTagger::TrigramMatrix* trigramMatrix,
    const Lima::LinguisticProcessing::PosTagger::BigramMatrix* bigramMatrix) :
      m_graph(graph),
      m_language(language),
      m_trigramMatrix(trigramMatrix),
  m_bigramMatrix(bigramMatrix) {}

  void writeToDotFile(const std::string& dotFileName,std::deque<std::string> vertexDisplay = std::deque<std::string>());
  
  void setOptions(const std::map<std::string,std::string>& graphDotOptions,
    const std::map<std::string,std::string>& nodeDotOptions,
    const std::map<std::string,std::string>& edgeDotOptions);
  
private:

  const LinguisticGraph* m_graph;
  const MediaId m_language;
  const Lima::LinguisticProcessing::PosTagger::TrigramMatrix*        m_trigramMatrix;
  const Lima::LinguisticProcessing::PosTagger::BigramMatrix*         m_bigramMatrix;

  std::map<std::string,std::string> m_graphDotOptions;
  std::map<std::string,std::string> m_nodeDotOptions;
  std::map<std::string,std::string> m_edgeDotOptions;
  
};

#include "posTaggingGraphWriter.tcc"

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
