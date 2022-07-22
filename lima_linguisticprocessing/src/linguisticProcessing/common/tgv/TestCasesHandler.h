// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_TGV_TESTCASESHANDLER_H
#define LIMA_TGV_TESTCASESHANDLER_H

#include "TestCase.h"
#include "TestCaseProcessor.hpp"

#include <QtXml/QXmlDefaultHandler>

namespace Lima
{
namespace Common
{
namespace TGV
{
class LIMA_TGV_EXPORT TestCasesHandler : public QXmlDefaultHandler
{

public:
  TestCasesHandler(TestCaseProcessor& processor);
  virtual ~ TestCasesHandler();

  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endElement(const QString & namespaceURI, const QString & name, const QString & qName) override;
  
  bool characters(const QString& chars) override;
  
  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes) override;
  
  bool startDocument() override;
  
  inline bool hasFatalError() const { return m_hasFatalError; }
  
  
  struct TestReport {
    uint64_t success;
    uint64_t failed;
    uint64_t conditional;
    uint64_t nbtests;
    TestReport() : success(0),failed(0),conditional(0),nbtests(0) {};
  };

  std::map<std::string,TestReport> m_reportByType;
  
private:
  std::string attributeValue(const QString& attr, const QXmlAttributes& attrs) const;
  
  // Liste des parametres possibles et obligatoires dans un TestCase
  // pour lp: (text, language, pipeline?...)
  // std::map<std::string,bool> m_simpleValParamTestCaseKeys;
  // std::map<std::string,bool> m_multiValParamTestCaseKeys;
  
  TestCaseProcessor& m_processor;
  TestCase currentTestCase;

  std::string getName(const QString& localName,
                      const QString& qName);
  
  bool m_inText;
  bool m_inExpl;
  bool m_inParam;
  bool m_inList;
  bool m_inMap;
  std::string m_listKey;
  std::string m_mapKey;
  
  bool m_hasFatalError;
};

// This utility function return a user-defined exit code (64) if a "bloquant"
// test failed.
LIMA_TGV_EXPORT int exitCode(TestCasesHandler const & tch);

} // TGV
} // Common
} // Lima


#endif
