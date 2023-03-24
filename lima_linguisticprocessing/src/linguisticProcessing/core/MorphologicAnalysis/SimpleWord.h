// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 8-OCT-2002
//
// SimpleWord is the implementation of the 1st module of
// Morphological Analysis. Each token from the main tokens
// path is searched into the specified dictionary.

#ifndef LIMA_MORPHOLOGICANALYSIS_SIMPLEWORD_H
#define LIMA_MORPHOLOGICANALYSIS_SIMPLEWORD_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "AlternativesReader.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

#define SIMPLEWORD_CLASSID "SimpleWord"

class LIMA_MORPHOLOGICANALYSIS_EXPORT SimpleWord : public MediaProcessUnit
{

public:
  SimpleWord();
  virtual ~SimpleWord();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

private:
  std::shared_ptr<AnalysisDict::AbstractAnalysisDictionary> m_dictionary;
  std::shared_ptr<AlternativesReader> m_reader;
  bool m_confidentMode;
  bool m_parseConcatenated;
  FsaStringsPool* m_sp;
  std::shared_ptr<FlatTokenizer::CharChart> m_charChart;
  
  void linkConcatVertices(
    LinguisticGraph* graph,
    LinguisticGraphVertex srcToken,
    const std::vector<std::vector<LinguisticGraphVertex> >& concats) const;
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
