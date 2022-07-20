// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISCONCATENATEDDATAHANDLER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISCONCATENATEDDATAHANDLER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

/**
@author Benoit Mathieu
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT ConcatenatedDataHandler : public AnalysisDict::AbstractDictionaryEntryHandler
{
public:
  ConcatenatedDataHandler(
    LinguisticGraph* outputGraph,
    LinguisticAnalysisStructure::Token* sourceToken,
    LinguisticAnalysisStructure::MorphoSyntacticType type,
    const FsaStringsPool* sp);

  virtual ~ConcatenatedDataHandler();

  virtual void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  virtual void endLingInfos() override;

  virtual void foundConcatenated() override;
  virtual void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form) override;
  virtual void endComponent() override;
  virtual void endConcatenated() override;

  virtual void foundProperties(LinguisticCode lings) override;
  
  const std::vector<std::vector<LinguisticGraphVertex> >& getConcatVertices() const;
  
private:
  LinguisticGraph* m_graph;
  LinguisticAnalysisStructure::Token* m_srcToken;
  const FsaStringsPool* m_stringsPool;
  
  std::vector<std::vector<LinguisticGraphVertex> > m_concatVertices;
  
  LinguisticAnalysisStructure::Token* m_currentToken;
  LinguisticAnalysisStructure::MorphoSyntacticData* m_currentData;
  LinguisticAnalysisStructure::LinguisticElement m_currentElement;
};

inline const std::vector<std::vector<LinguisticGraphVertex> >& ConcatenatedDataHandler::getConcatVertices() const
{
  return m_concatVertices;
}


}

}

}

#endif
