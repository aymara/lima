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
