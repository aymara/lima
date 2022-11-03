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

#include "S2ByteString.h"

#include <fstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace Lima {
namespace Common {
namespace Misc {

S2ByteString::S2ByteString() :
_s(NULL),
_len(0) {
}

S2ByteString::~S2ByteString() {
    delete[] _s;
}

// clears self
S2ByteString& S2ByteString::clear() {
    delete[] _s;
    _s=NULL;
    _len=0;
    return *this;
}

// redefine an existing S2ByteString
S2ByteString& S2ByteString::operator=(const S2ByteString& s) {
    delete[] _s;
    _s = new unsigned char[s._len];
    _len = s._len;
    memcpy (_s, s._s, s._len);
    return *this;
}

// return byte string pointer
const unsigned char* S2ByteString::data() const {
    return _s;
}

// return length of self
int S2ByteString::length() const {
    return _len;
}

// Adds the specified byte at the head/tail of self.
// Returns a reference to self.
S2ByteString& S2ByteString::append (const unsigned char c) {
    unsigned char *old = _s;
    _s = new unsigned char[_len+1];
    memcpy (_s, old, _len);
    _s[_len] = c;
    _len = _len+1;
    delete[] old;
    return *this;
}

S2ByteString& S2ByteString::prepend (const unsigned char c) {
    unsigned char *old = _s;
    _s = new unsigned char[_len+1];
    _s[0] = c;
    memcpy (_s+1, old, _len);
    _len = _len+1;
    delete[] old;
    return *this;
}

// Adds the specified byte string at the head/tail of self.
// Returns a reference to self.
S2ByteString& S2ByteString::append (const unsigned char *c, 
                                    const uint64_t length) {
    unsigned char *old = _s;
    _s = new unsigned char[_len+length];
    memcpy (_s, old, _len);
    memcpy (_s+_len, c, length);
    _len = _len+length;
    delete[] old;
    return *this;
}

S2ByteString& S2ByteString::prepend (const unsigned char *c, 
                                     const uint64_t length) {
    unsigned char *old = _s;
    _s = new unsigned char[_len+length];
    memcpy (_s, c, length);
    memcpy (_s+length, old, _len);
    _len = _len+length;
    delete[] old;
    return *this;
}

// Adds the specified string at the head/tail of self
// It is assumed that string is in normal endian mode
// (big or little depending of platform). String in file
// is in big endian mode
// Returns a reference to self,
S2ByteString& S2ByteString::append (const wchar_t* s) {
    unsigned char *old = _s;
    uint64_t length = wcslen(s)*2;
    _s = new unsigned char[_len+length];
    memcpy (_s, old, _len);
    // if big endian platform and sizeof(wchar_t)==2, memcpy works
    // but it is a rare case, so memcpy is not used
    wchar_t chr;
    unsigned char *s2 = _s + _len;
    for (uint64_t i=0; i<wcslen(s); i++) {
        chr = s[i];
        // highter significant byte first
        // in case of sizeof(wchar_t)==4, 2 hightest bytes are discarded
        *s2++ = (unsigned char)((chr&0xFF00)>>8);
        *s2++ = (unsigned char) (chr&0x00FF);
    }
    _len = _len+length;
    delete[] old;
    return *this;
}

S2ByteString& S2ByteString::prepend (const wchar_t* s) {
    unsigned char *old = _s;
    uint64_t length = wcslen(s)*2;
    _s = new unsigned char[_len+length];
    wchar_t chr;
    unsigned char *s2 = _s;
    for (uint64_t i=0; i<wcslen(s); i++) {
        chr = s[i];
        // highter significant byte first
        // in case of sizeof(wchar_t)==4, 2 hightest bytes are discarded
        *s2++ = (unsigned char)((chr&0xFF00)>>8);
        *s2++ = (unsigned char) (chr&0x00FF);
    }
    memcpy (_s+length, old, _len);
    _len = _len+length;
    delete[] old;
    return *this;
}

} // Misc
} // Common
} // Lima
