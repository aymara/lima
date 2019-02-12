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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGER_VITERBIPOSTAGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGER_VITERBIPOSTAGGER_H

#include "PosTaggerExport.h"
#include "viterbiUtils.h"
#include "frequencyCost.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
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

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

#define VITERBIPOSTAGGER_CLASSID "ViterbiPosTagger"

/**
@author Benoit Mathieu
*/
template<typename Cost,typename CostFunction>
class ViterbiPosTagger : public MediaProcessUnit
{
public:
  ViterbiPosTagger(const CostFunction& costFunction) :
      m_costFunction(costFunction),
      m_defaultCateg(),
      m_stopCategories(),
      m_microAccessor() {}

  virtual ~ViterbiPosTagger() {}

  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="ViterbiPosTagger"&gt;</b>
   *    -  defaultCategory : micro category to use when no categories are
   *         available. For example, used before and after text to
   *         disambiguate.
   *  -  stopCategories : list of categories that delimits independant
   *         segment to disambiguate.
   *    -  costFunction : Type of cost to user for disambiguation :
   *         -  ViterbiIntegerCost : use bigram and trigram without frequency
   *         -  ViterbiFrequencyCost : use bigram and trigram with frequency
   *    -  trigramFile : file containing the trigram matrix
   *    -  bigramFile : file containing the bigram matrix
   */
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  struct PredData
  {
    PredData() :
        m_cost(),
        m_predMicro(),
        m_predIndex(),
    m_predPredMicros() {}
    Cost m_cost;
    LinguisticCode m_predMicro;
    uint64_t m_predIndex;
    std::vector<LinguisticCode> m_predPredMicros;
    // this is used to insert PredData in ordered containers
    inline bool operator<(const PredData& pd) const { return m_predMicro<pd.m_predMicro; }
  };

  typedef std::map< LinguisticCode, std::vector<PredData> > MicroCatDataMap;
  typedef typename std::map< LinguisticCode, std::vector<PredData> >::iterator MicroCatDataMapItr;
  typedef typename std::vector<PredData>::iterator PredDataVectorItr;
  typedef typename std::vector<PredData>::const_iterator PredDataVectorCItr;

  struct StepData
  {
    LinguisticGraphVertex m_srcVertex; // associated vertex in the graph
    std::vector<uint64_t> m_predStepIndexes; // direct ancestors in the StepData vector
    MicroCatDataMap m_microCatsData; // the micro categories associated to the node
  };

  typedef std::vector<StepData> StepDataVector;
  typedef typename std::vector<StepData>::iterator StepDataVectorItr;

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

  const CostFunction m_costFunction;
  LinguisticCode m_defaultCateg;
  std::list<LinguisticCode> m_stopCategories;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  MediaId m_language;

	/**
	 * The goal here is to perform a topological sort on our sentence
	 * lattice. We can't do a simple breadth first search because our
	 * lattice will have paths of different size. Canonical example:
	 *
	 *      --B--
	 *     /     \
	 *  --A       E--
	 *     \     /
	 *      C---D
	 *
	 * If we start with A, we want to make sure that E is only added after
	 * D *and* B have been added. This is the reason we're comparing
	 * the indegree of a node and the number of predIndex seen so far.
	 * With the example below, we would put "A B C D E" in stepData,
	 * along with their microdatas.
	 */

	void initializeStepDataFromGraph(
    const LinguisticGraph* srcgraph,
    LinguisticGraphVertex start,
    LinguisticCode startMicro,
    const std::list< LinguisticCode >& predCats,
    LinguisticGraphVertex end,
    StepDataVector& stepData) const;

  void performViterbiOnStepData(StepDataVector& stepData) const;

	/**
	 * Every node in our sentence lattice knows what previous node will
	 * provide the best score. Thus, we only need to start from the last
	 * node and follow the indications left by the cost computations.
	 * The only edge case is when we have twice the same cost: we add two
	 * previous nodes and will provide two paths. We still have a graph,
	 * even if in most cases it will only be a list.
	 */
	LinguisticGraphVertex reportPathsInGraph(
    LinguisticGraph* srcgraph,
    LinguisticGraph* resultgraph,
    LinguisticGraphVertex startVertex,
    StepDataVector& stepData,
    Common::AnnotationGraphs::AnnotationData* annotationData) const;

};

class LIMA_POSTAGGER_EXPORT ViterbiPosTaggerFactory : public InitializableObjectFactory<MediaProcessUnit>
{
public:

  virtual MediaProcessUnit* create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    MediaProcessUnit::Manager* manager) const override ;

private:
  ViterbiPosTaggerFactory(const std::string& id);
  static std::unique_ptr< ViterbiPosTaggerFactory > s_instance;

};


}

}

}

#include "ViterbiPosTagger.tcc"

#endif
