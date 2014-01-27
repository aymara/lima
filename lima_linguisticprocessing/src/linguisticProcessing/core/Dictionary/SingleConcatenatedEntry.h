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
// SingleConcatenatedEntry is the way to access dictionary Concatenated word infos datas.
// SingleConcatenatedEntry manages recurent calls to lists of linguistic infos.
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

#ifndef SingleConcatenatedEntry_H
#define SingleConcatenatedEntry_H

#include "DictionaryExport.h"
#include "linguisticProcessing/core/Dictionary/BinaryEntry.h"
#include "common/Data/LimaString.h"


namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {


class LIMA_DICTIONARY_EXPORT SingleConcatenatedEntry : public BinaryEntry {

public:

    // Creates SingleConcatenatedEntry.
    SingleConcatenatedEntry();
    SingleConcatenatedEntry(const SingleConcatenatedEntry&);
    SingleConcatenatedEntry(
        unsigned char *startAddr,
        unsigned char *stringStartAddr);

    // Destroys SingleConcatenatedEntry
    virtual ~SingleConcatenatedEntry();

    // Assignment operator. Copies the specified SingleConcatenatedEntry 
    // into self. Returns a reference to self. 
    // After assignment, internal iteration pointers of self are reset
    SingleConcatenatedEntry& operator=(const SingleConcatenatedEntry&);

    // Resets iteration pointers
    SingleConcatenatedEntry& reset();

    // Returns true if self is empty
    bool isEmpty() const;

    // Returns component string 
    // If self is empty, EmptyEntryException is raised
    Lima::LimaString component() const;

    // Returns dictionary entry address attached to the component, if exists
    // If there is no dictionary entry, NULL is returned
    // (to solve recursivity datas definition)
    unsigned char* dictionaryEntryAddress();

protected:

private:
    unsigned char *m_stringStartAddr;
    unsigned char *m_startAddr;
    unsigned char *m_dictionaryEntry;

};

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif  
