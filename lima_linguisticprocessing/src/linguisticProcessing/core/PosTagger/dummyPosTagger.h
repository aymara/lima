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
  Common::PropertyCode::PropertyAccessor m_macroAccessor;
  Common::PropertyCode::PropertyAccessor m_microAccessor;

};


} // PosTagger
} // LinguisticProcessing
} // Lima

#endif
