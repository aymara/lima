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
 *  Utility to sort a list of words, in a more controlled way than
 *  the sort command of Linux
 *  Compare UTF-16 string to sort them in order of 16 bits word.
 *  This sort consider a string as a string of worde.
 *  This does not perfor a lexically
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <cstdlib>
#include <stdexcept>
#include <set>
#include <sstream>

#include <algorithm>

// Util
#include "common/Data/strwstrtools.h"

#include <QtCore/QCoreApplication>

using namespace Lima;
using namespace Lima::Common::Misc;

using namespace std;

/**
 *  Functor used as Compare parameter of set<..> template
 *  compare string to sort them in lexically
 **/
struct ltstr
{
  bool operator()(const LimaString & s1,
    const LimaString & s2) const
  {
    return( s1.compare(s2) < 0 );
  }
};

#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
  //   struct ltstr func;
  // path of input file
  std::string inputList;
  // path of output file
  std::string outputList;
  // Associated container to insert strings
  //set<LimaString, ltstr> list;
  multiset<LimaString, ltstr> list;

  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    std::string::size_type pos = std::string::npos;
    if (arg == "--help")
    {
      std::cout << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cout << "       " << argv[0]
                << " [--output=<filename>]"
                << " [--input=<filename>]"  << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--input=")) != std::string::npos ){
      inputList = arg.substr(pos+8);
    }
    else if ( (pos = arg.find("--output=")) != std::string::npos ){
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
  std::string newWord_UTF8 = std::string(newWord_cstr);

        // test if end of file
  if( !newWord_UTF8.compare(std::string("")))
    break;

  // Convert UTF-8 string in UTF-16
  LimaString newWord_UTF16= Common::Misc::utf8stdstring2limastring(newWord_UTF8);

        // INsert the new word in the assoiated container (which autmatically
  // place it according to the right order)
/*
  pair<set<LimaString, ltstr>::iterator,bool> ret =
     list.insert(newWord_UTF16);
  if( !ret.second ) {
    std::cerr << "Warning: word " << newWord <<  " already exist!" << std::endl;
  }
*/
        list.insert(newWord_UTF16);

    }

    // Open output file
    std::ofstream os(outputList.c_str(), std::ios::out | std::ios::binary );
    std::string outputListComment = outputList + std::string(".comment");
    std::ofstream osc(outputListComment.c_str(), std::ios::out | std::ios::binary );
    for( multiset<LimaString, ltstr>::iterator i = list.begin() ; i != list.end() ; i++ ) {
      // Convert UTF-16 string back in UTF-8
      LimaString line_UTF16 = *i;
      std::string line_UTF8=limastring2utf8stdstring(line_UTF16);

      os << line_UTF8 << '\n';
      std::string::size_type offset = line_UTF8.find('\t');
      std::string lineComment(line_UTF8, 0, offset);
      osc << line_UTF8 << '\t' << lineComment<< '\n';
    }

  return EXIT_SUCCESS;
}
