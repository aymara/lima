// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2007-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSNULLDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSNULLDUMPER_H

#include "AnalysisDumpersExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include "common/MediaticData/mediaticData.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{


#define NULLDUMPER_CLASSID "NullDumper"

/**
@author Gael de Chalendar
dumper pour ne rien sortir du tout
*/
class LIMA_ANALYSISDUMPERS_EXPORT NullDumper : public MediaProcessUnit
{
public:
  NullDumper();

  virtual ~NullDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
};


} // closing namespace AnalysisDumpers
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSNULLDUMPER_H
