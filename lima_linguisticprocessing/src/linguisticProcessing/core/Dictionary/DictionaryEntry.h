// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** NAUTITIA
//
// jys 1-OCT-2002
//
// DictionaryEntry is the way to access dictionary datas
// DictionaryEntry manages recurent calls to lists of properties.
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS
*/

#ifndef DictionaryEntry_H
#define DictionaryEntry_H

#include "DictionaryExport.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/Dictionary/LingInfoEntry.h"
#include "linguisticProcessing/core/Dictionary/ConcatenatedEntry.h"
#include "linguisticProcessing/core/Dictionary/LingPropertyEntry.h"
#include "linguisticProcessing/core/Dictionary/BinaryEntry.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

class LIMA_DICTIONARY_EXPORT DictionaryEntry : public BinaryEntry {

public:

    // creates DictionaryEntry
    DictionaryEntry();
    DictionaryEntry(const DictionaryEntry&);
    DictionaryEntry(
        const Lima::LimaString& key,
        unsigned char *stringStartAddr,
        unsigned char *lingPropertiesStartAddr,
        unsigned char *startAddr);

    // destroys DictionaryEntry
    virtual ~DictionaryEntry();

    // Assignment operator. Copies the specified DictionaryEntry
    // into self. Returns a reference to self.
    // After assignment, internal iteration pointers of self are reset
    DictionaryEntry& operator=(const DictionaryEntry&);

    // Reset internal iteration pointers of self.
    // After this call, self is in its creation state
    // Returns a reference to self.
    DictionaryEntry& reset();

    // Returns true if self is empty
    bool isEmpty() const;

    // Gets the next LingInfoEntry if exists
    // This call "increments" the internal LingInfoEntry pointer.
    // If there is no LingInfo anymore, an empty LingInfoEntry is returned
    LingInfoEntry nextLingInfo();

    // Gets the next accented entry if exists
    // This call "increments" the internal accented entry pointer.
    // If there is no accented entry anymore, an empty Lima::LimaString is returned
    Lima::LimaString nextAccented();

    // Gets the next idiomatic entry if exists
    // This call "increments" the internal idiomatic entry pointer.
    // If there is no idiomatic entry anymore, a null integer is returned
    uint64_t nextIdiomatic();

    // Gets the next hyphen LingInfoEntry if exists
    // This call "increments" the internal hyphen LingInfoEntry pointer.
    // If there is no hyphen LingInfo anymore, an empty LingInfoEntry is returned
    LingInfoEntry nextHyphenLingInfo();

    // Gets the next ConcatenatedEntry if exists
    // This call "increments" the internal ConcatenatedEntry pointer.
    // If there is no Concatenated anymore, an empty ConcatenatedEntry is returned
    ConcatenatedEntry nextConcatenated();

    // Gets the next LingPropertyEntry if exists
    // This call "increments" the internal LingPropertyEntry pointer.
    // If there is no LingPropertyEntry anymore, an empty LingPropertyEntry is returned
    LingPropertyEntry nextLingProperty();

    // Returns true if DictionaryEntry has at least one specified component
    inline bool hasLingInfo() const;
    inline bool hasAccented() const;
    inline bool hasIdiomatic() const;
    inline bool hasHyphenLingInfo() const;
    inline bool hasConcatenated() const;
    inline bool hasDefaultProperty() const;
    // at least LingIfo, HyphenLingInfo, Concatenated
    inline bool hasAnyLingInfoEntry() const;

    inline unsigned char* lingPropertiesStartAddr() { return m_lingPropertiesStartAddr; }
    inline const unsigned char* lingPropertiesStartAddr() const { return m_lingPropertiesStartAddr; }
    inline unsigned char* stringStartAddr() { return m_stringStartAddr; }
    inline const unsigned char* stringStartAddr() const { return m_stringStartAddr; }
protected:

private:
    Lima::LimaString m_key;
    unsigned char *m_stringStartAddr;
    unsigned char *m_lingPropertiesStartAddr;
    unsigned char *m_startAddr;
    unsigned char *m_endAddr;
    unsigned char *m_lingInfo;
    unsigned char *m_endLingInfo;
    unsigned char *m_accented;
    unsigned char *m_endAccented;
    unsigned char *m_idiomatic;
    unsigned char *m_endIdiomatic;
    unsigned char *m_hyphenLingInfo;
    unsigned char *m_endHyphenLingInfo;
    unsigned char *m_concatenated;
    unsigned char *m_endConcatenated;
    unsigned char *m_byDefault;
    unsigned char *m_endByDefault;
    bool m_hasLingInfo;
    bool m_hasAccented;
    bool m_hasIdiomatic;
    bool m_hasHyphenLingInfo;
    bool m_hasConcatenated;
    bool m_hasDefaultProperty;

};

// Returns true if DictionaryEntry has at least one specified component
inline bool DictionaryEntry::hasLingInfo() const {return m_hasLingInfo;}
inline bool DictionaryEntry::hasAccented() const {return m_hasAccented;}
inline bool DictionaryEntry::hasIdiomatic() const {return m_hasIdiomatic;}
inline bool DictionaryEntry::hasHyphenLingInfo() const {return m_hasHyphenLingInfo;}
inline bool DictionaryEntry::hasConcatenated() const {return m_hasConcatenated;}
inline bool DictionaryEntry::hasDefaultProperty() const {return m_hasDefaultProperty;}
inline bool DictionaryEntry::hasAnyLingInfoEntry() const {
    return (m_hasLingInfo || m_hasHyphenLingInfo || m_hasConcatenated);}

} //closing namespace Dictionary
} //closing namespace LinguisticProcessing
} //closing namespace Lima


#endif
