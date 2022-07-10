// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief  HyphenWordAlternatives is the module which creates split alternatives
  *         for hyphen word tokens. Each token from the supplied tokens path is processed :
  *         o FullToken must be "AlphaHyphen" typed by Tokenizer.
  *         o If a token has a single word entry or an orthographic alternative
  *           it is not decomposed
  *         o Token is break at hyphen boundaries and a new alternative path is created
  *         o each FullToken of the new Path is searched into dictionnary as Simple Word
  *         o If special hyphen entry, no alternatives are searched,
  *           otherwise Accented alternatives are searched
  *         o Path is valid even if not all FullToken have entry into dictionary
  *         @br
  *         Modified @date Dec, 02 2002 by GC to handle splitting on t_alpha_possessive
  *
  * @file   HyphenWordAlternatives.h
  * @author NAUTITIA jys
  * @author Gael de Chalendar
  * @author Copyright (c) 2002-2020 by CEA
  *
  * @date   created on Nov, 30 2002
  * @version    $Id$
  *
  */

#ifndef LIMA_MORPHOLOGICANALYSIS_HYPHENWORDALTERNATIVES_H
#define LIMA_MORPHOLOGICANALYSIS_HYPHENWORDALTERNATIVES_H

#include "MorphologicAnalysisExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/FlatTokenizer/Tokenizer.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "AlternativesReader.h"

namespace Lima {
  namespace Common {
    namespace AnnotationGraphs {
      class AnnotationData;
    }
  }
namespace LinguisticProcessing {
namespace MorphologicAnalysis {

#define HYPHENWORDALTERNATIVESFACTORY_CLASSID "HyphenWordAlternatives"

class LIMA_MORPHOLOGICANALYSIS_EXPORT HyphenWordAlternatives : public MediaProcessUnit
{

public:
    HyphenWordAlternatives();
    virtual ~HyphenWordAlternatives();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

    const FlatTokenizer::Tokenizer* m_tokenizer;
    AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
    FlatTokenizer::CharChart* m_charChart;
    bool m_deleteHyphenWord;
    bool m_confidentMode;
    MediaId m_language;
    MediaId m_engLanguageId;
    AlternativesReader* m_reader;
    std::string m_sentBoundariesName;

    void makeHyphenSplitAlternativeFor(
        LinguisticGraphVertex splitted,
        LinguisticGraph* graph,
        Common::AnnotationGraphs::AnnotationData* annotationData,
        SegmentationData* sb) const;

    bool isWorthSplitting(
        LinguisticGraphVertex splitted,
        LinguisticGraph* graph) const;

};

} // closing namespace MorphologicAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_MORPHOLOGICANALYSIS_HYPHENWORDALTERNATIVES_H
