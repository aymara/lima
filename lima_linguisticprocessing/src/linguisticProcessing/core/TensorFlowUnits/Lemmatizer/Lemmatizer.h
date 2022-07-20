// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_TENSORFLOWLEMMATIZER_H
#define LIMA_TENSORFLOWLEMMATIZER_H

#include "LemmatizerExport.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

#include "common/MediaProcessors/MediaProcessUnit.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Lemmatizer
{

#define TENSORFLOWLEMMATIZER_CLASSID "TensorFlowLemmatizer"

class TensorFlowLemmatizerPrivate;

class LIMA_TENSORFLOWLEMMATIZER_EXPORT TensorFlowLemmatizer : public MediaProcessUnit
{
public:

  TensorFlowLemmatizer();
  virtual ~TensorFlowLemmatizer();

  void init(
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  TensorFlowLemmatizerPrivate *m_d;
};

} // Lemmatizer
} // TensorFlowUnits
} // LinguisticProcessing
} // Lima

#endif
