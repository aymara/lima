// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * @brief      a dumper that outputs tab-separated word features, one word per line
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTFEATURESDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTFEATURESDUMPER_H

#include "AnalysisDumpersExport.h"
#include "WordFeatureExtractor.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define TEXTFEATURESDUMPER_CLASSID "TextFeaturesDumper"

class LIMA_ANALYSISDUMPERS_EXPORT TextFeaturesDumper : public AbstractTextualAnalysisDumper
{
public:
  TextFeaturesDumper();
  virtual ~TextFeaturesDumper();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  std::string m_graph; /*< name of the graph */
  std::string m_sep; /*< separator between features */
  std::string m_sepReplace; /*< character to replace separator in feature strings */
  std::string m_sepPOS; /*< separator between POS */
  WordFeatures m_features; /* list of features */

  // private member functions
  void outputVertex(std::ostream& out,
                    const LinguisticAnalysisStructure::AnalysisGraph* graph,
                    LinguisticGraphVertex v,
                    AnalysisContent& analysis,
                    uint64_t offset=0) const;
};

struct lTokenPosition
{
  inline bool operator()(const LinguisticAnalysisStructure::Token* f1, const LinguisticAnalysisStructure::Token* f2) const
  {
    if (f1->position()!=f2->position()) return f1->position()<f2->position();
    return f1->length()<f2->length();
  }
};

}

}

}

#endif
