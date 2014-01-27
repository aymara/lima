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

// NAUTITIA
//
// jys 1-OCT-2002
//
// LingInfoEntry is the way to access dictionary linguistic infos datas.
// LingInfoEntry manages recurent calls to lists of linguistic properties..
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS
////////////////////////////////////////////////////////////
// <lingInfoEntry> ::=    <Len4> <lemma> <normalized> <LingPropertiesOffset>
// <lemma> ::=      <0> | <StringOffset>
// <normalized> ::=      <0> | <StringOffset>
// <lingProperties> ::=    <Len7> { <LingPropertyEntry> }

#include "LingInfoEntry.h"

#include "common/misc/Exceptions.h"

using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

LingInfoEntry::LingInfoEntry() :
BinaryEntry(),
m_key(),
m_stringStartAddr(NULL),
m_lingPropertiesStartAddr(NULL),
m_startAddr(NULL),
m_endAddr(NULL),
m_lingProperty(NULL) {
}

// copy constructor. Copies the specified LingInfoEntry
// into self. Internal iteration pointers of self are reset
LingInfoEntry::LingInfoEntry(const LingInfoEntry& entry) :
BinaryEntry(),
m_key(entry.m_key),
m_stringStartAddr(entry.m_stringStartAddr),
m_lingPropertiesStartAddr(entry.m_lingPropertiesStartAddr),
m_startAddr(entry.m_startAddr),
m_endAddr(NULL),
m_lingProperty(NULL) {
    reset();
}

LingInfoEntry::LingInfoEntry(const LimaString& key, 
           unsigned char *stringStartAddr,
           unsigned char *lingPropertiesStartAddr,
                             unsigned char *startAddr) :
BinaryEntry(),
m_key(key),
m_stringStartAddr(stringStartAddr),
m_lingPropertiesStartAddr(lingPropertiesStartAddr),
m_startAddr(startAddr),
m_endAddr(NULL),
m_lingProperty(NULL) {
    reset();
}

// Destroys LingInfoEntry
LingInfoEntry::~LingInfoEntry() {
}

// Assignment operator. Copies the specified LingInfoEntry 
// into self. Returns a reference to self. 
// After assignment, internal iteration pointers of self are reset
LingInfoEntry& LingInfoEntry::operator=(const LingInfoEntry& lingInfo) {
    m_key = lingInfo.m_key;
    m_stringStartAddr = lingInfo.m_stringStartAddr;
    m_lingPropertiesStartAddr = lingInfo.m_lingPropertiesStartAddr;
    m_startAddr = lingInfo.m_startAddr;
    reset();
    return *this;
}

// Reset iteration pointer
// <lingInfoEntry> ::=    <Len4> <lemma> <normalized> <LingPropertiesOffset>
// <lemma> ::=      <0> | <StringOffset>
// <normalized> ::=      <0> | <StringOffset>
// <lingProperties> ::=    <Len7> { <LingPropertyEntry> }
LingInfoEntry& LingInfoEntry::reset() {
    if (m_startAddr != NULL) {
  unsigned char *ptr = m_startAddr;
  uint64_t num = getEncodedNumber(ptr);
  m_endAddr = num + ptr;  //<Len4>
  getEncodedNumber(ptr);          // skip <lemma>
  getEncodedNumber(ptr);          // skip <normalized>
  uint64_t offset = getEncodedNumber(ptr);  //<LingPropertiesOffset>
  ptr = m_lingPropertiesStartAddr + offset;
  if (offset == 0)         // when no ling properties
      m_endLingProperty = ptr;
  else
  {
    uint64_t num = getEncodedNumber(ptr);
      m_endLingProperty = num + ptr;  //<Len7>
  }
  m_lingProperty = ptr;  
    }
    else {
  m_endAddr = NULL;
  m_lingProperty = NULL;
    }
    return *this;
}

// Returns true if self is empty
bool LingInfoEntry::isEmpty() const {
    return (m_startAddr == NULL);
}

// Returns lemma string 
// If self is empty, EmptyEntryException is raised
// <lingInfoEntry> ::=    <Len4> <lemma> <normalized> <LingPropertiesOffset>
// <lemma> ::=        <0> | <StringOffset>
Lima::LimaString LingInfoEntry::lemma() const {
    if (m_startAddr == NULL) throw EmptyEntryException();
    unsigned char *ptr = m_startAddr;
    getEncodedNumber(ptr);          //skip <Len4>
    uint64_t offset =  getEncodedNumber(ptr);
    if (offset != 0)
  return getUtf8StringNoMove(m_stringStartAddr + offset);  //lemma 
    return m_key;            //key when lemma=0
}

// Returns normalized string 
// If self is empty, EmptyEntryException is raised
// <lingInfoEntry> ::=    <Len4> <lemma> <normalized> <LingPropertiesOffset>
// <lemma> ::=        <0> | <StringOffset>
// <normalized> ::=      <0> | <StringOffset>
Lima::LimaString LingInfoEntry::normalized() const {
    if (m_startAddr == NULL) throw EmptyEntryException();
    unsigned char *ptr = m_startAddr;
    getEncodedNumber(ptr);          //skip <Len4>
    unsigned char *ptrLemma = ptr;
    getEncodedNumber(ptr);          //skip lemma
    uint64_t offset =  getEncodedNumber(ptr);
    if (offset != 0)
  return getUtf8StringNoMove(m_stringStartAddr + offset);  //normalized 
    offset =  getEncodedNumber(ptrLemma);
    if (offset != 0)
  return getUtf8StringNoMove(m_stringStartAddr + offset);//lemma when normalized=0 
    return m_key;                    //key when normalized=lemma=0
}

// Gets the next LingPropertyEntry if exists
// This call "increments" the internal LingPropertyEntry pointer.
// If there is no LingPropertyEntry anymore, an empty LingPropertyEntry is returned
// If self is empty, EmptyEntryException is raised
LingPropertyEntry LingInfoEntry::nextLingProperty() {
  if (m_startAddr == NULL) throw EmptyEntryException();
  if (m_lingProperty < m_endLingProperty)
    return LingPropertyEntry(static_cast<LinguisticCode>(getEncodedNumber(m_lingProperty)));
  else
    return LingPropertyEntry();
}

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima
