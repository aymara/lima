// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
