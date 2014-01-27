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
  * @author Copyright (c) 2002-2003 by CEA
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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;    
        
protected:

private:
    
    const FlatTokenizer::Tokenizer* m_tokenizer;
    AnalysisDict::AbstractAnalysisDictionary* m_dictionary;
    FlatTokenizer::CharChart* m_charChart;
    bool m_deleteHyphenWord;
    bool m_confidentMode;
    MediaId m_language;
    AlternativesReader* m_reader;
    
    void makeHyphenSplitAlternativeFor(
        LinguisticGraphVertex splitted,
        LinguisticGraph* graph,
        Common::AnnotationGraphs::AnnotationData* annotationData) const;

};

} // closing namespace MorphologicAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_MORPHOLOGICANALYSIS_HYPHENWORDALTERNATIVES_H
