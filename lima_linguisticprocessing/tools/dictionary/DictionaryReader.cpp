// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "DictionaryReader.h"

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <QXmlStreamReader>

#include <algorithm>

using namespace Lima::Common;
using namespace Lima::Common::Misc;

namespace Lima
{

#define BUFFER_SIZE 4096

#define CODED_INT_BUFFER_SIZE 10

/* data type */
struct LingInfo
{
  LingInfo() : del(false),lemma(0),norm(0),lingProps(0) {};
  bool del;
  uint64_t lemma;
  uint64_t norm;
  uint64_t lingProps;
};

struct Accented
{
  Accented() : del(false),id(0) {};
  bool operator<(const Accented& a) const {return id<a.id;};
  bool del;
  uint64_t id;
};

struct Component
{
  Component() : form(0),pos(0),len(0),lingInfos() {};
  uint64_t form;
  unsigned char pos;
  unsigned char len;
  std::list<LingInfo> lingInfos;
};

struct Concat
{
  Concat() : del(false),components() {};
  bool del;
  std::list<Component> components;
};

struct Entry
{
  Entry() : del(false),lingInfoPos(0),lingInfoLength(0),accented(),concatPos(0),concatLength(0) {};
  bool del;
  std::streampos lingInfoPos;
  unsigned short lingInfoLength;
  std::list<Accented> accented;
  std::streampos concatPos;
  unsigned short concatLength;
};

class DictionaryCompilerPrivate
{
  friend class DictionaryCompiler;

  DictionaryCompilerPrivate(
    LinguisticProcessing::FlatTokenizer::CharChart* charChart,
    Common::AbstractAccessByString* access,
    const std::map<std::string,LinguisticCode>& conversionMap,
    bool reverseKeys);

  ~DictionaryCompilerPrivate();
  DictionaryCompilerPrivate(const DictionaryCompilerPrivate&) = delete;
  DictionaryCompilerPrivate& operator=(const DictionaryCompilerPrivate&) = delete;

  void writeBinaryDictionary(std::ostream& out);

  bool parse(QIODevice *device);
  void readDictionary();
  void readEntry();
  void readConcat();
  void readC();
  void readI();
  void readP();

  QXmlStreamReader m_reader;

  /* resources and parameters */
  LinguisticProcessing::FlatTokenizer::CharChart* m_charChart;
  Common::AbstractAccessByString* m_access;
  const std::map<std::string,LinguisticCode>& m_conv;
  bool m_reverseKeys;

  /** Write the 64 bits integer @ref number on the @ref out stream with the
   * minimum number of bytes possible
  */
  unsigned char writeCodedInt(std::ostream& out,uint64_t number);
  /** Computes the minimum number of bytes possible to encode the given 64 bits
   * number @ref number
   */
  unsigned char sizeOfCodedInt(uint64_t number) const;
  void placeEntryDataIntoCharBuf(std::streampos pos,uint64_t len);
  uint64_t placeLingPropsIntoCharBuf(const std::vector<LinguisticCode>& lingProps);

  /** buffer for coded int */
  char* m_codedintbuf=new char[CODED_INT_BUFFER_SIZE];
  char* m_charbuf=new char[BUFFER_SIZE];
  uint64_t m_charbufSize = BUFFER_SIZE;

  ///@{
  /** Symbolic names for XML tags */
  const QString S_DICTIONARY = "dictionary";
  const QString S_ENTRY = "entry";
  const QString S_K = "k";
  const QString S_I = "i";
  const QString S_L = "l";
  const QString S_N = "n";
  const QString S_P = "p";
  const QString S_V = "v";
  const QString S_CONCAT = "concat";
  const QString S_C = "c";
  const QString S_FORM = "form";
  const QString S_POS = "pos";
  const QString S_DESACC = "desacc";
  const QString S_YES = "yes";
  const QString S_NO = "no";
  const QString S_OP = "op";
  const QString S_REPLACE = "replace";
  const QString S_DELETE = "delete";
  const QString S_ADD = "add";
  ///@}

  /* state attributes, used when parsing input file*/
  std::vector<LingInfo> m_lingInfosStack;
  std::vector<Concat> m_concatStack;

  Entry* m_currentEntry;
  LingInfo* m_currentLingInfo;
  uint64_t m_currentIndex;
  LimaString m_currentKey;
  bool m_inConcat = false;
  bool m_inDeleteEntry = false;
  bool m_inDeleteLingInfo = false;
  bool m_inDeleteConcat = false;
  uint64_t m_nextComponentPos;
  uint64_t m_count = 0;
  std::vector<LinguisticCode> m_currentLingProps;

  /* building data*/
  std::stringstream m_entryData;
  // to ensure the 'no ling prop' vector is id 0;
  std::map<std::vector<LinguisticCode>,uint64_t> m_lingProps = { { {}, 0 } };
;
  std::vector<Entry> m_entries;
  Entry m_invalidEntry;

  /* cache access to FsaAccess */
  std::list<std::pair<LimaString,uint64_t> > m_strCache;
  uint64_t getStringIndex(const LimaString& str);

};

DictionaryCompilerPrivate::DictionaryCompilerPrivate(
  LinguisticProcessing::FlatTokenizer::CharChart* charChart,
  AbstractAccessByString* access,
  const std::map<std::string,LinguisticCode>& conversionMap,
  bool reverseKeys) :
    m_charChart(charChart),
    m_access(access),
    m_conv(conversionMap),
    m_reverseKeys(reverseKeys),
    m_entryData(std::ios::in | std::ios::out | std::ios::binary),
    m_strCache()
{
  m_entries.resize(access->getSize());
  // set cache size
  m_strCache.resize(CODED_INT_BUFFER_SIZE, std::make_pair(LimaString(), 0));
}

DictionaryCompilerPrivate::~DictionaryCompilerPrivate()
{
  delete[] m_codedintbuf;
  delete[] m_charbuf;
}

DictionaryCompiler::DictionaryCompiler(
  LinguisticProcessing::FlatTokenizer::CharChart* charChart,
  AbstractAccessByString* access,
  const std::map<std::string,LinguisticCode>& conversionMap,
  bool reverseKeys) :
    m_d(new DictionaryCompilerPrivate(charChart,
                                      access,
                                      conversionMap,
                                      reverseKeys))
{
}

bool DictionaryCompiler::parse(QIODevice *device)
{
  return m_d->parse(device);
}

// <dictionary>
// <entry k="$">
//   <i l="dollar">
//     <p v="NC:m--"/>
//   </i>
//   …
// </entry>
// …
bool DictionaryCompilerPrivate::parse(QIODevice *device)
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "parse";
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

// <dictionary>
// <entry k="$">
//   <i l="dollar">
//     <p v="NC:m--"/>
//   </i>
//   …
// </entry>
// …
void DictionaryCompilerPrivate::readDictionary()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readDictionary" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("dictionary"));
#endif
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("entry"))
          readEntry();
      else
          m_reader.raiseError(QObject::tr("Expected an entry but got a %1.").arg(m_reader.name()));
  }
}

// <entry k="$">
//   <i l="dollar">
//     <p v="NC:m--"/>
//   </i>
//   …
// </entry>
void DictionaryCompilerPrivate::readEntry()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readEntry" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("entry"));
#endif
  auto k = m_reader.attributes().value(S_K);
  if (k.isEmpty())
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : no attribute 'k' for entry !";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
  }
  m_currentKey = k.toString();
  if (m_reverseKeys)
  {
    std::reverse(m_currentKey.begin(), m_currentKey.end());
  }
//     LDEBUG << "DictionaryCompiler::startElement read entry " << m_currentKey;
  m_currentIndex = getStringIndex(m_currentKey);
//     LDEBUG << "index = " << m_currentIndex;
  if (m_currentIndex == 0)
  {
    ANALYSISDICTLOGINIT;
    LERROR << "ERROR : key '" << m_currentKey
            << "' is not in accessKeys ! ignore it" ;
    m_currentEntry = &m_invalidEntry;
  }
  else
  {
    m_currentEntry = &m_entries[m_currentIndex];
    if (m_currentEntry->lingInfoPos>0 || m_currentEntry->concatPos>0)
    {
      ANALYSISDICTLOGINIT
      LERROR << "WARNING : entry " << limastring2utf8stdstring(m_currentKey)
              << " already exists ! replace first";
    }
  }
  m_currentEntry->del = false;
  auto op = m_reader.attributes().value(S_OP);
  auto hasToDesaccentuate = true;
  m_inDeleteEntry = false;
  if (!op.isEmpty())
  {
    if (op == S_REPLACE)
    {
      m_currentEntry->del = true;
    }
    else if (op == S_DELETE)
    {
      m_currentEntry->del = true;
      hasToDesaccentuate = false;
      m_inDeleteEntry = true;
    }
    else if (op != S_ADD)
    {
      ANALYSISDICTLOGINIT;
      LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
    }
  }
  if (hasToDesaccentuate)
  {
    auto desacc = m_reader.attributes().value(S_DESACC);
    auto desaccstr = m_charChart->unmark(m_currentKey);
    if (!desaccstr.isEmpty() && desaccstr != m_currentKey)
    {
#ifdef DEBUG_LP
      LDEBUG << "DictionaryCompiler::startElement read desacc "
              << m_currentKey << " => " << desaccstr;
#endif
      auto desaccIndex = getStringIndex(desaccstr);
      if (desaccIndex != 0)
      {
        Accented acc;
        acc.id = m_currentIndex;
        if (desacc.isEmpty() || desacc==S_YES)
        {
          acc.del = false;
        }
        else if (!desacc.isEmpty() && desacc == S_NO)
        {
          acc.del = true;
        }
        else
        {
          ANALYSISDICTLOGINIT;
          LERROR << "ERROR : invalid attribute desacc=\"" << desacc
                  << "\" ! ignore it";
        }
        m_entries[desaccIndex].accented.push_back(acc);
      }
    }
  }
  m_count++;
  if ((m_count % 10000) == 0)
  {
    std::cout << "\rbuild data : " << m_count << " entries ...";
  }


  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("i"))
          readI();
      else if (m_reader.name() == QLatin1String("concat"))
          readConcat();
      else
          m_reader.raiseError(QObject::tr("Expected an i but got a %1.").arg(m_reader.name()));
  }

  // encode linginfo
  m_currentEntry->lingInfoPos = m_entryData.tellp();
  uint64_t len = 0;
  for (const auto& lingInfo : m_lingInfosStack)
  {
    if (lingInfo.del)
    {
      len += writeCodedInt(m_entryData, 0);
    }
    len += writeCodedInt(m_entryData, lingInfo.lemma);
    len += writeCodedInt(m_entryData, lingInfo.norm);
    len += writeCodedInt(m_entryData, lingInfo.lingProps);
  }
  m_currentEntry->lingInfoLength = len;
  m_lingInfosStack.clear();

  // encode concats
  m_currentEntry->concatPos = m_entryData.tellp();
  uint64_t concatLength = 0;
  for (const auto& concat : m_concatStack)
  {
    if (concat.del)
    {
      concatLength += writeCodedInt(m_entryData, 0);
    }
    concatLength += writeCodedInt(m_entryData, concat.components.size());
    for (const auto& component : concat.components)
    {
      concatLength += writeCodedInt(m_entryData, component.form);
      concatLength += writeCodedInt(m_entryData, component.pos);
      concatLength += writeCodedInt(m_entryData, component.len);

      uint64_t lilength = 0;
      for (const auto& lingInfo : component.lingInfos)
      {
        if (lingInfo.del)
        {
          ANALYSISDICTLOGINIT;
          LERROR << "ERROR : entry '" << limastring2utf8stdstring(m_currentKey)
                  << "' has a component with a delete lingInfo ! ignore it";
        }
        lilength += sizeOfCodedInt(lingInfo.lemma);
        lilength += sizeOfCodedInt(lingInfo.norm);
        lilength += sizeOfCodedInt(lingInfo.lingProps);
      }

      concatLength += writeCodedInt(m_entryData, lilength);
      for (const auto& lingInfo : component.lingInfos)
      {
        concatLength += writeCodedInt(m_entryData, lingInfo.lemma);
        concatLength += writeCodedInt(m_entryData, lingInfo.norm);
        concatLength += writeCodedInt(m_entryData, lingInfo.lingProps);
      }
    }
  }
  m_currentEntry->concatLength = concatLength;
  m_concatStack.clear();
  m_inDeleteEntry = false;
}

// <entry k="nouveau-phoenix" desacc="no">
//   <concat>
//     <c form="nouveau">
//       <i l="nouveau">
//         <p v="ADJ:-fs"/>
//       </i>
//     </c>
//     …
//   </concat>
// </entry>
void DictionaryCompilerPrivate::readConcat()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readConcat" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("concat"));
#endif
  if (m_inDeleteEntry)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : found concat in delete entry " << m_currentKey
        << " ! data can be inconsistant";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
  }
  m_inDeleteConcat = false;
  m_concatStack.push_back(Concat());
  auto op = m_reader.attributes().value(S_OP);
  if (!op.isEmpty())
  {
    if (op == S_REPLACE )
    {
      m_concatStack.back().del = true;
    }
    else if (op == S_DELETE)
    {
      m_concatStack.back().del = true;
      m_inDeleteConcat = true;
    }
    else if (op != S_ADD)
    {
      ANALYSISDICTLOGINIT;
      LERROR << "ERROR : invalid attribute op=\"" << op << "\" for tag concat ! ignore it" ;
    }
  }
  m_inConcat = true;
  m_nextComponentPos = 0;

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("c"))
          readC();
      else
          m_reader.raiseError(QObject::tr("Expected an i but got a %1.").arg(m_reader.name()));
  }

}

//     <c form="nouveau">
//       <i l="nouveau">
//         <p v="ADJ:-fs"/>
//       </i>
//     </c>
void DictionaryCompilerPrivate::readC()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readC" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("c"));
#endif
  if (m_inDeleteConcat)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : found component in delete concatenated for entry "
        << m_currentKey << " ! data can be inconsistant";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
  }
  auto form = m_reader.attributes().value(S_FORM);
  if (form.isEmpty())
  {
    ANALYSISDICTLOGINIT;
    LERROR << "ERROR : tag <c> has no attribute 'form' !";
  }
  auto formStr = form.toString();
  //    cerr << "read componant form " << limastring2utf8stdstring(m_currentKey) << endl;
  auto formStrIndex = getStringIndex(formStr);
  if (formStrIndex == 0)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : form '" << formStr
          << "' not found in access keys ! data will be incorrect";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());

//       formStrIndex=1;
  }
  if (m_reverseKeys)
  {
    std::reverse(formStr.begin(), formStr.end());
  }
  auto position = m_currentKey.indexOf(formStr,m_nextComponentPos);
  if (m_reverseKeys)
  {
    // nextComponentPos constraint is a bit more complicated
    position = m_currentKey.indexOf(formStr,
                                    m_currentKey.size() - m_nextComponentPos - formStr.size());
  }
  if (position == -1)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : component '" << formStr << "' doesn't match in key '"
        << m_currentKey << "' ! data will be incorrect";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
//       position=0;
  }
  auto pos = m_reader.attributes().value(S_POS);
  if (!pos.isEmpty())
  {
    position = pos.toInt();
  }
  if (m_currentKey.indexOf(formStr, position) != position)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR component '" << formStr << "' not found at position "
        << position << " in key '" << m_currentKey
        << "' ! data will be incorrect";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
  }
  if (m_reverseKeys)
  {
    position = m_currentKey.size() - position - formStr.size();
  }
  auto& concat = m_concatStack.back();
  concat.components.push_back(Component());
  auto& component = concat.components.back();
  component.form = formStrIndex;
  component.pos = position;
  component.len = formStr.size();
  m_nextComponentPos = component.pos+component.len;
  //    cerr << "read c form=" << formStrIndex << " pos=" << position << " len=" << formStr.size() << endl;

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("i"))
          readI();
      else
          m_reader.raiseError(QObject::tr("readC: Expected an i but got a %1.").arg(m_reader.name()));
  }

}

//   <i l="dollar">
//     <p v="NC:m--"/>
//     …
//   </i>
void DictionaryCompilerPrivate::readI()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readI" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("i"));
#endif
  if (m_inDeleteEntry)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : found linginfo in delete entry : " << m_currentKey << " ! entry data will be inconsistant ! ";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    throw std::runtime_error(errorString.toStdString());
  }
  uint64_t currentFormIndex(0);
  if (m_inConcat)
  {
    m_concatStack.back().components.back().lingInfos.push_back(LingInfo());
    m_currentLingInfo = &m_concatStack.back().components.back().lingInfos.back();
    currentFormIndex = m_concatStack.back().components.back().form;
  }
  else
  {
    m_lingInfosStack.push_back(LingInfo());
    m_currentLingInfo = &m_lingInfosStack.back();
    currentFormIndex = m_currentIndex;
  }
  auto op = m_reader.attributes().value(S_OP);
  m_inDeleteLingInfo = false;
  if (!op.isEmpty())
  {
    if (op == S_REPLACE)
    {
      m_currentLingInfo->del = true;
    }
    else if (op == S_DELETE)
    {
      m_currentLingInfo->del = true;
      m_inDeleteLingInfo = true;
    }
    else if (op != S_ADD)
    {
      ANALYSISDICTLOGINIT;
      LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
    }
  }
  auto l = m_reader.attributes().value(S_L);
  if (!l.isEmpty())
  {
    auto lemma = l.toString();
//       std::cerr << "read lemma " << limastring2utf8stdstring(lemma) << std::endl;
    auto lemmaIndex = getStringIndex(lemma);
    //      if (lemmaIndex!=m_currentIndex)
    //      {
    m_currentLingInfo->lemma = lemmaIndex;
    //      }
  }
  else
  {
    m_currentLingInfo->lemma = currentFormIndex;
  }
  auto n = m_reader.attributes().value(S_N);
  if (!n.isEmpty())
  {
    auto norm = n.toString();
    //      cerr << "read norm " << limastring2utf8stdstring(norm) << endl;
    auto normIndex = getStringIndex(norm);
    if (normIndex != m_currentLingInfo->lemma)
    {
      m_currentLingInfo->norm = normIndex;
    }
  }

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("p"))
          readP();
      else
          m_reader.raiseError(QObject::tr("Expected a p but got a %1.").arg(m_reader.name()));
  }

  std::sort(m_currentLingProps.begin(), m_currentLingProps.end());
  auto value = std::make_pair(m_currentLingProps, m_lingProps.size());
  auto res = m_lingProps.insert(value);
  m_currentLingInfo->lingProps = res.first->second;
  m_currentLingProps.clear();
  m_currentLingInfo = 0;
  m_inDeleteLingInfo = false;

}

//     <p v="NC:m--"/>
void DictionaryCompilerPrivate::readP()
{
#ifdef DEBUG_LP
  ANALYSISDICTLOGINIT;
  LTRACE << "DictionaryCompilerPrivate::readP" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("p"));
#endif

  if (m_inDeleteLingInfo)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : found property in delete lingInfo for entry " << m_currentKey << " ! data can be inconsistant";
    ANALYSISDICTLOGINIT;
    LERROR << errorString;
    m_reader.raiseError(QObject::tr("ERROR : found property in delete lingInfo for entry %1 ! data can be inconsistant")
                          .arg(m_currentKey));
  }
  auto v = m_reader.attributes().value(S_V);
  if (v.isEmpty())
  {
    ANALYSISDICTLOGINIT;
    LERROR << "ERROR : tag <p> has no attribute 'v' for '"
            << m_currentKey << "'!";
    m_reader.raiseError(QObject::tr("ERROR : tag <p> has no attribute 'v' for '%1'!").arg(m_currentKey));
  }
  const auto& it = m_conv.find(v.toString().toStdString());
  if (it != m_conv.end())
  {
//       std::cerr << "DictionaryCompiler::startElement push in current ling prop: "
//                 << it->second << std::endl;
    m_currentLingProps.push_back(it->second);
  }
  else
  {
    ANALYSISDICTLOGINIT;
    LERROR << "Invalid property '" << v << "' for '" << m_currentKey << "', ignore it !";
  }
  m_reader.skipCurrentElement();
}


void DictionaryCompiler::writeBinaryDictionary(std::ostream& out)
{
  m_d->writeBinaryDictionary(out);
}

void DictionaryCompilerPrivate::writeBinaryDictionary(std::ostream& out)
{

  // write dictionary entries data

  uint64_t nbEntries = m_entries.size();
  writeCodedInt(out, nbEntries);
  for (auto& entry : m_entries)
  {
    if (entry.del)
    {
      writeCodedInt(out, 1);
    }

    // write accented data to allow computing entry length
    auto accPos = m_entryData.tellp();
    uint64_t accLen = 0;
    entry.accented.sort();
    for (const auto& accented : entry.accented)
    {
      if (accented.del)
      {
        accLen += writeCodedInt(m_entryData,0);
      }
      accLen += writeCodedInt(m_entryData, accented.id);
    }
    entry.accented.clear();

    // compute and write entry length
    uint64_t entryLen = 0;
    if ((entry.lingInfoLength > 0) || (accLen > 0) || (entry.concatLength > 0))
    {
      entryLen += sizeOfCodedInt(entry.lingInfoLength) + entry.lingInfoLength;
    }
    if ((accLen > 0) || (entry.concatLength > 0))
    {
      entryLen += sizeOfCodedInt(accLen) + accLen;
    }
    if (entry.concatLength > 0)
    {
      entryLen += sizeOfCodedInt(entry.concatLength) + entry.concatLength;
    }
    writeCodedInt(out, entryLen);

    // write all datas
    if ((entry.lingInfoLength > 0) || (accLen > 0) || (entry.concatLength > 0))
    {
      writeCodedInt(out, entry.lingInfoLength);
      if (entry.lingInfoLength > 0)
      {
        placeEntryDataIntoCharBuf(entry.lingInfoPos, entry.lingInfoLength);
        out.write(m_charbuf, entry.lingInfoLength);
      }
    }
    if ((accLen > 0) || (entry.concatLength > 0))
    {
      writeCodedInt(out, accLen);
      if (accLen > 0)
      {
        placeEntryDataIntoCharBuf(accPos, accLen);
        out.write(m_charbuf, accLen);
      }
    }
    if (entry.concatLength > 0)
    {
      writeCodedInt(out, entry.concatLength);
      placeEntryDataIntoCharBuf(entry.concatPos, entry.concatLength);
      out.write(m_charbuf, entry.concatLength);
    }
  }
//   cerr << "wrote " << nbEntries << " entries" << endl;
  m_entries.clear();
  m_entryData.clear();

  // write linguisticPropertiesdata;
  std::vector< const std::vector<LinguisticCode>* > lingPropVec(m_lingProps.size());
  for (const auto& elem : m_lingProps)
  {
    lingPropVec[elem.second]=&elem.first;
  }
  writeCodedInt(out,lingPropVec.size());
  for (const auto& lingProp : lingPropVec)
  {
    uint64_t l = placeLingPropsIntoCharBuf(*lingProp);
    writeCodedInt(out, l);
    out.write(m_charbuf, l);
  }
//   std::cerr << "wrote " << lingPropVec.size() << " linguistic property set" << std::endl;
  lingPropVec.clear();
}

unsigned char DictionaryCompilerPrivate::writeCodedInt(std::ostream& out, uint64_t number)
{
  uint64_t n(number);
  unsigned char i = 0;
  do
  {
    if (i >= CODED_INT_BUFFER_SIZE)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "DictionaryCompiler::writeCodedInt number too large to be coded in buffer of size "
          << CODED_INT_BUFFER_SIZE << ": " << number;
      throw std::runtime_error(errorString.toStdString());
    }
    m_codedintbuf[i] = (n & 0x7F) << 1;
    n >>= 7;
    i++;
  }
  while (n); // need 5 bytes for values > 268435455

  for(unsigned char ii = i-1; ii>0; ii--)
  {
    m_codedintbuf[ii] |= 0x1;
    out.write((char*)&m_codedintbuf[ii], sizeof(char));
  }
  out.write((char*)&m_codedintbuf[0], sizeof(char));
  return i;
}

unsigned char DictionaryCompilerPrivate::sizeOfCodedInt(uint64_t number) const
{
  uint64_t n(number);
  unsigned char i=0;
  do
  {
    n >>= 7;
    i++;
  }
  while (n); // need 5 bytes for values > 268435455
  return i;
}

void DictionaryCompilerPrivate::placeEntryDataIntoCharBuf(std::streampos pos, uint64_t len)
{
  if (len>m_charbufSize)
  {
    delete [] m_charbuf;
    m_charbuf = new char[len];
    m_charbufSize = len;
  }
  m_entryData.seekg(pos);
  m_entryData.read(m_charbuf, len);
}

uint64_t DictionaryCompilerPrivate::placeLingPropsIntoCharBuf(const std::vector<LinguisticCode>& lingProps)
{
  uint64_t len = 0;
  for (const auto& lingProp : lingProps)
  {
    std::stringstream ss;
    LinguisticCode::encodeToBinary(ss, lingProp);
    for ( char c : ss.str() )
    {
      m_charbuf[len++] = c;
    }

  }
  return len;
}

uint64_t DictionaryCompilerPrivate::getStringIndex(const LimaString& str)
{
  // look at cache
  for (auto it = m_strCache.begin(); it!=m_strCache.end(); it++)
  {
    if (str == it->first)
    {
      m_strCache.splice(m_strCache.begin(), m_strCache, it);
      //      cerr << "cache success : " << limastring2utf8stdstring(str) << " => " << m_strCache.front().second << endl;
      return m_strCache.front().second;
    }
  }
  // if not found, look into fsa Access
  m_strCache.pop_back();
  m_strCache.push_front(std::make_pair(str, m_access->getIndex(str)));
  //  cerr << "cache mismatch : " << limastring2utf8stdstring(str) << " => " << m_strCache.front().second << endl;
  return m_strCache.front().second;
}


QString DictionaryCompiler::errorString() const
{
  ANALYSISDICTLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_d->m_reader.errorString())
          .arg(m_d->m_reader.lineNumber())
          .arg(m_d->m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

}
