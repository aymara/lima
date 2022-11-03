// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// SingleConcatenatedEntry is the way to access dictionary Concatenated word infos datas.
// SingleConcatenatedEntry manages recurent calls to lists of linguistic infos.
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS
//    <Len10><String>
//    <Len1><lingInfo>
//    ...

#include "SingleConcatenatedEntry.h"

#include "common/misc/Exceptions.h"
#include <wchar.h>

using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

// Creates SingleConcatenatedEntry
SingleConcatenatedEntry::SingleConcatenatedEntry() :
BinaryEntry(),
m_stringStartAddr(NULL),
m_startAddr(NULL),
m_dictionaryEntry(NULL) {
}

// copy constructor. Copies the specified SingleConcatenatedEntry 
// into self. Internal iteration pointers of self are reset
SingleConcatenatedEntry::SingleConcatenatedEntry(const SingleConcatenatedEntry& concatenated) :
BinaryEntry(),
m_stringStartAddr(concatenated.m_stringStartAddr),
m_startAddr(concatenated.m_startAddr),
m_dictionaryEntry(NULL) {
    reset();
}

SingleConcatenatedEntry::SingleConcatenatedEntry(unsigned char *startAddr,
                                                unsigned char *stringStartAddr) :
BinaryEntry(),
m_stringStartAddr(stringStartAddr),
m_startAddr(startAddr),
m_dictionaryEntry(NULL) {
    reset();
}

// Destroys SingleConcatenatedEntry
SingleConcatenatedEntry::~SingleConcatenatedEntry() {
}

// Assignment operator. Copies the specified SingleConcatenatedEntry 
// into self. Returns a reference to self. 
// After assignment, internal iteration pointers of self are reset
SingleConcatenatedEntry& SingleConcatenatedEntry::operator=(
                            const SingleConcatenatedEntry& single) {
    m_stringStartAddr = single.m_stringStartAddr;
    m_startAddr = single.m_startAddr;
    reset();
    return *this;
}

// Resets iteration pointers
SingleConcatenatedEntry& SingleConcatenatedEntry::reset() {
    if (m_startAddr != NULL) {
        unsigned char *ptr = m_startAddr;
        getEncodedNumber(ptr);        // Skip <component>
        m_dictionaryEntry = ptr;
    }
    else 
        m_dictionaryEntry = NULL;
    return *this;
}

// Returns true if self is empty
bool SingleConcatenatedEntry::isEmpty() const {
    return (m_startAddr == NULL);
}

// Returns component string 
// If self is empty, EmptyEntryException is raised
Lima::LimaString SingleConcatenatedEntry::component() const {
    if (m_startAddr == NULL) throw EmptyEntryException();
     return getUtf8StringNoMove(m_stringStartAddr + getEncodedNumberNoMove(m_startAddr));                //component 
}

// Returns dictionary entry address attached to the component, if exists
// If there is no dictionary entry, NULL is returned
// (to solve recursivity datas definition)
unsigned char* SingleConcatenatedEntry::dictionaryEntryAddress() {
    if (m_startAddr == NULL) throw EmptyEntryException();
    return m_dictionaryEntry;
}

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima
