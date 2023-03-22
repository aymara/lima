// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    std::shared_ptr<FlatTokenizer::CharChart> charChart,
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
  std::shared_ptr<FlatTokenizer::CharChart> m_charChart;
  FsaStringsPool* m_sp;
};

} // MorphologicAnalysis

} // LinguisticProcessing

} // Lima

#endif
