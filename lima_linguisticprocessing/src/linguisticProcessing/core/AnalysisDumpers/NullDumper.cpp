// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2007 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "NullDumper.h"

#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDumpers
{

  SimpleFactory<MediaProcessUnit,NullDumper> NullDumperFactory(NULLDUMPER_CLASSID);

NullDumper::NullDumper()
: MediaProcessUnit()
{
}


NullDumper::~NullDumper()
{
}

void NullDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& ,
  Manager* )
{
}

LimaStatusCode NullDumper::process(
  AnalysisContent& ) const
{
  TimeUtils::updateCurrentTime();
//   DUMPERLOGINIT;

  TimeUtils::logElapsedTime("NullDumper");
  return SUCCESS_ID;
}

} // closing namespace AnalysisDumpers
} // closing namespace LinguisticProcessing
} // closing namespace Lima
