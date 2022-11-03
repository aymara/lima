// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// LingPropertyEntry is the way to access elementary linguistic properties
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

#ifndef LingPropertyEntry_H
#define LingPropertyEntry_H

#include "DictionaryExport.h"
#include "common/Data/LimaString.h"
#include "common/LimaCommon.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {

class LIMA_DICTIONARY_EXPORT LingPropertyEntry {

public:

    // Creates LingPropertyEntry 
    LingPropertyEntry();
    LingPropertyEntry(const LingPropertyEntry&);
    LingPropertyEntry(const LinguisticCode& property);

    // Destroys LingPropertyEntry
    virtual ~LingPropertyEntry();

    // Assignment operator. Copies the specified LingPropertyEntry 
    // into self. Returns a reference to self. 
    LingPropertyEntry& operator=(const LingPropertyEntry&);

    // Returns true if self is empty
    bool isEmpty() const;

    // Returns value of properties
    LinguisticCode data() const;

protected:

private:
    LinguisticCode m_lingProperty;

};

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif  
