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
