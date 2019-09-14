/*
    Copyright 2002-2019 CEA LIST

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

#ifndef LIMA_TENSORFLOWMORPHOSYNTAX_H
#define LIMA_TENSORFLOWMORPHOSYNTAX_H

#include "TensorFlowMorphoSyntaxExport.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

#include "common/MediaProcessors/MediaProcessUnit.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace MorphoSyntax
{

#define TENSORFLOWMORPHOSYNTAX_CLASSID "TensorFlowMorphoSyntax"

class TensorFlowMorphoSyntaxPrivate;

class LIMA_TENSORFLOWMORPHOSYNTAX_EXPORT TensorFlowMorphoSyntax : public MediaProcessUnit
{
public:

  TensorFlowMorphoSyntax();
  virtual ~TensorFlowMorphoSyntax();

  void init(
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  TensorFlowMorphoSyntaxPrivate *m_d;
};

} // MorphoSyntax
} // TensorFlowUnits
} // LinguisticProcessing
} // Lima

#endif
