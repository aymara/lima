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
// test.cpp : main project file.

#include <iostream>
#include <cstdint>

int main(int , char**)
{
  enum e {};
  std::cerr << "sizeof(bool)=" << sizeof(bool) << std::endl;
  std::cerr << "sizeof(unsigned char)=" << sizeof(unsigned char) << std::endl;
  std::cerr << "sizeof(char)=" << sizeof(char) << std::endl;
  std::cerr << "sizeof(short int)=" << sizeof(short int) << std::endl;
  std::cerr << "sizeof(uint64_t)=" << sizeof(uint64_t) << std::endl;
  std::cerr << "sizeof(int)=" << sizeof(int) << std::endl;
  std::cerr << "sizeof(unsigned long)=" << sizeof(unsigned long) << std::endl;
  std::cerr << "sizeof(long)=" << sizeof(long) << std::endl;
  std::cerr << "sizeof(enum)=" << sizeof(e) << std::endl;
  std::cerr << "sizeof(float)=" << sizeof(float) << std::endl;
  std::cerr << "sizeof(double)=" << sizeof(double) << std::endl;
  std::cerr << "sizeof(long double)=" << sizeof(long double) << std::endl;
}
