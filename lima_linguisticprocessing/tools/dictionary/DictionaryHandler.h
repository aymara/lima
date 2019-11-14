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

#ifndef DICTIONARYCOMPILER_H
#define DICTIONARYCOMPILER_H

#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "common/misc/AbstractAccessByString.h"

#include <QtXml/QXmlDefaultHandler>

#include <sstream>
#include <iostream>
#include <list>

namespace Lima
{

class DictionaryCompilerPrivate;
class DictionaryCompiler : public QXmlDefaultHandler
{
  friend class DictionaryCompilerPrivate;
public:

  DictionaryCompiler(
    LinguisticProcessing::FlatTokenizer::CharChart* charChart,
    Common::AbstractAccessByString* access,
    const std::map<std::string,LinguisticCode>& conversionMap,
    bool reverseKeys);

  virtual ~DictionaryCompiler() = default;
  DictionaryCompiler(const DictionaryCompiler&) = delete;
  DictionaryCompiler& operator=(const DictionaryCompiler&) = delete;

  bool startElement(const QString & namespaceURI,
                    const QString & name,
                    const QString & qName,
                    const QXmlAttributes & atts) override;

  bool endElement(const QString& namespaceURI,
                  const QString& name,
                  const QString & qName) override;

  void writeBinaryDictionary(std::ostream& out);

private:
  DictionaryCompilerPrivate* m_d;
};

}

#endif
