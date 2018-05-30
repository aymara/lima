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

DictionaryCompiler::DictionaryCompiler(
  LinguisticProcessing::FlatTokenizer::CharChart* charChart,
  AbstractAccessByString* access,
  const std::map<std::string,LinguisticCode>& conversionMap,
  bool reverseKeys) :
    m_charChart(charChart),
    m_access(access),
    m_conv(conversionMap),
    m_reverseKeys(reverseKeys),
    m_inConcat(false),
    m_count(0),
    m_entryData(ios::in | ios::out | ios::binary),
    m_strCache()
{
  S_DICTIONARY="dictionary";
  S_ENTRY="entry";
  S_K="k";
  S_I="i";
  S_L="l";
  S_N="n";
  S_P="p";
  S_V="v";
  S_CONCAT="concat";
  S_C="c";
  S_FORM="form";
  S_POS="pos";
  S_DESACC="desacc";
  S_YES="yes";
  S_NO="no";
  S_OP="op";
  S_REPLACE="replace";
  S_DELETE="delete";
  S_ADD="add";
  m_entries.resize(access->getSize());
  m_codedintbuf=new char[5];
  m_charbuf=new char[BUFFER_SIZE];
  m_charbufSize=BUFFER_SIZE;

  // to ensure the 'no ling prop' vector is id 0;
  m_lingProps.insert(make_pair(std::vector<LinguisticCode>(),0));

  // set cache size
  m_strCache.resize(5,make_pair(LimaString(),0));
}

DictionaryCompiler::~DictionaryCompiler() 
{
  delete[] m_codedintbuf;
  delete[] m_charbuf;
}

bool DictionaryCompiler::startElement(const QString & namespaceURI, 
                                      const QString & name, 
                                      const QString & qName, 
                                      const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI)
  LIMA_UNUSED(qName)
  ANALYSISDICTLOGINIT;
//   LDEBUG << "startElement : " << name;
  if (name==S_ENTRY)
  {
    QString k=attributes.value(S_K);
    if (k==0)
    {
      cerr << "ERROR : no attribute 'k' for entry !" << endl;
      exit(-1);
    }
    m_currentKey=k;
    if (m_reverseKeys)
    {
      reverse(m_currentKey.begin(),m_currentKey.end());
    }
//     LDEBUG << "DictionaryCompiler::startElement read entry " << m_currentKey;
    m_currentIndex=getStringIndex(m_currentKey);
//     LDEBUG << "index = " << m_currentIndex;
    if (m_currentIndex == 0 || m_currentIndex >= m_entries.size())
    {
      LERROR << "ERROR : key '" << m_currentKey << "' is not in accessKeys ! ignore it" ;
      m_currentEntry = &m_invalidEntry;
    }
    else
    {
      m_currentEntry = &m_entries[m_currentIndex];
      if (m_currentEntry->lingInfoPos>0 || m_currentEntry->concatPos>0)
      {
        LERROR << "WARNING : entry " << limastring2utf8stdstring(m_currentKey) << " already exists ! replace first";
      }
    }
    m_currentEntry->del=false;
    QString op=attributes.value(S_OP);
    bool hasToDesaccentuate=true;
    m_inDeleteEntry=false;
    if (op != 0)
    {
      if (op==S_REPLACE)
      {
        m_currentEntry->del=true;
      }
      else if (op==S_DELETE)
      {
        m_currentEntry->del=true;
        hasToDesaccentuate=false;
        m_inDeleteEntry=true;
      }
      else if (op!=S_ADD)
      {
        LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
      }
    }
    if (hasToDesaccentuate)
    {
      QString desacc=attributes.value(S_DESACC);
      LimaString desaccstr=m_charChart->unmark(m_currentKey);
      if (desaccstr.size()>0 && desaccstr!=m_currentKey)
      {
#ifdef DEBUG_LP
        LDEBUG << "DictionaryCompiler::startElement read desacc " << m_currentKey << " => " << desaccstr;
#endif
        uint64_t desaccIndex=getStringIndex(desaccstr);
        if (desaccIndex == 0 || desaccIndex >= m_entries.size())
        {
          LERROR << "ERROR : desacc key '" << desaccIndex << "' is not in accessKeys ! ignore it" ;
        }
        else
        {
          Accented acc;
          acc.id=m_currentIndex;
          if (desacc == 0 || desacc==S_YES)
          {
            acc.del=false;
          }
          else if (desacc !=0 && desacc==S_NO)
          {
            acc.del=true;
          }
          else
          {
            LERROR << "ERROR : invalid attribute desacc=\"" << desacc << "\" ! ignore it";
          }
          m_entries[desaccIndex].accented.push_back(acc);
        }
      }
    }
    m_count++;
    if ((m_count % 10000)==0)
    {
      std::cout << "\rbuild data : " << m_count << " entries ...";
    }
  }
  else if (name==S_I)
  {
    if (m_inDeleteEntry)
    {
      LERROR << "ERROR : found linginfo in delete entry : " << m_currentKey << " ! entry data will be inconsistant ! ";
    }
    uint64_t currentFormIndex(0);
    if (m_inConcat)
    {
      m_concatStack.back().components.back().lingInfos.push_back(LingInfo());
      m_currentLingInfo=&m_concatStack.back().components.back().lingInfos.back();
      currentFormIndex=m_concatStack.back().components.back().form;
    }
    else
    {
      m_lingInfosStack.push_back(LingInfo());
      m_currentLingInfo=&m_lingInfosStack.back();
      currentFormIndex=m_currentIndex;
    }
    QString op=attributes.value(S_OP);
    m_inDeleteLingInfo=false;
    if (op != 0)
    {
      if (op==S_REPLACE || op==S_DELETE)
      {
        m_currentLingInfo->del=true;
        if (op==S_DELETE)
        {
          m_inDeleteLingInfo=true;
        }
      }
      else if (op!=S_ADD)
      {
        LERROR << "ERROR : invalid attribute op=\"" << op << "\" !";
      }
    }
    QString l=attributes.value(S_L);
    if (l != 0)
    {
      LimaString lemma=l;
      //      cerr << "read lemma " << limastring2utf8stdstring(lemma) << endl;
      int64_t lemmaIndex=getStringIndex(lemma);
      //      if (lemmaIndex!=m_currentIndex)
      //      {
      m_currentLingInfo->lemma=lemmaIndex;
      //      }
    }
    else
    {
      m_currentLingInfo->lemma=currentFormIndex;
    }
    QString n=attributes.value(S_N);
    if (n != 0)
    {
      LimaString norm=n;
      //      cerr << "read norm " << limastring2utf8stdstring(norm) << endl;
      uint64_t normIndex=getStringIndex(norm);
      if (normIndex!=m_currentLingInfo->lemma)
      {
        m_currentLingInfo->norm=normIndex;
      }
    }
  }
  else if (name==S_P)
  {
    if (m_inDeleteLingInfo)
    {
      LERROR << "ERROR : found property in delete lingInfo for entry " << m_currentKey << " ! data can be inconsistant";
    }
    QString v = attributes.value(S_V);
    if (v == 0)
    {
      LERROR << "ERROR : tag <p> has no attribute 'v' for '" 
              << m_currentKey << "'!";
      return false;
    }
    const auto & it = m_conv.find(v.toUtf8().constData());
    if (it != m_conv.end())
    {
      m_currentLingProps.push_back(it->second);
    }
    else
    {
      LERROR << "Invalid property '" << v << "' for '"
              << m_currentKey << "', ignore it !";
    }
  }
  else if (name==S_CONCAT)
  {
    if (m_inDeleteEntry)
    {
      LERROR << "ERROR : found concat in delete entry " << limastring2utf8stdstring(m_currentKey) << " ! data can be inconsistant";
    }
    m_inDeleteConcat=false;
    m_concatStack.push_back(Concat());
    QString op=attributes.value(S_OP);
    if ((op!=0) && (op==S_REPLACE || op==S_DELETE))
    {
      m_concatStack.back().del=true;
      if (op==S_DELETE)
      {
        m_inDeleteConcat=true;
      }
    }
    else if (op!=0 && op!=S_ADD)
    {
      LERROR << "ERROR : invalid attribute op=\"" << op << "\" for tag concat ! ignore it" ;
    }
    m_inConcat=true;
    m_nextComponentPos=0;
  }
  else if (name==S_C)
  {
    if (m_inDeleteConcat)
    {
      LERROR << "ERROR : found component in delete concatenated for entry " << m_currentKey << " ! data can be inconsistant";
    }
    QString form=attributes.value(S_FORM);
    if (form == 0)
    {
      LERROR << "ERROR : tag <c> has no attribute 'form' !";
    }
    LimaString formStr=form;
    //    cerr << "read componant form " << limastring2utf8stdstring(m_currentKey) << endl;
    uint64_t formStrIndex=getStringIndex(formStr);
    if (formStrIndex==0)
    {
      LERROR << "ERROR : form '" << formStr << "' not found in access keys ! data will be incorrect";
      formStrIndex=1;
    }
    if (m_reverseKeys)
    {
      reverse(formStr.begin(),formStr.end());
    }
    int position=m_currentKey.indexOf(formStr,m_nextComponentPos);
    if (m_reverseKeys)
    {
      // nextComponentPos constraint is a bit more complicated
      position=m_currentKey.indexOf(formStr,m_currentKey.size() - m_nextComponentPos - formStr.size());
    }
    if (position == -1)
    {
      LERROR << "ERROR : component '" << formStr << "' doesn't match in key '" << m_currentKey << "' ! data will be incorrect";
      position=0;
    }
    QString pos=attributes.value(S_POS);
    if (pos != 0)
    {
      position=pos.toInt();
    }
    if (m_currentKey.indexOf(formStr,position)!=position)
    {
      LERROR << "ERROR component '" << formStr << "' not found at position " << position << " in key '" << m_currentKey << "' ! data will be incorrect";
    }
    if (m_reverseKeys)
    {
      position = m_currentKey.size() - position - formStr.size();
    }
    Concat& concat=m_concatStack.back();
    concat.components.push_back(Component());
    Component& component=concat.components.back();
    component.form=formStrIndex;
    component.pos=position;
    component.len=formStr.size();
    m_nextComponentPos=component.pos+component.len;
    //    cerr << "read c form=" << formStrIndex << " pos=" << position << " len=" << formStr.size() << endl;
  }
  else if (name!=S_DICTIONARY)
  {
    LERROR << "ERROR : unknown open tag <" << name << "> in input file";
  }
  return true;
  
}

bool DictionaryCompiler::endElement(const QString& namespaceURI, 
                                    const QString& name, 
                                    const QString & qName)
{
  LIMA_UNUSED(namespaceURI)
  LIMA_UNUSED(qName)
  //  cerr << "endElement " << name << endl;
  if (name==S_ENTRY)
  {
    // encode linginfo
    m_currentEntry->lingInfoPos=m_entryData.tellp();
    uint64_t len=0;
    for (vector<LingInfo>::const_iterator it=m_lingInfosStack.begin();
         it!=m_lingInfosStack.end();
         it++)
    {
      if (it->del)
      {
        len+=writeCodedInt(m_entryData,0);
      }
      len+=writeCodedInt(m_entryData,it->lemma);
      len+=writeCodedInt(m_entryData,it->norm);
      len+=writeCodedInt(m_entryData,it->lingProps);
    }
    m_currentEntry->lingInfoLength=len;
    m_lingInfosStack.clear();

    // encode concats
    m_currentEntry->concatPos=m_entryData.tellp();
    len=0;
    for (vector<Concat>::iterator it=m_concatStack.begin();
         it!=m_concatStack.end();
         it++)
    {
      if (it->del)
      {
        len+=writeCodedInt(m_entryData,0);
      }
      len+=writeCodedInt(m_entryData,it->components.size());
      for (list<Component>::iterator cItr=it->components.begin();
           cItr!=it->components.end();
           cItr++)
      {
        len+=writeCodedInt(m_entryData,cItr->form);
        len+=writeCodedInt(m_entryData,cItr->pos);
        len+=writeCodedInt(m_entryData,cItr->len);

        uint64_t lilength=0;
        for (list<LingInfo>::const_iterator lItr=cItr->lingInfos.begin();
             lItr!=cItr->lingInfos.end();
             lItr++)
        {
          if (lItr->del)
          {
            cerr << "ERROR : entry '" << limastring2utf8stdstring(m_currentKey) << "' has a component with a delete lingInfo ! ignore it" << endl;
          }
          lilength+=sizeOfCodedInt(lItr->lemma);
          lilength+=sizeOfCodedInt(lItr->norm);
          lilength+=sizeOfCodedInt(lItr->lingProps);
        }

        len+=writeCodedInt(m_entryData,lilength);
        for (list<LingInfo>::const_iterator lItr=cItr->lingInfos.begin();
             lItr!=cItr->lingInfos.end();
             lItr++)
        {
          len+=writeCodedInt(m_entryData,lItr->lemma);
          len+=writeCodedInt(m_entryData,lItr->norm);
          len+=writeCodedInt(m_entryData,lItr->lingProps);
        }
      }
    }
    m_currentEntry->concatLength=len;
    m_concatStack.clear();
    m_inDeleteEntry=false;
  }
  else if (name==S_I)
  {
    sort(m_currentLingProps.begin(),m_currentLingProps.end());
    pair<vector<LinguisticCode>,uint64_t> value=make_pair(m_currentLingProps,m_lingProps.size());
    pair<map<vector<LinguisticCode>,uint64_t>::iterator,bool> res(m_lingProps.insert(value));
    m_currentLingInfo->lingProps=res.first->second;
    m_currentLingProps.clear();
    m_currentLingInfo=0;
    m_inDeleteLingInfo=false;
  }
  else if (name==S_CONCAT)
  {
    m_inConcat=false;
    m_inDeleteConcat=false;
  }
  else if (name==S_C)
  {}
  else if ((name!=S_P) && (name!=S_DICTIONARY))
  {
    cerr << "ERROR : unknown end tag </" << name << ">" << endl;
  }
  return true;
}

void DictionaryCompiler::writeBinaryDictionary(std::ostream& out)
{

  // write dictionary entries data

  auto nbEntries = m_entries.size();
  writeCodedInt(out,nbEntries);
  for (auto entryItr = m_entries.begin();
       entryItr != m_entries.end();
       entryItr++)
  {
    if (entryItr->del)
    {
      writeCodedInt(out,1);
    }

    // write accented data to allow computing entry length
    std::streampos accPos=m_entryData.tellp();
    uint64_t accLen=0;
    entryItr->accented.sort();
    for (list<Accented>::const_iterator accItr=entryItr->accented.begin();
         accItr!=entryItr->accented.end();
         accItr++)
    {
      if (accItr->del)
      {
        accLen+=writeCodedInt(m_entryData,0);
      }
      accLen+=writeCodedInt(m_entryData,accItr->id);
    }
    entryItr->accented.clear();

    // compute and write entry length
    uint64_t entryLen = 0;
    if ((entryItr->lingInfoLength > 0) || (accLen > 0) || (entryItr->concatLength > 0))
    {
      entryLen += sizeOfCodedInt(entryItr->lingInfoLength) + entryItr->lingInfoLength;
    }
    if ((accLen > 0) || (entryItr->concatLength > 0))
    {
      entryLen += sizeOfCodedInt(accLen) + accLen;
    }
    if (entryItr->concatLength > 0)
    {
      entryLen+=sizeOfCodedInt(entryItr->concatLength) + entryItr->concatLength;
    }
    writeCodedInt(out,entryLen);

    // write all datas
    if ((entryItr->lingInfoLength > 0) || (accLen > 0) || (entryItr->concatLength > 0))
    {
      writeCodedInt(out,entryItr->lingInfoLength);
      if (entryItr->lingInfoLength>0)
      {
        placeEntryDataIntoCharBuf(entryItr->lingInfoPos,entryItr->lingInfoLength);
        out.write(m_charbuf,entryItr->lingInfoLength);
      }
    }
    if ((accLen > 0) || (entryItr->concatLength > 0))
    {
      writeCodedInt(out,accLen);
      if (accLen>0)
      {
        placeEntryDataIntoCharBuf(accPos,accLen);
        out.write(m_charbuf,accLen);
      }
    }
    if (entryItr->concatLength > 0)
    {
      writeCodedInt(out,entryItr->concatLength);
      placeEntryDataIntoCharBuf(entryItr->concatPos,entryItr->concatLength);
      out.write(m_charbuf,entryItr->concatLength);
    }
  }
  cerr << "wrote " << nbEntries << " entries" << endl;
  m_entries.clear();
  m_entryData.clear();

  // write linguisticPropertiesdata;
  vector< const vector<LinguisticCode>* > lingPropVec(m_lingProps.size());
  for (std::map<std::vector<LinguisticCode>,uint64_t>::const_iterator lingItr=m_lingProps.begin();
       lingItr!=m_lingProps.end();
       lingItr++)
  {
    lingPropVec[lingItr->second]=&lingItr->first;
  }
  writeCodedInt(out,lingPropVec.size());
  for (vector<const vector<LinguisticCode>*>::const_iterator it=lingPropVec.begin();
       it!=lingPropVec.end();
       it++)
  {
    uint64_t l=placeLingPropsIntoCharBuf(**it);
    writeCodedInt(out,l);
    out.write(m_charbuf,l);
  }
  std::cerr << "wrote " << lingPropVec.size() << " linguistic property set" << std::endl;
  lingPropVec.clear();
}

unsigned char DictionaryCompiler::writeCodedInt(ostream& out,uint64_t number)
{
  uint64_t n(number);
  unsigned char i=0;
  do
  {
    m_codedintbuf[i]=(n & 0x7F) << 1;
    n >>= 7;
    i++;
  }
  while (n); // need 5 bytes for values > 268435455

  for(unsigned char ii=i-1;ii>0;ii--)
  {
    m_codedintbuf[ii] |= 0x1;
    out.write((char*)&m_codedintbuf[ii], sizeof(char));
  }
  out.write((char*)&m_codedintbuf[0], sizeof(char));
  return i;
}

unsigned char DictionaryCompiler::sizeOfCodedInt(uint64_t number) const
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

void DictionaryCompiler::placeEntryDataIntoCharBuf(streampos pos,uint64_t len)
{
  if (len>m_charbufSize)
  {
    delete [] m_charbuf;
    m_charbuf=new char[len];
    m_charbufSize=len;
  }
  m_entryData.seekg(pos);
  m_entryData.read(m_charbuf,len);
}

uint64_t DictionaryCompiler::placeLingPropsIntoCharBuf(const std::vector<LinguisticCode>& lingProps)
{
  //  cerr << "placeLingPropsIntoCharBuf size=" << lingProps.size() << endl;
  uint64_t len=0;
  for (vector<LinguisticCode>::const_iterator it=lingProps.begin();
       it!=lingProps.end();
       it++)
  {
    // coded int should never exceed 5 byte, so 10 bound size should works well ;-)
    if (len + 10 > m_charbufSize)
    {
      cerr << "info : has to increase buffer size" << endl;
      m_charbufSize*=2;
      char* newbuf=new char[m_charbufSize];
      memcpy(newbuf,m_charbuf,len);
      delete [] m_charbuf;
      m_charbuf=newbuf;
    }

    uint64_t n(*it);
    //    cerr << "code lingProp " << n << endl;
    unsigned char i=0;
    do
    {
      m_codedintbuf[i]=(n & 0x7F) << 1;
      n >>= 7;
      i++;
      //      cerr << "byte " << i-1 << " is " << (unsigned short)m_codedintbuf[i-1] << " now n=" << n << endl;
    }
    while (n); // need 5 bytes for values > 268435455
    //    cerr << "use " << (int)i << " bytes" << endl;
    i--;
    for(;i>0;i--)
    {
      //      cerr << "tmp[i]=" << (unsigned short)m_codedintbuf[i] << endl;
      m_charbuf[len++] = m_codedintbuf[i] | 0x1;
      //      cerr << "wrote char " << (int)i << " : " << (unsigned short)m_charbuf[len-1] << endl;
    }
    m_charbuf[len++] = m_codedintbuf[0];
    //    cerr << "wrote char 0 : " << (unsigned short)m_codedintbuf[0] << endl;
  }
  //  cerr << "return len=" << len << endl;
  return len;
}

uint64_t DictionaryCompiler::getStringIndex(const LimaString& str)
{
  // look at cache
  for (auto it = m_strCache.begin(); it != m_strCache.end(); it++)
  {
    if (str == it->first)
    {
      m_strCache.splice(m_strCache.begin(),m_strCache,it);
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
