// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGER_SVMTOOLPOSTAGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGER_SVMTOOLPOSTAGGER_H

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

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

#define SVMTOOLPOSTAGGER_CLASSID "SvmToolPosTagger"

class SvmToolPosTaggerPrivate;
/**
@brief A PoS tagger using SVMTool as a backend
@note This is a basic implementation that should be improved in several ways:
- currently SVMTool accepts only a linear stream of tokens instead as a lattice as produced by our morphologic analysis. It should be merged with another algorithm (Viterbi ?) to chose the better "SVM path" between the various possibilities
- SVMTool takes as input only the tokens. It then searches the possible categories inside its dictionary built during model learning. It uses a learnt guesser to find categories of words it does not know. We should be able to give it the list of possible categories for each token, even if it is in its dictionary
- the handling of I/O is currently brutal: create a string from anagraph with all tokens and get the full result in a string befor splitting it into tokens again to produce the posgraph. This should be improved with streambuffers adapting the anagraph and the posgraph
@author Gael de Chalendar
*/
class SvmToolPosTagger : public MediaProcessUnit
{
  friend class SvmToolPosTaggerPrivate;
public:
  SvmToolPosTagger();

  // TODO delete m_tagger!
  virtual ~SvmToolPosTagger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  SvmToolPosTaggerPrivate* m_d;
};

class SvmToolPosTaggerFactory : public InitializableObjectFactory<MediaProcessUnit>
{
public:

  virtual std::shared_ptr<MediaProcessUnit> create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    MediaProcessUnit::Manager* manager) const override ;

private:
  SvmToolPosTaggerFactory(const std::string& id);
  static SvmToolPosTaggerFactory* s_instance;

};


}

}

}

#endif
