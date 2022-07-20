// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_ACCENTEDCONCATENATEDDATAHANDLER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_ACCENTEDCONCATENATEDDATAHANDLER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima
{

namespace LinguisticProcessing
{
namespace FlatTokenizer
{
  class CharChart;
}
namespace MorphologicAnalysis
{

/**
@author Benoit Mathieu
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT AccentedConcatenatedDataHandler : public AnalysisDict::AbstractDictionaryEntryHandler
{
public:
  AccentedConcatenatedDataHandler(
    LinguisticGraph* outputGraph,
    const LimaString& sourceStr,
    uint64_t positionOffset,
    const LinguisticAnalysisStructure::TStatus& status,
    LinguisticAnalysisStructure::MorphoSyntacticType type,
    const FsaStringsPool* sp,
    FlatTokenizer::CharChart* charChart);

  virtual ~AccentedConcatenatedDataHandler();

  virtual void foundAccentedForm(StringsPoolIndex form) override;
  virtual void endAccentedForm() override;
  
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
  const LimaString& m_srcStr;
  uint64_t m_positionOffset;
  const LinguisticAnalysisStructure::TStatus& m_status;
  const FsaStringsPool* m_stringsPool;
  
  FlatTokenizer::CharChart* m_charChart;
  std::vector<unsigned char> m_unmarkToTextMapping;
  std::vector<unsigned char> m_positionMapping;
  
  std::vector<std::vector<LinguisticGraphVertex> > m_concatVertices;
  
  LinguisticAnalysisStructure::Token* m_currentToken;
  LinguisticAnalysisStructure::MorphoSyntacticData* m_currentData;
  LinguisticAnalysisStructure::LinguisticElement m_currentElement;
};

inline const std::vector<std::vector<LinguisticGraphVertex> >& AccentedConcatenatedDataHandler::getConcatVertices() const
{
  return m_concatVertices;
}


}

}

}

#endif
