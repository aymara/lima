// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *  Utility to sort a list of words, in a more controlled way than
 *  the sort command of Linux
 *  Compare string to sort them in lexically order. Character order is
 *  according to the value of byte.
 *  This sort consider a string as a string of byte. (It does not consider
 *  multibyte caracter). But this is OK because of the smart encoding of UTF8
 *  Double words are supressed.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iosfwd>
#include <stdexcept>
#include <set>

// ICU
#include <unicode/ustring.h>
#include <unicode/unistr.h>
#include <algorithm>

// Util
//#include "common/misc/strwstrtools.h"

using namespace std;

/**
 *  Functor used as Compare parameter of set<..> template
 *  compare string to sort them in lexically
 **/
struct ltstr
{
  bool operator()(const std::string & s1, const std::string & s2) const
  {
    return strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};

int main(int argc, char *argv[])
{
  // path of input file
  std::string inputList;
  // path of output file
  std::string outputList;
  // Associated container to insert strings
  set<std::string, ltstr> list;

  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      std::cout << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cout << "       " << argv[0]
                << " [--output=<filename>]"
                << " [--input=<filename>]"  << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--input=")) != -1 ){
      inputList = arg.substr(pos+8);
    }
    else if ( (pos = arg.find("--output=")) != -1 ){
      outputList = arg.substr(pos+9);
    }
  }

  std::cerr << argv[0] << " ";
  std::cerr << "--input='" << inputList << "' ";
  std::cerr << "--output=" << outputList << "'" << endl;

    std::cerr <<  "Open " << inputList << std::endl;
    std::ifstream is(inputList.c_str(), std::ios::in | std::ios::binary );
    if ( !is.is_open() )
      return 1;

    std::cerr <<  "Read words...." << std::endl;
      std::string newWord;
      char newWordBuff[256];
      for( ; ; ) {
  // read line in array of bytes
        is.getline( newWordBuff, 256, '\n');

  // skip zero space non breaking space (from BOM)
  char *newWord_cstr = newWordBuff;
  if( (newWordBuff[0] == 'ï')
    &&(newWordBuff[1] == '»')
    &&(newWordBuff[2] == '¿') )
  newWord_cstr += 3;

        // build string from array of bytes
  newWord = std::string(newWord_cstr);

        // test if end of file
  if( !newWord.compare(std::string("")))
    break;

        // INsert the new word in the assoiated container (which autmatically
  // put it according to the lexical order)
  pair<set<std::string, ltstr>::iterator,bool> ret = list.insert(newWord);
  if( !ret.second ) {
    std::cerr << "Warning: word " << newWord <<  " already exist!" << std::endl;
  }

    }

    // Open output file
    std::ofstream os(outputList.c_str(), std::ios::out | std::ios::binary );
    for( set<std::string, ltstr>::iterator i = list.begin() ; i != list.end() ; i++ ) {
       os << *i << '
';
    }

  return EXIT_SUCCESS;
}
