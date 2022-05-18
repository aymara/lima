// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KEYLOGGER_H
#define KEYLOGGER_H


#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

#include <QtXml/QXmlDefaultHandler>

namespace Lima {

class KeysLogger : public QXmlDefaultHandler
{
public:
 
  KeysLogger(std::ostream& out,
             LinguisticProcessing::FlatTokenizer::CharChart* charChart,
             bool reverseKeys);
  
  virtual ~KeysLogger();

  bool  startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts ) override;
  
private:
  std::ostream& m_out;
  LinguisticProcessing::FlatTokenizer::CharChart* m_charChart;
  bool m_reverseKeys;
  
  QString m_current;
  uint64_t m_count;
  
  QString S_ENTRY;
  QString S_K;
  QString S_I;
  QString S_L;
  QString S_N;
  QString S_C;
  QString S_FORM;
  QString S_DESACC;
  QString S_OP;
  QString S_DELETE;
  QString S_NO;
};

}

#endif
