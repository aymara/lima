// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 17-JAN-2003
//
// DicoConcatenatedAlternatives is the module which creates split alternatives
// for concatenated expression tokens found into dictionary.
// Rules :
// <each FullToken of the main path is processed. Alternative paths
// are not processed>
// <there are as many created alternative paths as there are concatenated
// entries associated with main Token and orthographic alternative Tokens>
// <each concatenated entry gives FullToken path. Each token has the original
// FullToken localization>
// <if concatenated entry supplies dictionary entry, main Token of the just
// created FullToken takes this entry. Otherwise, a dictionary access is
// performed to find dictionary entry>

#ifndef LIMA_MORPHOLOGICANALYSIS_DICOCONCATENATEDALTERNATIVES_H
#define LIMA_MORPHOLOGICANALYSIS_DICOCONCATENATEDALTERNATIVES_H

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguistcAnalysisStructure {
class Token;
}

namespace MorphologicAnalysis
{

#define DICOCONCATENATEDALTERNATIVES_CLASSID "DicoConcatenatedAlternatives"

class DicoConcatenatedAlternatives : public MediaProcessUnit
{

public:

  DicoConcatenatedAlternatives();
  virtual ~DicoConcatenatedAlternatives();
  
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:

  MediaId m_language;

  void expandConcatenatedEntries(
    LinguisticGraphVertex v,
    LinguisticGraph* graph,
    LinguisticAnalysisStructure::Token* token,
    Dictionary::DictionaryEntry* entry) const;

};

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
