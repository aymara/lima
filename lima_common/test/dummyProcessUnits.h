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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_DUMMYPROCESSUNITS_H
#define LIMA_DUMMYPROCESSUNITS_H

#include <iostream>

#include "common/LimaCommon.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/ProcessUnitFramework/AbstractProcessUnit.h"
#include "common/ProcessUnitFramework/ProcessUnitPipeline.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

namespace Lima
{

class DummyProcessUnit : public AbstractProcessUnit<DummyProcessUnit>
{
public:

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) = 0;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const = 0;
};

class DreamingProcessUnit : public DummyProcessUnit
{
public:

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
   {
     LIMA_UNUSED(unitConfiguration);
     LIMA_UNUSED(manager);
  };

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const {
      LIMA_UNUSED(analysis);
      std::cout << "DreamingProcessUnit is dreaming that it chases the mouse ..." << std::endl;
      return SUCCESS_ID;
    }
};

class ZenProcessUnit : public DummyProcessUnit
{
public:

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
   {
     LIMA_UNUSED(unitConfiguration);
     LIMA_UNUSED(manager);
  };

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const {
      LIMA_UNUSED(analysis);
      std::cout << "ZenProcessUnit is wondering what is the sound of one hand clapping ..." << std::endl;
      return SUCCESS_ID;
  }
};

typedef ProcessUnitPipeline<DummyProcessUnit> DummyProcessUnitPipeline;

} // Lima

#endif
