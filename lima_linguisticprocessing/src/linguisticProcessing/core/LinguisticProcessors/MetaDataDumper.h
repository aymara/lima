// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef METADATADUMPER_H
#define METADATADUMPER_H

#include "LinguisticProcessorsExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima {
namespace LinguisticProcessing {

#define METADATADUMPER_CLASSID "MetaDataDumper"

class LIMA_LINGUISTICPROCESSORS_EXPORT MetaDataDumper : public AbstractTextualAnalysisDumper
{
 public:
  MetaDataDumper(); 
  virtual ~MetaDataDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
};

} // end namespace
} // end namespace

#endif
