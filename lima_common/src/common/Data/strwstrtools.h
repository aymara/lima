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
/**
  * @file               strwstrtools.h
  * @date               begin Sep, 2003
  * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *                     Copyright (c) 2002,2003-2012 by CEA LIST
  * @version            $Id$
  * @todo               Remove code from GLT OpenGL C++ Toolkit
  */

#ifndef LIMA_COMMONS_MISC_STRWSTRTOOLS_H
#define LIMA_COMMONS_MISC_STRWSTRTOOLS_H

#include "LimaString.h"

#include <string>
#include <vector>
#include <iosfwd>
#include <ostream>

// // From ICU
#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
(((char32_t)(lead)<<10UL)+(char32_t)(trail)-U16_SURROGATE_OFFSET)

#define U16_LEAD(supplementary) (char32_t)(((supplementary)>>10)+0xd7c0)
#define U16_TRAIL(supplementary) (char32_t)(((supplementary)&0x3ff)|0xdc00)

#define U_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SINGLE(c) !U_IS_SURROGATE(c)
#define U16_IS_SURROGATE(c) U_IS_SURROGATE(c)
#define U16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)

#ifdef WIN32
#include <stdint.h>
#endif

#define U16_NEXT(s, i, length, c) { \
(c)=(s)[(i)++].unicode(); \
if(U16_IS_LEAD(c)) { \
  uint16_t __c2; \
  if((i)<(length) && U16_IS_TRAIL(__c2=(s)[(i)].unicode())) { \
    ++(i); \
    (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
    } \
    } \
    }

#define U16_PREV(s, start, i, c) { \
(c)=(s)[--(i)].unicode(); \
if(U16_IS_TRAIL(c)) { \
  uint16_t __c2; \
  if((i)>(start) && U16_IS_LEAD(__c2=(s)[(i)-1].unicode())) { \
    --(i); \
    (c)=U16_GET_SUPPLEMENTARY(__c2, (c)); \
  } \
} \
}

#define U_MASK(x) ((uint32_t)1<<(x))
    
    
namespace Lima {
namespace Common {
namespace Misc {

//
// string <-> stream
//

/*!
    rief        Read an input stream into a string, until EOF
    \ingroup    Misc
*/
LIMA_DATA_EXPORT void readStream (std::istream &is,std::string &dest);

/*!
    rief        Write a string to an output stream
    \ingroup    Misc
*/
LIMA_DATA_EXPORT void writeStream(std::ostream &os,const std::string &src);

/*!
    rief        Read an input stream into a wide string, until EOF
    \ingroup    Misc
*/
//
// reading and writing string and wstring in binary format
//
/*!
    rief        read a string in an input file stream
    \ingroup    Misc
*/
LIMA_DATA_EXPORT void readString(std::istream& file, std::string& s);
/*!
    rief        write a string on an output file stream
    \ingroup    Misc
*/
LIMA_DATA_EXPORT void writeString(std::ostream& file, const std::string& s);

//@{ binary read/write function for simple string
// (use variable-byte coding for length of strings (less space used)
LIMA_DATA_EXPORT void readStringField(std::istream& file, std::string& s);
LIMA_DATA_EXPORT void writeStringField(std::ostream& file, const std::string& s);
//@}

/** 
 * Binary read an utf8 string from a file and store it in a @ref LimaString
 *
 * @param file the file from which to read
 * @param s the LimaString read
 */
LIMA_DATA_EXPORT void readUTF8StringField(std::istream& file,
                    Lima::LimaString& s);
/**
 * Binary write a a @ref LimaString in a file as an utf8 string
 *
 * @param file the file to write in
 * @param s the wstring to write
 */
LIMA_DATA_EXPORT void writeUTF8StringField(std::ostream& file,
                      const Lima::LimaString& s);


//
// string <-> limastring
//

LIMA_DATA_EXPORT LimaString utf8stdstring2limastring(const std::string& src);

/**
  * @brief        Convert a latin15 string to a LimaString
  * @ingroup    Misc
  */
LIMA_DATA_EXPORT LimaString latin15stdstring2limastring(const std::string &src);

/** LIMA own code */

/*!
    rief        Convert a string in the locale codepage to a UTF8 string
    \ingroup    Misc
*/
LIMA_DATA_EXPORT std::string localestring2utf8string(const std::string &src);

/**
  * @brief        Convert a wide string to a string , in dest up to size bytes
  * @ingroup      Misc
  */
LIMA_DATA_EXPORT std::string limastring2utf8stdstring(const Lima::LimaString & phrase, uint32_t size );

/**
  * @brief        Convert a Lima string to a string
  * @ingroup      Misc
*/
LIMA_DATA_EXPORT std::string limastring2utf8stdstring(const Lima::LimaString& src);

template < typename C > 
void chomp(std::basic_string< C >& str)
{

    if ( ( str.begin() != str.end() )
        && ( *(--str.end()) == C('\n') ) ) str.erase(--str.end());
}

template < typename C >
void chop(std::basic_string< C >& str)
{

    if ( str.begin() != str.end() ) str.erase(--str.end());
}


// read 1 line of text file in string
LIMA_DATA_EXPORT std::string readLine(std::istream & inputFile);

// convert any String to displayable string (for trace only)
template <typename charT>
std::string convertString( const std::basic_string<charT> &input ) {
  static const char hexSymbol[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  std::string result;
  for(typename std::basic_string<charT>::const_iterator i = input.begin() ; i != input.end() ; i++ ) {
    result.append(" x");
    uint64_t val = (*i).unicode();
    if ( sizeof(charT) == 4 ) {
      result.append( 1, hexSymbol[ (val>>28)&0xF ] );
      result.append( 1, hexSymbol[ (val>>24)&0xF ] );
      result.append( 1, hexSymbol[ (val>>20)&0xF ] );
      result.append( 1, hexSymbol[ (val>>16)&0xF ] );
    }
    if ( sizeof(charT) >= 2 ) {
      result.append( 1, hexSymbol[ (val>>12)&0xF ] );
      result.append( 1, hexSymbol[ (val>>8)&0xF ] );
    }
    result.append( 1, hexSymbol[ (val>>4)&0xF ] );
    result.append( 1, hexSymbol[ val&0x0F ] );
  }
  if ( sizeof(charT) == 1 ) {
    const void *ptr = static_cast<const void*>(&input);
    const std::string* strPtr = static_cast<const std::string*>(ptr);
    const std::string text = *strPtr;
    result += std::string("(") + text + std::string(")");
  }
  return result;
}

LIMA_DATA_EXPORT LimaString setStringSameEndian (const short unsigned int* s, const uint32_t count);
LIMA_DATA_EXPORT LimaString setStringReverseEndian (const short unsigned int* s, const uint32_t count);

LIMA_DATA_EXPORT LimaString transcodeToXmlEntities(const LimaString& str);



} // closing namespace Misc
} // closing namespace Common
} // closing namespace Lima

#endif // LIMA_COMMONS_MISC_STRWSTRTOOLS_H
