// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGER_DYNAMICSVMTOOLPOSTAGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGER_DYNAMICSVMTOOLPOSTAGGER_H

// #include "viterbiUtils.h"
// #include "frequencyCost.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <list>
#include <vector>
#include <set>
#include <map>
#include <queue>

#include <sstream>

class tagger;
class nodo;

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

#define DYNAMICSVMTOOLPOSTAGGER_CLASSID "DynamicSvmToolPosTagger"

/**
@brief A PoS tagger using SVMTool as a backend and Viterbi to maximize the results
@author Quentin Pradet
*/
class DynamicSvmToolPosTagger : public MediaProcessUnit
{
public:
  DynamicSvmToolPosTagger() {}

  virtual ~DynamicSvmToolPosTagger() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  /* Configuration */
  LinguisticCode m_defaultCateg; // default category, used before and after text.
  std::list<LinguisticCode> m_stopCategories; // sentence boundaries

  /* Book-keeping */
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  const Common::PropertyCode::PropertyManager* m_MicroManager;
  MediaId m_language;

  /* Stores the information we know about the path thus far:
   *  - prev is the best vertex to choose when going through this vertex
   *  - score is the sum of the scores thus far (not normalized)
   *  - pathLength helps us to normalize the score
   *  - category is the chosen microcategory
   */
  struct PathInfo {
    LinguisticGraphVertex prev;
    double score;
    std::string pos;
    int pathLength;
  };


  /* The SVMTool tagger */
  tagger *t;
  boost::tuple<std::string, uint64_t> SVMTool(
    const LinguisticGraph* srcGraph,
    LinguisticGraphVertex vertex,
    LinguisticGraphVertex prevVertex,
    std::map<LinguisticGraphVertex, struct PathInfo > &maxAncestors) const;

  /* Useful functions */
  std::vector<std::string> getMicros(LinguisticGraphVertex token, const LinguisticGraph *srcGraph) const;
  std::set<LinguisticGraphVertex> getPreviousTokens(LinguisticGraphVertex token, const LinguisticGraph *srcGraph) const;
  std::vector<nodo*> buildContext(const LinguisticGraph*, LinguisticGraphVertex, LinguisticGraphVertex, LinguisticGraphVertex) const;
  std::string getWord(LinguisticGraphVertex token, const LinguisticGraph *srcGraph) const;
  LinguisticGraphVertex nextToken(LinguisticGraphVertex token, const LinguisticGraph* srcGraph) const;
  std::set<LinguisticGraphVertex> nextTokens(LinguisticGraphVertex token, const LinguisticGraph* srcGraph) const;

};

class DynamicSvmToolPosTaggerFactory : public InitializableObjectFactory<MediaProcessUnit>
{
public:

  virtual MediaProcessUnit* create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    MediaProcessUnit::Manager* manager) const override ;

private:
  DynamicSvmToolPosTaggerFactory(const std::string& id);
  static DynamicSvmToolPosTaggerFactory* s_instance;

};


}

}

}

#endif
