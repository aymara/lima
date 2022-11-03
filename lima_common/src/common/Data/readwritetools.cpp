// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       readwritetools.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Nov  7 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003-2020 by CEA LIST
 *
 ***********************************************************************/

#include "readwritetools.h"
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

  do
  {
    char c = (n & 0x7F) << 1;
    chars.push_back(c);
    n >>= 7;
  } while (n);

  //std::cerr << "writeCodedInt: nb chars to write: " << chars.size() << std::endl;
  for (std::deque<char>::size_type i=chars.size()-1;i>0;i--)
  {
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

} // end namespace
} // end namespace
} // end namespace
