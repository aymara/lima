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

#include "DictionaryHandler.h"

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <algorithm>

using namespace std;
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

  bool startElement(const QString & namespaceURI,
                    const QString & name,
                    const QString & qName,
                    const QXmlAttributes & atts);

  bool endElement(const QString& namespaceURI,
                  const QString& name,
                  const QString & qName);

  void writeBinaryDictionary(std::ostream& out);

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
    m_entryData(ios::in | ios::out | ios::binary),
    m_strCache()
{
  m_entries.resize(access->getSize());
  // set cache size
  m_strCache.resize(CODED_INT_BUFFER_SIZE,make_pair(LimaString(), 0));
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
    QXmlDefaultHandler(),
    m_d(new DictionaryCompilerPrivate(charChart,
                                      access,
                                      conversionMap,
                                      reverseKeys))
{
}

bool DictionaryCompiler::startElement(const QString & namespaceURI,
                                      const QString & name,
                                      const QString & qName,
                                      const QXmlAttributes & attributes)
{
  return m_d->startElement(namespaceURI, name, qName, attributes);
}

bool DictionaryCompilerPrivate::startElement(const QString & namespaceURI,
                                      const QString & name,
                                      const QString & qName,
                                      const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI)
  LIMA_UNUSED(qName)
  ANALYSISDICTLOGINIT;
//   LDEBUG << "startElement : " << name;
  if (name == S_ENTRY)
  {
    auto k = attributes.value(S_K);
    if (k.isEmpty())
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : no attribute 'k' for entry !";
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());
    }
    m_currentKey = k;
    if (m_reverseKeys)
    {
      reverse(m_currentKey.begin(), m_currentKey.end());
    }
//     LDEBUG << "DictionaryCompiler::startElement read entry " << m_currentKey;
    m_currentIndex=getStringIndex(m_currentKey);
//     LDEBUG << "index = " << m_currentIndex;
    if (m_currentIndex == 0)
    {
      LERROR << "ERROR : key '" << m_currentKey
              << "' is not in accessKeys ! ignore it" ;
      m_currentEntry = &m_invalidEntry;
    }
    else
    {
      m_currentEntry = &m_entries[m_currentIndex];
      if (m_currentEntry->lingInfoPos>0 || m_currentEntry->concatPos>0)
      {
        LERROR << "WARNING : entry " << limastring2utf8stdstring(m_currentKey)
                << " already exists ! replace first";
      }
    }
    m_currentEntry->del = false;
    auto op = attributes.value(S_OP);
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
        LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
      }
    }
    if (hasToDesaccentuate)
    {
      auto desacc = attributes.value(S_DESACC);
      auto desaccstr = m_charChart->unmark(m_currentKey);
      if (desaccstr.size() > 0 && desaccstr != m_currentKey)
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
  }
  else if (name == S_I)
  {
    if (m_inDeleteEntry)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : found linginfo in delete entry : " << m_currentKey << " ! entry data will be inconsistant ! ";
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
    auto op = attributes.value(S_OP);
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
        LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
      }
    }
    auto l = attributes.value(S_L);
    if (!l.isEmpty())
    {
      auto lemma = l;
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
    auto n = attributes.value(S_N);
    if (!n.isEmpty())
    {
      auto norm = n;
      //      cerr << "read norm " << limastring2utf8stdstring(norm) << endl;
      auto normIndex = getStringIndex(norm);
      if (normIndex != m_currentLingInfo->lemma)
      {
        m_currentLingInfo->norm = normIndex;
      }
    }
  }
  else if (name == S_P)
  {
    if (m_inDeleteLingInfo)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : found property in delete lingInfo for entry "
          << m_currentKey << " ! data can be inconsistant";
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());
    }
    QString v = attributes.value(S_V);
    if (v.isEmpty())
    {
      LERROR << "ERROR : tag <p> has no attribute 'v' for '"
              << m_currentKey << "'!";
      return false;
    }
    const auto& it = m_conv.find(Common::Misc::limastring2utf8stdstring(v));
    if (it != m_conv.end())
    {
//       std::cerr << "DictionaryCompiler::startElement push in current ling prop: "
//                 << it->second << std::endl;
      m_currentLingProps.push_back(it->second);
    }
    else
    {
      LERROR << "Invalid property '" << v << "' for '"
              << m_currentKey << "', ignore it !";
    }
  }
  else if (name == S_CONCAT)
  {
    if (m_inDeleteEntry)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : found concat in delete entry " << m_currentKey
          << " ! data can be inconsistant";
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());
    }
    m_inDeleteConcat = false;
    m_concatStack.push_back(Concat());
    auto op = attributes.value(S_OP);
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
        LERROR << "ERROR : invalid attribute op=\"" << op
                << "\" for tag concat ! ignore it" ;
      }
    }
    m_inConcat = true;
    m_nextComponentPos = 0;
  }
  else if (name == S_C)
  {
    if (m_inDeleteConcat)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : found component in delete concatenated for entry "
          << m_currentKey << " ! data can be inconsistant";
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());
    }
    auto form = attributes.value(S_FORM);
    if (form.isEmpty())
    {
      LERROR << "ERROR : tag <c> has no attribute 'form' !";
    }
    auto formStr = form;
    //    cerr << "read componant form " << limastring2utf8stdstring(m_currentKey) << endl;
    auto formStrIndex = getStringIndex(formStr);
    if (formStrIndex == 0)
    {
      QString errorString;
      QTextStream qts(&errorString);
      qts << "ERROR : form '" << formStr
            << "' not found in access keys ! data will be incorrect";
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());

//       formStrIndex=1;
    }
    if (m_reverseKeys)
    {
      reverse(formStr.begin(), formStr.end());
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
      LERROR << errorString;
      throw std::runtime_error(errorString.toStdString());
//       position=0;
    }
    auto pos = attributes.value(S_POS);
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
  }
  else if (name != S_DICTIONARY)
  {
    QString errorString;
    QTextStream qts(&errorString);
    qts << "ERROR : unknown open tag <" << name << "> in input file";
    LERROR << errorString;
  }
  return true;

}

bool DictionaryCompiler::endElement(const QString& namespaceURI,
                                    const QString& name,
                                    const QString & qName)
{
  return m_d->endElement(namespaceURI, name, qName);
}

bool DictionaryCompilerPrivate::endElement(const QString& namespaceURI,
                                    const QString& name,
                                    const QString & qName)
{
  //  cerr << "endElement " << name << endl;
  if (name == S_ENTRY)
  {
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
  else if (name == S_I)
  {
    std::sort(m_currentLingProps.begin(), m_currentLingProps.end());
    auto value = std::make_pair(m_currentLingProps, m_lingProps.size());
    auto res = m_lingProps.insert(value);
    m_currentLingInfo->lingProps = res.first->second;
    m_currentLingProps.clear();
    m_currentLingInfo = 0;
    m_inDeleteLingInfo = false;
  }
  else if (name == S_CONCAT)
  {
    m_inConcat = false;
    m_inDeleteConcat = false;
  }
  else if (name == S_C)
  {
  }
  else if ((name != S_P) && (name != S_DICTIONARY))
  {
    ANALYSISDICTLOGINIT;
    LERROR << "ERROR : unknown end tag </" << name << ">";
  }
  return true;
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

unsigned char DictionaryCompilerPrivate::writeCodedInt(ostream& out, uint64_t number)
{
  uint64_t n(number);
  unsigned char i = 0;
  do
  {
    if (i >= CODED_INT_BUFFER_SIZE)
    {
      ANALYSISDICTLOGINIT;
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

void DictionaryCompilerPrivate::placeEntryDataIntoCharBuf(streampos pos, uint64_t len)
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
//   std::cerr << "placeLingPropsIntoCharBuf size=" << lingProps.size() << std::endl;
  uint64_t len = 0;
  for (const auto& lingProp : lingProps)
  {
    // coded int should never exceed CODED_INT_BUFFER_SIZE byte,
    // so CODED_INT_BUFFER_SIZE + 5 bound size should works well ;-)
    if (len + CODED_INT_BUFFER_SIZE + 5 > m_charbufSize)
    {
//       std::cerr << "info : has to increase buffer size" << std::endl;
      m_charbufSize *= 2;
      char* newbuf = new char[m_charbufSize];
      memcpy(newbuf, m_charbuf, len);
      delete [] m_charbuf;
      m_charbuf = newbuf;
    }

//     std::cerr << "code lingProp " << lingProp << std::endl;
    uint64_t n = lingProp;
    unsigned char i=0;
    do
    {
      if (i >= CODED_INT_BUFFER_SIZE)
      {
        ANALYSISDICTLOGINIT;
        QString errorString;
        QTextStream qts(&errorString);
        qts << "DictionaryCompiler::placeLingPropsIntoCharBuf number too large to be coded in buffer of size "
            << CODED_INT_BUFFER_SIZE << ": " << lingProp;
        throw std::runtime_error(errorString.toStdString());
      }
      m_codedintbuf[i] = (n & 0x7F) << 1;
      n >>= 7;
      i++;
//       std::cerr << "byte " << i-1 << " is "
//                 << (unsigned short)m_codedintbuf[i-1]
//                 << " now n=" << n << std::endl;
    } while (n); // need 5 bytes for values > 268435455
//     std::cerr << "use " << (int)i << " bytes" << std::endl;
    i--;
    for(;i>0;i--)
    {
//       std::cerr << "tmp[i]=" << (unsigned short)m_codedintbuf[i] << std::endl;
      m_charbuf[len++] = m_codedintbuf[i] | 0x1;
//       std::cerr << "wrote char " << (int)i << " : "
//                 << (unsigned short)m_charbuf[len-1] << std::endl;
    }
    m_charbuf[len++] = m_codedintbuf[0];
//     std::cerr << "wrote char 0 : " << (unsigned short)m_codedintbuf[0] << std::endl;
  }
//   std::cerr << "return len=" << len << std::endl;
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
  m_strCache.push_front(make_pair(str, m_access->getIndex(str)));
  //  cerr << "cache mismatch : " << limastring2utf8stdstring(str) << " => " << m_strCache.front().second << endl;
  return m_strCache.front().second;
}


}
