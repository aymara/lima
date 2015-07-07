/*
    Copyright 2015 CEA LIST

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
 * @file       FileUtils.h
 * @author     Gael de Chalendar
 * @date       Tue Jul  7 2015
 * copyright   Copyright (C) 2015 by CEA LIST
 ***********************************************************************/

#include "FileUtils.h"
#include <deque>
#include <iostream>
#include <QDebug>
#include <QFile>

namespace Lima {
namespace Common {
namespace Misc {

uint64_t countLines(std::istream& file)
{ 
  uint64_t result = 0;
  std::streampos initialPosition = file.tellg();
  int c = file.get();
  while (c != -1)
  {
    while (c != -1 && c != '\n') 
    {
      c = file.get();
    }
    result = result + 1;
    c = file.get();
  }
  file.clear();
  file.seekg(initialPosition, std::ios_base::beg);
  return result;
}

uint64_t countLines(QFile& file)
{ 
  uint64_t result = 0;
  qint64 initialPosition = file.pos();
  char c = '\0';
  while (!file.atEnd())
  {
    while (!file.atEnd() && c != '\n') 
    {
      file.getChar(&c);
    }
    result = result + 1;
    file.getChar(&c);
  }
  file.seek(initialPosition);
  return result;
}



} // end namespace
} // end namespace
} // end namespace
