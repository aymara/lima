// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
