// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    Manager* manager) override = 0;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override = 0;
};

class DreamingProcessUnit : public DummyProcessUnit
{
public:

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  {
     LIMA_UNUSED(unitConfiguration);
     LIMA_UNUSED(manager);
  }

  virtual LimaStatusCode process(AnalysisContent& analysis) const override 
  {
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
    Manager* manager) override
   {
     LIMA_UNUSED(unitConfiguration);
     LIMA_UNUSED(manager);
  };

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const override {
      LIMA_UNUSED(analysis);
      std::cout << "ZenProcessUnit is wondering what is the sound of one hand clapping ..." << std::endl;
      return SUCCESS_ID;
  }
};

typedef ProcessUnitPipeline<DummyProcessUnit> DummyProcessUnitPipeline;

} // Lima

#endif
