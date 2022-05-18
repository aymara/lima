// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
