// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LDEBUG << "KeysLoggerPrivate::parse";
#endif
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
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readDictionary" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("dictionary"));
#endif
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("entry"))
          readEntry();
      else
          m_reader.raiseError(QObject::tr("Expected an entry but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readEntry()
{
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readEntry" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("entry"));
#endif
  m_current = m_reader.attributes().value(S_K).toString();
  if (m_current.isEmpty())
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
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readConcat" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("concat"));
#endif
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("c"))
          readC();
      else
          m_reader.raiseError(QObject::tr("Expected a c but got a %1.").arg(m_reader.name()));
  }
}

void KeysLoggerPrivate::readI()
{
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readI" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("i"));
#endif
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
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readC" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("c"));
#endif
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
#ifdef DEBUG_LP
  DICTIONARYLOGINIT;
  LTRACE << "KeysLoggerPrivate::readP" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("p"));
#endif
  m_reader.skipCurrentElement();
}

QString KeysLoggerPrivate::errorString() const
{
  DICTIONARYLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

}
