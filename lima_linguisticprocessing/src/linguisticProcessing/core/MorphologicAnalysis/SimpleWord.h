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
  Lima::LinguisticProcessing::AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
  AlternativesReader* m_reader;
  bool m_confidentMode;
  bool m_parseConcatenated;
  FsaStringsPool* m_sp;
  FlatTokenizer::CharChart* m_charChart;
  
  void linkConcatVertices(
    LinguisticGraph* graph,
    LinguisticGraphVertex srcToken,
    const std::vector<std::vector<LinguisticGraphVertex> >& concats) const;
};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
