// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// ConcatenatedEntry is the way to access dictionary Concatenated words infos datas.
// ConcatenatedEntry manages recurent calls to lists of linguistic infos.
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

#ifndef ConcatenatedEntry_H
#define ConcatenatedEntry_H

#include "DictionaryExport.h"
#include "linguisticProcessing/core/Dictionary/SingleConcatenatedEntry.h"
#include "linguisticProcessing/core/Dictionary/BinaryEntry.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

class LIMA_DICTIONARY_EXPORT ConcatenatedEntry : public BinaryEntry {

public:

    // Creates ConcatenatedEntry as an empty structure.
    ConcatenatedEntry();
    ConcatenatedEntry(const ConcatenatedEntry&);
    ConcatenatedEntry(
        unsigned char *startAddr,
        unsigned char *stringStartAddr);

    // Destroys ConcatenatedEntry
    virtual ~ConcatenatedEntry();

    // Assignment operator. Copies the specified ConcatenatedEntry 
    // into self. Returns a reference to self. 
    // After assignment, internal iteration pointers of self are reset
    ConcatenatedEntry& operator=(const ConcatenatedEntry&);

    // Reset iteration pointer
    ConcatenatedEntry& reset();

    // Returns true if self is empty
    bool isEmpty() const;

    // Gets the next SingleConcatenatedEntry if exists
    // This call "increments" the internal SingleConcatenatedEntry pointer.
    // If there is no SingleConcatenated anymore, an empty SingleConcatenatedEntry is returned
    SingleConcatenatedEntry nextSingleConcatenated();

protected:

private:
    unsigned char *m_stringStartAddr;
    unsigned char *m_startAddr;
    unsigned char *m_endAddr;
    unsigned char *m_singleConcatenated;

};

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima 

#endif  
