// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// LingInfoEntry is the way to access dictionary linguistic infos datas.
// LingInfoEntry manages recurent calls to lists of linguistic properties..
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

#ifndef LingInfoEntry_H
#define LingInfoEntry_H

#include "DictionaryExport.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/Dictionary/BinaryEntry.h"
#include "linguisticProcessing/core/Dictionary/LingPropertyEntry.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

class LIMA_DICTIONARY_EXPORT LingInfoEntry : public BinaryEntry
{

public:
    LingInfoEntry();
    LingInfoEntry(const LingInfoEntry&);
    LingInfoEntry(
        const Lima::LimaString& key, 
        unsigned char *stringStartAddr,
        unsigned char *lingPropertiesStartAddr,
        unsigned char *startAddr);

    // Destroys LingInfoEntry
    virtual ~LingInfoEntry();

    // Assignment operator. Copies the specified LingInfoEntry 
    // into self. Returns a reference to self. 
    // After assignment, internal iteration pointers of self are reset
    LingInfoEntry& operator=(const LingInfoEntry&);

    // Reset iteration pointer
    LingInfoEntry& reset();

    // Returns true if self is empty
    bool isEmpty() const;

    // Returns lemma string 
    // If self is empty, EmptyEntryException is raised
    Lima::LimaString lemma() const;

    // Returns normalized string 
    // If self is empty, EmptyEntryException is raised
    Lima::LimaString normalized() const;

    // Gets the next LingPropertyEntry if exists
    // This call "increments" the internal LingPropertyEntry pointer.
    // If there is no LingPropertyEntry anymore, an empty LingPropertyEntry is returned
    // If self is empty, EmptyEntryException is raised
    LingPropertyEntry nextLingProperty();

protected:

private:
    Lima::LimaString m_key;
    unsigned char *m_stringStartAddr;
    unsigned char *m_lingPropertiesStartAddr;
    unsigned char *m_startAddr;
    unsigned char *m_endAddr;
    unsigned char *m_lingProperty;
    unsigned char *m_endLingProperty;

};

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif  
