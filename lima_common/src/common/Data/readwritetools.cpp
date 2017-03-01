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
/************************************************************************
 *
 * @file       readwritetools.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Nov  7 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "readwritetools.h"
#include "strwstrtools.h"
#include <deque>
#include <QDebug>
#include <QFile>

namespace Lima {
namespace Common {
namespace Misc {


uint64_t readCodedInt(std::istream& file) {
  char c = 0x1;
  uint64_t val = 0;

  while((c & 0x1) && file.good()) {
    file.read((char*)&c, sizeof(char));
    val = (val << 7) + ((c >> 1) & 0x7F);
  }
  return(val);
}

void writeCodedInt(std::ostream& file, const uint64_t number)
{
  //std::cerr << "writeCodedInt: input, file at: " << file.tellp() << std::endl;
  std::deque<char> chars;
  uint64_t n(number);
  
  do {
    char c = (n & 0x7F) << 1;
    chars.push_back(c);
    n >>= 7;
  } while (n);

  //std::cerr << "writeCodedInt: nb chars to write: " << chars.size() << std::endl;
  for (std::deque<char>::size_type i=chars.size()-1;i>0;i--) {
    chars[i] |= 0x1;
    //std::cerr << "writeCodedInt: in for writing nb chars: " << sizeof(char) << std::endl;
    file.put(chars[i]);
    file.flush();
  }
  //std::cerr << "writeCodedInt: writing nb chars: " << sizeof(char) << std::endl;
  file.put(chars[0]);
  file.flush();

  //std::cerr << "writeCodedInt: output, file at: " << file.tellp() << std::endl;
}

//**********************************************************************
uint8_t readOneByteInt(std::istream& file) {
  uint8_t c;
  file.read((char*)&c, 1);
  file.peek(); // Try to read next byte to force update end-of-file flag on windows
  return c;
}

void writeOneByteInt(std::ostream& file, const uint8_t number) {
  file.put((char)number);
  file.flush();
}


//**********************************************************************
double readDouble(std::istream& file)
{
  LimaString str;
  readUTF8StringField(file, str);
  return str.toDouble();
}

void writeDouble(std::ostream& file, const double number)
{
  LimaString str;
  QTextStream qts(&str);
  qts << number;
  writeUTF8StringField(file, str);
}

} // end namespace
} // end namespace
} // end namespace
