// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 24-SEP-2002
//
// Byte String
// This class uses was made to build exemples of dictionary written
// in XML

#ifndef S2ByteString_H
#define S2ByteString_H


#include <wchar.h>

namespace Lima {
namespace Common {
namespace Misc {

class S2ByteString {

public:
    S2ByteString();                            
    virtual ~S2ByteString();

    // clears self
    S2ByteString& clear();

    // redefine an existing S2ByteString
    S2ByteString& operator=(const S2ByteString&);

    // return byte string pointer
    const unsigned char* data() const;

    // return length of self
    int length() const;

    // Adds the specified byte at the head/tail of self.
    // Returns a reference to self.
    S2ByteString& append (const unsigned char c);
    S2ByteString& prepend (const unsigned char c);

    // Adds the specified byte string at the head/tail of self.
    // Returns a reference to self.
    S2ByteString& append (
        const unsigned char *c, 
        const uint64_t length);
    S2ByteString& prepend (
        const unsigned char *c, 
        const uint64_t length);

    // Adds the specified string at the head/tail of self
    // Returns a reference to self,
    S2ByteString& append (const wchar_t* s);
    S2ByteString& prepend (const wchar_t* s);

protected:

private:
    unsigned char *_s;                    // pointer to data
    uint64_t _len;

};

} // Misc
} // Common
} // Lima

#endif  
