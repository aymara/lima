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
