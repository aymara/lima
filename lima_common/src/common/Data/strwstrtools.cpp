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
  * @file               strwstrtools.cpp
  * @date               begin Sep, 2003
  * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *                     Copyright (c) 2002,2003-2012 by CEA LIST
  * @version            $Id$
  */

#include "strwstrtools.h"
#include "readwritetools.h"

#include <QtCore/QTextCodec>

#include <algorithm>
#include <string.h>

#define LIDLOGINIT LOGINIT("LanguageIdent::dedal")

/*! ile
    rief   string and wstring utility routines
    \ingroup Misc
*/

/*! xample string.cpp
 
    String functionality example and regression test.
 
    Output:
    \include string.ok
*/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

namespace Lima
{
namespace Common
{
namespace Misc
{

void readStream(std::istream &is,std::string &dest)
{
  while (is.good() && !is.eof())
  {
    const int bufferSize = 10240;
    char buffer[bufferSize];
    is.read(buffer,bufferSize);
    const int size = is.gcount();
    if (size>0)
      dest.insert(dest.end(),buffer,buffer+size);
  }
}

void writeStream(std::ostream &os,const std::string &src)
{
  os.write(src.c_str(),src.length());
  os.flush();
}

//
// Read one line of characters from a file into a string
// additional LINE_FEED characters leads to empty string
//

std::string readLine(std::istream & inputFile)
{
  std::string result;
  
  // read characters, up to '\n', into string
  std::getline( inputFile, result, '\n');
  // discard CR (Carriage return) for Windows
  size_t len = result.length();
  if( (len >= 1) && (result[len-1] == 0x0D) )
    result.erase(len-1);
  // discard BOM
  size_t pos(0);
  if( (result.length() >= 3) && ((unsigned char)(result[0]) == 0xEF)
   && ((unsigned char)(result[1]) == 0xBB)  && ((unsigned char)(result[2]) == 0xBF) ) {
    pos = 3;
    result.erase(0,pos);
  }
  return( result );
}

//
// input/output of strings and wide string in binary files
//

// the strings are stored in binary files as a set of bytes (i.e. char *)
// preceded by the number of bytes occupied
void readString(istream& file, string& s)
{
  uint64_t i(0);
  file.read((char *) &i,sizeof(uint64_t));
  if (i)
  {
    char* cstr=new char[i];
    file.read((char *) cstr,i*sizeof(char));
    s=string(cstr);
    delete []cstr;
  }
}

void writeString(ostream& file, const string& s)
{
  uint64_t i(s.length()+1); // + 1 for ''
  file.write((char *) &i, sizeof(uint64_t));
  file.flush();
  if (i)
  {
    const char* cstr=s.c_str();
    file.write((char *) cstr,i*sizeof(char));
    file.flush();
  }
}

void readStringField(istream& file, string& s)
{
  s.clear();
  uint64_t i=readCodedInt(file);
  if (i)
  {
    char* cstr=new char[i+1]; // + 1 for '' (not in file)
    file.read((char *) cstr,i*sizeof(char));
    cstr[i]='\0'; // add the end of string character
    s=string(cstr);
    delete []cstr;
  }
}

void writeStringField(ostream& file, const string& s)
{
  uint64_t i(s.size()); // not + 1 => does not write '\0'
  writeCodedInt(file,i);
  if (i)
  {
    const char* cstr=s.c_str();
    file.write((char *) cstr,i*sizeof(char));
    file.flush();
  }
}

void readUTF8StringField(std::istream& file,
                         Lima::LimaString& s)
{
  s = LimaString();
  uint64_t i=readCodedInt(file);
  //std::cerr << "readUTF8StringField got size=" << i << std::endl;
  if (i)
  {
    char* cstr = new char[i+1]; // + 1 for '\0' (not in file)
    file.read((char *) cstr,i);
    cstr[i]='\0'; // add the end of string character
    //std::cerr << "readUTF8StringField got cstr=" << cstr << std::endl;
    s = utf8stdstring2limastring(cstr);
    //std::cerr << "readUTF8StringField got s size=" << s.size() << std::endl;
    delete []cstr;
  }
}

void writeUTF8StringField(std::ostream& file,
                          const Lima::LimaString& s)
{
  //std::cerr << "writeUTF8StringField " << s.toUtf8().constData() << " in file at " << file.tellp() << std::endl;
  std::string str = limastring2utf8stdstring(s);
  const char *cstr=str.c_str();
  uint64_t i(str.size()); // not + 1 => does not write '\0'
  //std::cerr << "writeUTF8StringField before writing i=" << i << ", file at " << file.tellp() << std::endl;
  writeCodedInt(file,i);
  //std::cerr << "writeUTF8StringField wrote i=" << i << ", file at " << file.tellp() << std::endl;
  if (i)
  {
    file.write((char *) cstr,i);
    file.flush();
    //std::cerr << "writeUTF8StringField wrote cstr=" << cstr << ", file at " << file.tellp() << std::endl;
  }
}

//
// conversion functions
//

LimaString utf8stdstring2limastring(const std::string& src)
{
  return LimaString::fromUtf8(src.c_str());
}


/*!
    rief        Convert a string in the locale codepage to a UTF8 string
    \ingroup    Misc
*/
std::string localestring2utf8string(const std::string &src)
{
  return QString::fromLocal8Bit(src.c_str()).toUtf8().data();
}

/**
  * @brief        Convert a latin15 string to a LimaString 
  * @ingroup    Misc
  */
LimaString latin15stdstring2limastring(const std::string &src)
{
  return QString::fromLatin1(src.c_str());
}

/*
 * returns the input phrase converted to utf-8, up to size bytes
 * (check that converted phrase does not exceed size)
 */
std::string limastring2utf8stdstring(const Lima::LimaString & phrase, uint32_t size0 )
{
  return phrase.left(size0).toUtf8().data();
}

std::string limastring2utf8stdstring(const Lima::LimaString& src)
{
  return src.toUtf8().constData();
}

//

/**
  * @brief redefine an existing LimaString from UTF-16
  */
LimaString setStringSameEndian (const unsigned short *s,
                                 const uint32_t count)
{
  LimaString result;
  result.resize(count);
  
  if (sizeof(LimaChar)==2)
  {
    result = *(const LimaChar*)s;
  }
  else
    for (uint32_t index=0; index<count; index++)
      result[index] = (wchar_t)s[index];
  return result;
}

/**
  * @brief redefine an existing LimaString from UTF-16 changing the endian mode
  */
LimaString setStringReverseEndian (const unsigned short* s,
                                    const uint32_t count)
{
  LimaString result;
  result.resize(count);
/*
    if (sizeof(LimaChar)==2) {
                            //    0  1  2  3  4  5  6 ...
                            // +  1 -1  1 -1  1 -1  1 ...
                            // =  1  0  3  2  5  4  7 ...
        int offset = 1;
        for (uint64_t index=0; index<count*2; index++) {
            result[index] = ((uint8_t*)s)[index+offset];
            offset = -offset;
        }
    }
    else {*/
  unsigned short chr;
  for (uint32_t index=0; index<count; index++) {
    chr = s[index];
    result[index] = (LimaChar)(((chr&0x00FF)<<8)|((chr&0xFF00)>>8));
  }
//    }
  return result;
}

LimaString transcodeToXmlEntities(const LimaString& str)
{
  LimaString xml;
  LimaString::const_iterator it, it_end;
  it = str.begin(); it_end = str.end();
  for (; it != it_end; it++)
  {
    switch (it->toAscii())
    {
    case '<' : xml += Common::Misc::utf8stdstring2limastring("&lt;"); break;
    case '&' : xml += Common::Misc::utf8stdstring2limastring("&amp;"); break;
    case '>' : xml += Common::Misc::utf8stdstring2limastring("&gt;"); break;
    case '"' : xml += Common::Misc::utf8stdstring2limastring("&quot;"); break;
    case '\'' : xml += Common::Misc::utf8stdstring2limastring("&apos;"); break;
    default:
      {
        xml += LimaChar(*it);
      }
    }
  }
  return xml;
}


} // closing namespace Misc
} // closing namespace Common
} // closing namespace Lima
