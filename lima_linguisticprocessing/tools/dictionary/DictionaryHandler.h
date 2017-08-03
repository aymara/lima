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

#define BUFFER_SIZE 4096

namespace Lima
{

class DictionaryCompiler : public QXmlDefaultHandler
{
public:

  DictionaryCompiler(
    LinguisticProcessing::FlatTokenizer::CharChart* charChart,
    Common::AbstractAccessByString* access,
    const std::map<std::string,LinguisticCode>& conversionMap,
    bool reverseKeys);

  virtual ~DictionaryCompiler();

  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & atts);
  
  bool endElement(const QString& namespaceURI, const QString& name, const QString & qName);
  
  void writeBinaryDictionary(std::ostream& out);

private:

  /* resources and parameters */
  LinguisticProcessing::FlatTokenizer::CharChart* m_charChart;
  Common::AbstractAccessByString* m_access;
  const std::map<std::string,LinguisticCode>& m_conv;
  bool m_reverseKeys;

  /* usefull function */
  unsigned char writeCodedInt(std::ostream& out,uint64_t number);
  unsigned char sizeOfCodedInt(uint64_t number) const;
  void placeEntryDataIntoCharBuf(std::streampos pos,uint64_t len);
  uint64_t placeLingPropsIntoCharBuf(const std::vector<LinguisticCode>& lingProps);

  /* buffer for coded int */
  char* m_codedintbuf;
  char* m_charbuf;
  uint64_t m_charbufSize;

  /* constant */
  QString S_DICTIONARY;
  QString S_ENTRY;
  QString S_K;
  QString S_I;
  QString S_L;
  QString S_N;
  QString S_P;
  QString S_V;
  QString S_CONCAT;
  QString S_C;
  QString S_FORM;
  QString S_POS;
  QString S_DESACC;
  QString S_YES;
  QString S_NO;
  QString S_OP;
  QString S_REPLACE;
  QString S_DELETE;
  QString S_ADD;

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

  /* state attributes, used when parsing input file*/
  std::vector<LingInfo> m_lingInfosStack;
  std::vector<Concat> m_concatStack;

  Entry* m_currentEntry;
  LingInfo* m_currentLingInfo;
  uint64_t m_currentIndex;
  LimaString m_currentKey;
  bool m_inConcat;
  bool m_inDeleteEntry;
  bool m_inDeleteLingInfo;
  bool m_inDeleteConcat;
  uint64_t m_nextComponentPos;
  uint64_t m_count;
  std::vector<LinguisticCode> m_currentLingProps;

  /* building data*/
  std::stringstream m_entryData;
  std::map<std::vector<LinguisticCode>,uint64_t> m_lingProps;
  std::vector<Entry> m_entries;
  Entry m_invalidEntry;
  
  /* cache access to FsaAccess */
  std::list<std::pair<LimaString,uint64_t> > m_strCache;
  uint64_t getStringIndex(const LimaString& str);

};

}

#endif
