// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      Declaration of a PosTagger with a simple Dummy algorithm.
  *
  * @file       DummyPosTagger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @date       Created on November 2003
  * @version    $Id$
  */

#ifndef LIMA_POSTAGGER_DUMMYPOSTAGGER_H
#define LIMA_POSTAGGER_DUMMYPOSTAGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "ngramMatrices.h"
#include "ngrampaths.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

#define DUMMYPOSTAGGER_CLASSID "DummyPosTagger"

class LIMA_POSTAGGER_EXPORT DummyPosTagger : public MediaProcessUnit
{

public:

  DummyPosTagger() {}

  virtual ~DummyPosTagger() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  MediaId m_language;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

};


} // PosTagger
} // LinguisticProcessing
} // Lima

#endif
