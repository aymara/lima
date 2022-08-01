// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_MORPHOLOGICANALYSIS_DOTGRAPHWRITER_H
#define LIMA_MORPHOLOGICANALYSIS_DOTGRAPHWRITER_H

#include "CompoundsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

#define ANNOTDOTGRAPHWRITER_CLASSID "AnnotDotGraphWriter"

class LIMA_COMPOUNDS_EXPORT AnnotDotGraphWriter : public MediaProcessUnit
{

public:
  AnnotDotGraphWriter();
  virtual ~AnnotDotGraphWriter();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

protected:

  std::string m_outputSuffix;
  MediaId m_language;
  std::string m_graphId;

};

} // Compounds
} // LinguisticProcessing
} // Lima

#endif
