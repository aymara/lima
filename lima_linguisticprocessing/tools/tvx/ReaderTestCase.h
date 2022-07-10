// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2006 by Oliier Mesnard                                  *
 *   olivier.mesnard@cea.fr                                                *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_ANALYSISTESTCASE_H
#define LIMA_ANALYSISTESTCASE_H

#include "linguisticProcessing/common/tgv/TestCase.h"
#include "linguisticProcessing/common/tgv/TestCaseProcessor.hpp"
#include "linguisticProcessing/client/xmlreader/AbstractXmlReaderClient.h"

#include <string>
#include <set>
#include <list>
#include <stdexcept>

namespace Lima
{

namespace ReaderValidation
{


class ReaderTestCaseProcessor : public Lima::Common::TGV::TestCaseProcessor
{
public:

  ReaderTestCaseProcessor( const std::string& workingDirectory,
    std::shared_ptr<Lima::XmlReader::AbstractXmlReaderClient> client);

  ~ReaderTestCaseProcessor() {}

  Lima::Common::TGV::TestCaseError processTestCase(const Lima::Common::TGV::TestCase& testCase) override;

private:

  std::shared_ptr<Lima::XmlReader::AbstractXmlReaderClient> m_lpclient;
};

}

}

#endif
