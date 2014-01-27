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
