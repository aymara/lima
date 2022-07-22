// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SpiritTokenizerParser.hpp"

#include <iostream>
#include <fstream>

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

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Needs exactly one argument" << std::endl;
    return 1;
  }
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef tokenizer<iterator_type> tokenizer;

  // @ERROR DOES NOT WORK ON WINDOWS PORT
 tokenizer tokenizer_parser; 
  std::string str;
  std::ifstream file(argv[1], std::ifstream::binary);
  readStream(file, str);

  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();

  tokenizer_automaton automaton;
  bool r = false;
  // @ERROR DOES NOT WORK ON WINDOWS PORT
  r = phrase_parse(iter, end, tokenizer_parser, skipper, automaton);

  if (r && iter == end)
  {
      std::cout << "Parsing succeeded: "<<automaton.size()<<" states" << std::endl;
  }
  else
  {
      std::string rest(iter, end);
      std::cout << "Parsing failed. Stopped at: \": " << rest << "\"\n";
  }

  return 0;
}
