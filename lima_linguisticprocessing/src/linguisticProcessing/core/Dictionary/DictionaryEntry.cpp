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
/**
 NAUTITIA

 jys 3-OCT-2002

 DictionaryEntry is the way to access dictionary datas
 DictionaryEntry manages recurent calls to lists of properties.
 Dictionaries in memory are specified in NA2002.r2128 JYS
 Dictionaries iterators are specified in NA2002.r2129 JYS
*/
// <dictionaryEntry> ::=  <Len0> <infos>
// <infos> ::=        <lingInfo> [<accented> [<idiomatic> [<hyphenLingInfo> 
//                [<concatenated> [<byDefault> ]]]]]
// <byDefault> ::=      <LingPropertiesOffset>
// <concatenated> ::=    <Len3> { <concatenatedEntry> }
// <concatenatedEntry> ::=  <Len9> { <Component> <dictionaryEntry> }
// <lingInfo> ::=      <Len1> { <lingInfoEntry> }
// <hyphenLingInfo> ::=    <Len1> { <lingInfoEntry> }
// <lingInfoEntry> ::=    <Len4> <lemma> <normalized> <LingPropertiesOffset>
// <lemma> ::=        <0> | <StringOffset>
// <normalized> ::=      <0> | <StringOffset>
// <accented> ::=      <Len2> { <accentedEntry> }
// <accentedEntry> ::=    <StringOffset>
// <idiomatic> ::=      <len11> { <IdiomaticEntry> }
// <lingProperties> ::=    <Len7> { <LingPropertyEntry> }
////////////////////////////////////////////////////////////

#include "DictionaryEntry.h"

#include "common/misc/Exceptions.h"
#include <memory.h>
#include <iostream>

using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {


/** creates DictionaryEntry */
DictionaryEntry::DictionaryEntry() :
        BinaryEntry(),
        m_key(),
        m_stringStartAddr(NULL),
        m_lingPropertiesStartAddr(NULL),
        m_startAddr(NULL),
        m_endAddr(NULL),
        m_lingInfo(NULL),
        m_endLingInfo(NULL),
        m_accented(NULL),
        m_endAccented(NULL),
        m_idiomatic(NULL),
        m_endIdiomatic(NULL),
        m_hyphenLingInfo(NULL),
        m_endHyphenLingInfo(NULL),
        m_concatenated(NULL),
        m_endConcatenated(NULL),
        m_byDefault(NULL),
        m_endByDefault(NULL),
        m_hasLingInfo(false),
        m_hasAccented(false),
        m_hasIdiomatic(false),
        m_hasHyphenLingInfo(false),
        m_hasConcatenated(false),
        m_hasDefaultProperty(false)
{
}

/** copy constructor. Copies the specified DictionaryEntry
 into self. Internal iteration pointers of self are reset
*/
DictionaryEntry::DictionaryEntry(const DictionaryEntry& entry) :
        BinaryEntry(),
        m_key(entry.m_key),
        m_stringStartAddr(entry.m_stringStartAddr),
        m_lingPropertiesStartAddr(entry.m_lingPropertiesStartAddr),
        m_startAddr(NULL),
        m_endAddr(NULL),
        m_lingInfo(NULL),
        m_endLingInfo(NULL),
        m_accented(NULL),
        m_endAccented(NULL),
        m_idiomatic(NULL),
        m_endIdiomatic(NULL),
        m_hyphenLingInfo(NULL),
        m_endHyphenLingInfo(NULL),
        m_concatenated(NULL),
        m_endConcatenated(NULL),
        m_byDefault(NULL),
        m_endByDefault(NULL),
        m_hasLingInfo(false),
        m_hasAccented(false),
        m_hasIdiomatic(false),
        m_hasHyphenLingInfo(false),
        m_hasConcatenated(false),
        m_hasDefaultProperty(false)
{
    if (entry.m_startAddr != NULL) {
        uint64_t entrySize = entry.m_endAddr - entry.m_startAddr;
        m_startAddr = new unsigned char[entrySize];
        memcpy(m_startAddr, entry.m_startAddr, entrySize);
        reset();
    }
}

DictionaryEntry::DictionaryEntry(const LimaString& key,
                                unsigned char *stringStartAddr,
                                unsigned char *lingPropertiesStartAddr,
                                unsigned char *startAddr) :
        BinaryEntry(),
        m_key(key),
        m_stringStartAddr(stringStartAddr),
        m_lingPropertiesStartAddr(lingPropertiesStartAddr),
        m_startAddr(NULL),
        m_endAddr(NULL),
        m_lingInfo(NULL),
        m_endLingInfo(NULL),
        m_accented(NULL),
        m_endAccented(NULL),
        m_idiomatic(NULL),
        m_endIdiomatic(NULL),
        m_hyphenLingInfo(NULL),
        m_endHyphenLingInfo(NULL),
        m_concatenated(NULL),
        m_endConcatenated(NULL),
        m_byDefault(NULL),
        m_endByDefault(NULL),
        m_hasLingInfo(false),
        m_hasAccented(false),
        m_hasIdiomatic(false),
        m_hasHyphenLingInfo(false),
        m_hasConcatenated(false),
        m_hasDefaultProperty(false)
{
    if (startAddr != NULL) {
        unsigned char *ptr = startAddr;
        uint64_t num = getEncodedNumber(ptr);
        unsigned char *endAddr = num + ptr;
        uint64_t entrySize = endAddr - startAddr;
        m_startAddr = new unsigned char[entrySize];
        memcpy(m_startAddr, startAddr, entrySize);
        reset();
    }
}

/** destroys DictionaryEntry
*/
DictionaryEntry::~DictionaryEntry()
{
    delete[] m_startAddr;
}

/** Assignment operator. Copies the specified DictionaryEntry
 into self. Returns a reference to self.
 After assignment, internal iteration pointers of self are reset
*/
DictionaryEntry& DictionaryEntry::operator=(const DictionaryEntry& dictionaryEntry) {
    BinaryEntry::operator=(dictionaryEntry);
    m_key = dictionaryEntry.m_key;
    m_stringStartAddr = dictionaryEntry.m_stringStartAddr;
    m_lingPropertiesStartAddr = dictionaryEntry.m_lingPropertiesStartAddr;
    delete[] m_startAddr;
    m_startAddr = NULL;
    if (dictionaryEntry.m_startAddr != NULL)
{
        uint64_t entrySize = dictionaryEntry.m_endAddr - dictionaryEntry.m_startAddr;
        m_startAddr = new unsigned char[entrySize];
        memcpy(m_startAddr, dictionaryEntry.m_startAddr, entrySize);
        reset();
    }
    return *this;
}

/** Reset internal iteration pointers of self.
 After this call, self is in its creation state
 Returns a reference to self.
*/
DictionaryEntry& DictionaryEntry::reset()
{
    m_endAddr = NULL;
    m_lingInfo = NULL;
    m_endLingInfo = NULL;
    m_accented = NULL;
    m_endAccented = NULL;
    m_idiomatic = NULL;
    m_endIdiomatic = NULL;
    m_hyphenLingInfo = NULL;
    m_endHyphenLingInfo = NULL;
    m_concatenated = NULL;
    m_endConcatenated = NULL;
    m_byDefault = NULL;
    m_endByDefault = NULL;
    m_hasLingInfo = false;
    m_hasAccented = false;
    m_hasIdiomatic = false;
    m_hasHyphenLingInfo = false;
    m_hasConcatenated = false;
    m_hasDefaultProperty = false;
    if (m_startAddr != NULL) {
        unsigned char *ptr = m_startAddr;
        uint64_t num = getEncodedNumber(ptr);
        m_endAddr = num + ptr;                         //   <Len0>
        if (ptr < m_endAddr) {
            uint64_t num = getEncodedNumber(ptr);
            m_endLingInfo = num + ptr;                 //   <Len1>
            m_lingInfo = ptr;
            m_hasLingInfo = (m_endLingInfo > m_lingInfo);
            ptr = m_endLingInfo;
            if (ptr < m_endAddr) {
                uint64_t num = getEncodedNumber(ptr);
                m_endAccented = num + ptr;             //   <Len2>
                m_accented = ptr;
                m_hasAccented = (m_endAccented > m_accented);
                ptr = m_endAccented;
                if (ptr < m_endAddr) {
                    uint64_t num = getEncodedNumber(ptr);
                    m_endIdiomatic = num + ptr;        //   <Len11>
                    m_idiomatic = ptr;
                    m_hasIdiomatic = (m_endIdiomatic > m_idiomatic);
                    ptr = m_endIdiomatic;
                    if (ptr < m_endAddr) {
                        uint64_t num = getEncodedNumber(ptr);
                        m_endHyphenLingInfo = num + ptr;//    <Len1>
                        m_hyphenLingInfo = ptr;
                        m_hasHyphenLingInfo = (m_endHyphenLingInfo > m_hyphenLingInfo);
                        ptr = m_endHyphenLingInfo;
                        if (ptr < m_endAddr) {
                            uint64_t num = getEncodedNumber(ptr);
                            m_endConcatenated = num + ptr;//  <Len3>
                            m_concatenated = ptr;
                            m_hasConcatenated = (m_endConcatenated > m_concatenated);
                            ptr = m_endConcatenated;
                            if (ptr < m_endAddr) {
                                    uint64_t offset = getEncodedNumber(ptr);  //<LingPropertiesOffset>
                                    ptr = m_lingPropertiesStartAddr + offset;
                                    if (offset == 0)         // when no ling properties
                                    {
                                            m_endByDefault = ptr;
                                    }
                                    else
                                    {
                                        uint64_t num = getEncodedNumber(ptr);
                                        m_endByDefault = num + ptr;  //<Len7>
                                    }
                                    m_byDefault = ptr;  
                                    m_hasDefaultProperty = (m_endByDefault > m_byDefault);
                             }
                        }
                    }
                }
            }
        }
    }
    return *this;
}

/** Returns true if self is empty
*/
bool DictionaryEntry::isEmpty() const {
    return (m_startAddr == NULL);
}

/** Gets the next LingInfoEntry if exists
 This call "increments" the internal LingInfoEntry pointer.
 If there is no LingInfo anymore, an empty LingInfoEntry is returned
*/
LingInfoEntry DictionaryEntry::nextLingInfo() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_lingInfo < m_endLingInfo) {
        unsigned char *lingInfo = m_lingInfo;
        nextField(m_lingInfo);
        return LingInfoEntry(m_key, m_stringStartAddr, m_lingPropertiesStartAddr, lingInfo);
    }
    else
        return LingInfoEntry();
}

/** Gets the next accented entry if exists
 This call "increments" the internal accented entry pointer.
 If there is no accented entry anymore, an empty LimaString is returned
*/
Lima::LimaString DictionaryEntry::nextAccented() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_accented < m_endAccented) {
        return getUtf8StringNoMove(m_stringStartAddr + getEncodedNumber(m_accented));
    }
    else
        return Lima::LimaString();
}

/** Gets the next idiomatic entry if exists
 This call "increments" the internal idiomatic entry pointer.
 If there is no idiomatic entry anymore, a null integer is returned
*/
uint64_t DictionaryEntry::nextIdiomatic() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_idiomatic < m_endIdiomatic)
        return getEncodedNumber(m_idiomatic) + 1; // 0 is prohibited
    else
        return 0;
}

/** Gets the next hyphen LingInfoEntry if exists
 This call "increments" the internal hyphen LingInfoEntry pointer.
 If there is no hyphen LingInfo anymore, an empty LingInfoEntry is returned
*/
LingInfoEntry DictionaryEntry::nextHyphenLingInfo() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_hyphenLingInfo < m_endHyphenLingInfo) {
        unsigned char *hyphenLingInfo = m_hyphenLingInfo;
        nextField(m_hyphenLingInfo);
        return LingInfoEntry(m_key, m_stringStartAddr, m_lingPropertiesStartAddr, hyphenLingInfo);
    }
    else
        return LingInfoEntry();
}

/** Gets the next ConcatenatedEntry if exists
 This call "increments" the internal ConcatenatedEntry pointer.
 If there is no Concatenated anymore, an empty ConcatenatedEntry is returned
*/
ConcatenatedEntry DictionaryEntry::nextConcatenated() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_concatenated < m_endConcatenated) {
        unsigned char *concatenated = m_concatenated;
        nextField(m_concatenated);
        return ConcatenatedEntry(concatenated, m_stringStartAddr);
    }
    else
        return ConcatenatedEntry();
}

/** Gets the next LingPropertyEntry if exists
 This call "increments" the internal LingPropertyEntry pointer.
 If there is no LingPropertyEntry anymore, an empty LingPropertyEntry is returned
*/
LingPropertyEntry DictionaryEntry::nextLingProperty()
{
  if (m_startAddr == NULL) throw EmptyEntryException();
  if (m_byDefault < m_endByDefault)
    return LingPropertyEntry(static_cast<LinguisticCode>(getEncodedNumber(m_byDefault)));
  else
    return LingPropertyEntry();
}

} //closing namespace Dictionary
} //closing namespace LinguisticProcessing
} //closing namespace Lima
