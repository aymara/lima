// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_ANALYSISTESTCASE_H
#define LIMA_ANALYSISTESTCASE_H

#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCaseProcessor.hpp"
#include "linguisticProcessing/common/tgv/TestCaseError.hpp"
#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"

#include <string>
#include <set>
#include <list>
#include <stdexcept>

namespace Lima
{

namespace AnalysisValidation
{


class AnalysisTestCaseProcessor : public Lima::Common::TGV::TestCaseProcessor
{
public:

  AnalysisTestCaseProcessor( const std::string& workingDirectory,
  LinguisticProcessing::AbstractLinguisticProcessingClient* client, const std::map<std::string, AbstractAnalysisHandler*>& handlers);

  ~AnalysisTestCaseProcessor() {}

  Lima::Common::TGV::TestCaseError processTestCase(const Lima::Common::TGV::TestCase& testCase) override;

private:

  LinguisticProcessing::AbstractLinguisticProcessingClient* m_lpclient;
  std::map<std::string, AbstractAnalysisHandler*> m_handlers;
};

}

}

#endif
