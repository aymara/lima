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
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_ALTERNATIVESREADER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_ALTERNATIVESREADER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

/**
@author Benoit Mathieu
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT AlternativesReader
{
public:
  AlternativesReader(
    bool confidentMode,
    bool tryDirect,
    bool tryUncapitalized,
    bool tryDesaccentedForm,
    FlatTokenizer::CharChart* charChart,
    FsaStringsPool* sp);
    
  virtual ~AlternativesReader();

  void readAlternatives(
    LinguisticAnalysisStructure::Token& token,
    const AnalysisDict::AbstractAnalysisDictionary& dico,
    AnalysisDict::AbstractDictionaryEntryHandler* lingInfosHandler = 0,
    AnalysisDict::AbstractDictionaryEntryHandler* concatHandler = 0,
    AnalysisDict::AbstractDictionaryEntryHandler* accentedHandler = 0) const;
    
private:
  bool m_confidentMode;
  bool m_tryDirect;
  bool m_tryUncapitalized;
  bool m_tryDesaccentedForm;
  FlatTokenizer::CharChart* m_charChart;
  FsaStringsPool* m_sp;
};

} // MorphologicAnalysis

} // LinguisticProcessing

} // Lima

#endif
