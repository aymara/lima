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
