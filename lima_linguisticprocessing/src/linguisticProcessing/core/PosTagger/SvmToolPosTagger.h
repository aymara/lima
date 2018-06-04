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

  struct PredData
  {
    PredData() :
        m_predMicro(),
        m_predIndex(),
    m_predPredMicros() {}
    LinguisticCode m_predMicro;
    uint64_t m_predIndex;
    std::vector<LinguisticCode> m_predPredMicros;
    inline bool operator<(const PredData& pd) const { return m_predMicro<pd.m_predMicro; }
  };

  typedef std::map< LinguisticCode, std::vector<PredData> > MicroCatDataMap;
  typedef std::map< LinguisticCode, std::vector<PredData> >::iterator MicroCatDataMapItr;
  typedef std::vector<PredData>::iterator PredDataVectorItr;
  typedef std::vector<PredData>::const_iterator PredDataVectorCItr;

  struct StepData
  {
    LinguisticGraphVertex m_srcVertex;
    std::vector<uint64_t> m_predStepIndexes;
    MicroCatDataMap m_microCatsData;
  };

  typedef std::vector<StepData> StepDataVector;
  typedef std::vector<StepData>::iterator StepDataVectorItr;

  struct TargetVertexId
  {
    LinguisticGraphVertex m_sourceVx;
    LinguisticCode m_categ;
    std::vector<LinguisticCode> m_preds;
    bool operator<(const TargetVertexId& tvi) const
    {
      if (m_sourceVx!=tvi.m_sourceVx) return m_sourceVx<tvi.m_sourceVx;
      if (m_categ!=tvi.m_categ) return m_categ<tvi.m_categ;
      return m_preds<tvi.m_preds;
    }
  };

  LinguisticGraphVertex reportPathsInGraph(
    LinguisticGraph* srcgraph,
    LinguisticGraph* resultgraph,
    LinguisticGraphVertex startVertex,
    StepDataVector& stepData,
    Common::AnnotationGraphs::AnnotationData* annotationData) const;

  SvmToolPosTaggerPrivate* m_d;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  MediaId m_language;
  tagger* m_tagger;
  std::string m_model;
};

class SvmToolPosTaggerFactory : public InitializableObjectFactory<MediaProcessUnit>
{
public:

  virtual MediaProcessUnit* create(
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
