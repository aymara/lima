/*
    Copyright 2002-2019 CEA LIST

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

#include "KeysLogger.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <QDate>
#include <QFile>
#include <QXmlStreamReader>

#include <algorithm>

using namespace std;
using namespace Lima::Common::Misc;

namespace Lima
{

class KeysLoggerPrivate
{
  friend class KeysLogger;
public:

  KeysLoggerPrivate(std::ostream& out,
                    LinguisticProcessing::FlatTokenizer::CharChart* charChart,
                    bool reverseKeys);

  ~KeysLoggerPrivate() = default;

  bool parse(QIODevice *device);
  QString errorString() const;

  void readDictionary();
  void readEntry();
  void readConcat();
  void readI();
  void readC();
  void readP();

  QXmlStreamReader m_reader;
  std::ostream& m_out;
  LinguisticProcessing::FlatTokenizer::CharChart* m_charChart;
  bool m_reverseKeys;

  QString m_current;
  uint64_t m_count;

  const QString S_ENTRY = "entry";
  const QString S_K="k";
  const QString S_I="i";
  const QString S_L="l";
  const QString S_N="n";
  const QString S_C="c";
  const QString S_FORM="form";
  const QString S_DESACC="desacc";
  const QString S_OP="op";
  const QString S_DELETE="delete";
  const QString S_NO="no";
};

KeysLoggerPrivate::KeysLoggerPrivate(std::ostream& out,
                                     LinguisticProcessing::FlatTokenizer::CharChart* charChart,
                                     bool reverseKeys) :
    m_out(out),
    m_charChart(charChart),
    m_reverseKeys(reverseKeys),
    m_current(),
    m_count(0)
{
}

KeysLogger::KeysLogger(const QString& fileName,
                       std::ostream& out,
                       LinguisticProcessing::FlatTokenizer::CharChart* charChart,
                       bool reverseKeys) :
    m_d(new KeysLoggerPrivate(out, charChart, reverseKeys))
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
  {
    std::cerr << "Error opening " << fileName.toStdString() << std::endl;
    throw LimaException("Error opening KeysLogger file");
  }
  if (!m_d->parse(&file))
  {
    std::cerr << "Error parsing " << fileName.toStdString() << " : "
              << m_d->errorString().toStdString()
              << std::endl;
    throw LimaException("Error parsing KeysLogger file");
  }
}

KeysLogger::~KeysLogger()
{
  delete m_d;
}

bool KeysLoggerPrivate::parse(QIODevice *device)
{
  BOWLOGINIT;
  LDEBUG << "KeysLoggerPrivate::parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("dictionary"))
      {
          readDictionary();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA full-form dictionary XML file."));
      }
  }
  return !m_reader.error();
}

void KeysLoggerPrivate::readDictionary()
{
  BOWLOGINIT;
  LTRACE << "KeysLoggerPrivate::readDictionary" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("dictionary"));

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("entry"))
          readEntry();
      else
          m_reader.raiseError(QObject::tr("Expected an entry but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readEntry()
{
  BOWLOGINIT;
  LTRACE << "KeysLoggerPrivate::readEntry" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("entry"));

  m_current = m_reader.attributes().value(S_K).toString();
  if (m_current == 0)
  {
    DICTIONARYLOGINIT;
    LERROR << "invalid entry !";
    m_reader.raiseError(QObject::tr("invalid entry !"));
  }
  LimaString currentStr = m_current;
  if (m_reverseKeys)
  {
    std::reverse(currentStr.begin(),currentStr.end());
  }
  m_out << limastring2utf8stdstring(currentStr) << "\n";
  auto op = m_reader.attributes().value(S_OP);
  if (op.isEmpty() || op != S_DELETE)
  {
    LimaString desaccstr = m_charChart->unmark(currentStr);
    if (desaccstr.size() > 0 && desaccstr != currentStr)
    {
      m_out << limastring2utf8stdstring(desaccstr) << "\n";
    }
  }
  m_count++;
  if ((m_count % 10000)==0)
  {
    //DICTIONARYLOGINIT;
    std::cout << "\rextract keys :  " << m_count << " entries ..."
              << std::flush;
  }

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("i"))
          readI();
      else if (m_reader.name() == QLatin1String("concat"))
          readConcat();
      else
          m_reader.raiseError(QObject::tr("Expected an i or a concat but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readConcat()
{
  BOWLOGINIT;
  LTRACE << "KeysLoggerPrivate::readConcat" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("concat"));

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("c"))
          readC();
      else
          m_reader.raiseError(QObject::tr("Expected a c but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readI()
{
  BOWLOGINIT;
  LTRACE << "KeysLoggerPrivate::readI" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("i"));

  auto l = m_reader.attributes().value(S_L);
  if ((!l.isEmpty()) && (m_reverseKeys || m_current != l))
  {
    m_out << l.toUtf8().data() << "\n";
    m_current = l.toString();
  }
  auto n = m_reader.attributes().value(S_N);
  if ((!n.isEmpty()) && (l != n))
  {
    m_out << n.toUtf8().data() << "\n";
    m_current = n.toString();
  }

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("p"))
          readP();
      else
          m_reader.raiseError(QObject::tr("Expected a p but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readC()
{
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readC" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("c"));

    m_current = m_reader.attributes().value(S_FORM).toString();
    if (!m_current.isEmpty())
    {
      m_out << Common::Misc::limastring2utf8stdstring(m_current) << "\n";
    }
    else
    {
      DICTIONARYLOGINIT;
      LERROR << "WARN : no attribute form in tag 'c' !" ;
      m_reader.raiseError(QObject::tr("WARN : no attribute form in tag 'c' !"));
    }

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("i"))
          readI();
      else
          m_reader.raiseError(QObject::tr("Expected an i but got a %1.").arg(m_reader.name()));
  }
}

//     <p v="NC:m--"/>
void KeysLoggerPrivate::readP()
{
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readP" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("p"));
  m_reader.skipCurrentElement();
}

QString KeysLoggerPrivate::errorString() const
{
  XMLCFGLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

}
