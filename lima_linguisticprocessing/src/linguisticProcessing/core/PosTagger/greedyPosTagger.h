// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      Declaration of a PosTagger with a simple greedy algorithm.
  *
  * @file       greedyPosTagger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @date       Created on November 2003
  * @version    $Id$
  */

#ifndef LIMA_POSTAGGER_GREEDYPOSTAGGER_H
#define LIMA_POSTAGGER_GREEDYPOSTAGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "ngramMatrices.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

#define GREEDYPOSTAGGER_CLASSID "GreedyPosTagger"

class LIMA_POSTAGGER_EXPORT [[deprecated]] GreedyPosTagger : public MediaProcessUnit
{

public:

  GreedyPosTagger() {}

  virtual ~GreedyPosTagger() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  std::shared_ptr<TrigramMatrix> m_trigramMatrix;
  std::shared_ptr<BigramMatrix> m_bigramMatrix;
  LinguisticCode m_microCatPonctuForte;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  MediaId m_language;

  void processVertex(LinguisticGraphVertex vx,LinguisticAnalysisStructure::AnalysisGraph* graph) const;

};


} // PosTagger
} // LinguisticProcessing
} // Lima

#endif
