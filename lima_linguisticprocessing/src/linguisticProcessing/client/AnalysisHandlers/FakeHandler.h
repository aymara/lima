// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGFAKEHANDLER_H
#define LIMA_LINGUISTICPROCESSINGFAKEHANDLER_H

#include "AnalysisHandlersExport.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/Data/genericDocumentProperties.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

namespace Lima
{

namespace LinguisticProcessing
{

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT FakeHandler : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  FakeHandler();

  virtual ~FakeHandler();

  virtual void endAnalysis() override;
  virtual void endDocument() override;
  virtual void handle(const char* buf, int length) override ;
  virtual void startAnalysis() override;
  virtual void startDocument(const Common::Misc::GenericDocumentProperties& props) override;

};

}

}

#endif
