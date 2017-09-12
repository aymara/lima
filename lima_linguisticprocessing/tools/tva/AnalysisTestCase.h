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
