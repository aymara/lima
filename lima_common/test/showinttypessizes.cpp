// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
