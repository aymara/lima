// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// ConcatenatedEntry is the way to access dictionary concatenated words infos datas.
// ConcatenatedEntry manages recurent calls to list of concatenated words.
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

// <concatenated> ::=    <Len3> { <concatenatedEntry> }
// <concatenatedEntry> ::=  <Len9> { <Component> <dictionaryEntry> }
// <dictionaryEntry> ::=        <Len0> <infos>

#include "ConcatenatedEntry.h"

#include "common/misc/Exceptions.h"
#include <wchar.h>

using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

// Creates ConcatenatedEntry 
ConcatenatedEntry::ConcatenatedEntry() :
BinaryEntry(),
m_stringStartAddr(NULL),
m_startAddr(NULL),
m_endAddr(NULL),
m_singleConcatenated(NULL) {
}

// copy constructor. Copies the specified ConcatenatedEntry 
// into self. Internal iteration pointers of self are reset
ConcatenatedEntry::ConcatenatedEntry(const ConcatenatedEntry& concatenated) :
BinaryEntry(),
m_stringStartAddr(concatenated.m_stringStartAddr),
m_startAddr(concatenated.m_startAddr),
m_endAddr(NULL),
m_singleConcatenated(NULL) {
    reset();
}

ConcatenatedEntry::ConcatenatedEntry(unsigned char *startAddr,
                                            unsigned char *stringStartAddr) :
BinaryEntry(),
m_stringStartAddr(stringStartAddr),
m_startAddr(startAddr),
m_endAddr(NULL),
m_singleConcatenated(NULL) {
    reset();
}

// Destroys ConcatenatedEntry
ConcatenatedEntry::~ConcatenatedEntry() {
}

// Assignment operator. Copies the specified ConcatenatedEntry 
// into self. Returns a reference to self. 
// After assignment, internal iteration pointers of self are reset
ConcatenatedEntry& ConcatenatedEntry::operator=(
                            const ConcatenatedEntry& concatenated) {
    BinaryEntry::operator=(concatenated);
    m_stringStartAddr = concatenated.m_stringStartAddr;
    m_startAddr = concatenated.m_startAddr;
    reset();
    return *this;
}

// Reset iteration pointer
ConcatenatedEntry& ConcatenatedEntry::reset() {
    if (m_startAddr != NULL) {
        unsigned char *ptr = m_startAddr;
        uint64_t num = getEncodedNumber(ptr);
        m_endAddr = num + ptr;        //<Len9>
        m_singleConcatenated = ptr;
    }
    else {
        m_endAddr = NULL;
        m_singleConcatenated = NULL;
    }
    return *this;
}

// Returns true if self is empty
bool ConcatenatedEntry::isEmpty() const{
    return (m_startAddr == NULL);
}

// Gets the next SingleConcatenatedEntry if exists
// This call "increments" the internal SingleConcatenatedEntry pointer.
// If there is no SingleConcatenated anymore, an empty SingleConcatenatedEntry 
// is returned
SingleConcatenatedEntry ConcatenatedEntry::nextSingleConcatenated() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    if (m_singleConcatenated < m_endAddr) {
        unsigned char *singleConcatenated = m_singleConcatenated;
        getEncodedNumber(m_singleConcatenated);        // Skip <component>
        nextField(m_singleConcatenated);                // Skip <dictionaryEntry>
        return SingleConcatenatedEntry(singleConcatenated, m_stringStartAddr);
    }
    else 
        return SingleConcatenatedEntry();
}

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima 
