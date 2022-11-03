// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISUNKNOWNWORDLOGGER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISUNKNOWNWORDLOGGER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <iostream>

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

#define UNKNOWNWORDLOGGER_CLASSID "UnknownWordLogger"

/**
@author Benoit Mathieu
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT UnknownWordLogger : public MediaProcessUnit
{
public:
  UnknownWordLogger();

  virtual ~UnknownWordLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  std::ostream* m_out;

};

}

}

}

#endif
