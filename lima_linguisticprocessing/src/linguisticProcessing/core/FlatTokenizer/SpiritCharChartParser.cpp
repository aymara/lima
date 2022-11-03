// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SpiritCharChartParser.hpp"

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

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  std::vector<std::string> files;

  po::options_description desc("Usage");
  desc.add_options()
    ("help,h", "Display this help message")
    ("input-file", po::value< std::vector<std::string> >(&files), "Set a text file to analyze");

  po::positional_options_description p;
  p.add("input-file", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
  options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef charchart_parser<iterator_type> charchart_parser;

  // @ERROR DOES NOT WORK ON WINDOWS PORT
  charchart_parser parser; // Our grammar
  for (std::vector<std::string>::iterator fileItr=files.begin(); fileItr!=files.end(); fileItr++)
  {
    std::string str;
    std::ifstream file(fileItr->c_str(), std::ifstream::binary);
    readStream(file, str);

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

    charchart charchart;
    bool r = false;
    // @ERROR DOES NOT WORK ON WINDOWS PORT
   r = phrase_parse(iter, end, parser, skipper, charchart);
    if (r && iter == end)
    {
      std::cout << "Parsing succeeded: "<<charchart.classes.size()<<" classes, "<<charchart.chars.size()<<" chars" << std::endl;
    }
    else
    {
        std::string rest(iter, end);
        std::cout << "Parsing failed. Stopped at: \": " << rest << "\"\n";
    }
  }

  return 0;
}
