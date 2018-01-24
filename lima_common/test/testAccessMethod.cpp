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
/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

#include "common/Data/strwstrtools.h" 
#include "common/misc/AbstractAccessByString.h"
#include "common/FsaAccess/FsaAccessSpare16.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::FsaAccess;

// options
typedef struct ParamStruct
{
  std::string key;
  int offset;
  std::string keyFileName;
  std::string accessMethod;
  bool withAssert;
  std::string listOfWords;
  std::string listOfHyperWords;
}
Param;

int readListOfWords(
  const std::string& listOfWordsFilename,
  std::vector<std::basic_string<char> > & listOfWords );
int readListOfHyperWords(
  const std::string& listOfHyperWordsFilename,
  std::vector<std::pair<string,int> > & listOfHyperWords );
void testAccessMethod(const Param& param );


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
  
  setlocale(LC_ALL, "");

  // options reading
  Param param = {
                  std::string(),  // key
                  -1,              // offset
                  std::string(),  // keyFileName
                  std::string("fsa"),  // accessMethod
                  false,          // withAssert
                  std::string(),  // listOfWords
                  std::string()   // listOfHyperwords
                };

  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
      << " --help" << std::endl;
      std::cerr << "       " << argv[0]
      << " [--key=<word>]"
      << " [--offset=<int>]"
      << " [--keyFileName=<filename>]"
      << " [--accessMethod=Fsa|Tree]"
      << " [--language=fre|eng]"
      << " [--withAssert]"
      << " [--listOfWords=<filename>]"
      << " [--listOfHyperWords=<filename>]"
      << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--keyFileName=")) != -1 )
    {
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--key=")) != -1 )
    {
      param.key = arg.substr(pos+6);
    }
    else if ( (pos = arg.find("--offset=")) != -1 )
    {
      param.offset = atoi( (arg.substr(pos+9)).c_str() );
    }
    else if ( arg.compare("--withAssert") == 0 )
    {
      param.withAssert = true;
    }
    else if ( (pos = arg.find("--accessMethod=")) != -1 )
    {
      param.accessMethod = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--listOfHyperWords=")) != -1 )
    {
      param.listOfHyperWords = arg.substr(pos+19);
    }
    else if ( (pos = arg.find("--listOfWords=")) != -1 )
    {
      param.listOfWords = arg.substr(pos+14);
    }
  }

  cerr << "testAccessMethod : ";
  if(param.keyFileName.size())
  {
    cerr << "--keyFileName='" << param.keyFileName << "' ";
  }
  if(param.withAssert)
  {
    cerr << "--withAssert ";
  }
  if(param.accessMethod.size())
  {
    cerr << "--accessMethod='" << param.accessMethod << "' ";
  }
  cerr << "--key="<< param.key << " ";
  cerr << "--offset="<< param.offset << " ";
  if(param.listOfWords.size())
  {
    cerr << "--listOfWords='" << param.listOfWords << "'";
  }
  if(param.listOfHyperWords.size())
  {
    cerr << "--listOfHyperwords= " << param.listOfHyperWords << " ";
  }
  cerr << endl;
  testAccessMethod( param );
  return EXIT_SUCCESS;
}

void testAccessMethod(const Param& param )
{
  string resourcesPath=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
      ?"/usr/share/apps/lima/resources"
      :string(qgetenv("LIMA_RESOURCES").constData());
  string commonConfigFile=string("lima-common.xml");
  string configDir=qEnvironmentVariableIsEmpty("LIMA_CONF")
      ?"/usr/share/config/lima"
      :string(qgetenv("LIMA_CONF").constData());

  AbstractAccessByString* accessMethod(0);
  if (param.accessMethod == "fsa")
  {
    FsaAccessSpare16* fsaAccess=new FsaAccessSpare16();
    fsaAccess->read(param.keyFileName);
    uint64_t size = fsaAccess->getSize();
    cerr <<  "FSA Access : " << size << " keys read from main keyfile" << endl;
    accessMethod=fsaAccess;
  } else {
    cerr << "unknown access method " << param.accessMethod << "!" << endl;
    exit(-1);
  }

  if( !param.key.empty() )
  {
    Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.key));
    uint64_t index = accessMethod->getIndex(limaKey);
    std::cout << param.key << " => " << index << std::endl;
    std::cout << index << " => " << limastring2utf8stdstring(accessMethod->getSpelling(index)) << std::endl;
  }

  if( (!param.key.empty()) && (param.offset != -1 ) )
  {
    Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.key));
    const int offset0 = param.offset;

    std::pair<AccessSubWordIterator,AccessSubWordIterator> ret=
      accessMethod->getSubWords(offset0,limaKey);

    std::cerr << "accessMethod->getSubWors(" << param.key
    << "," << offset0 << ")" << std::endl;
    for( ; ret.first != ret.second ; (ret.first)++)
    {
      int offset = (*(ret.first)).first;
      int64_t elementIndex = (*(ret.first)).second;
      std::cerr << "offset " << offset << " => " << elementIndex << std::endl;
    }
    std::cout << std::endl;
  }

  // test acces simple getEntry() sur liste dans param.listOfWords
  std::cout <<  "read list of words..."<< std::endl;
  typedef std::vector<std::basic_string<char> > ListOfWords;
  if( param.listOfWords.size() > 0 )
  {
    ListOfWords listOfWords;

    readListOfWords(param.listOfWords, listOfWords );

    for( ListOfWords::const_iterator pw = listOfWords.begin() ;
         pw != listOfWords.end() ; pw++ )
    {
      Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(*pw);
      int64_t index = accessMethod->getIndex(word);
      std::cout << "accessMethod->getIndex(" << *pw << ") = "
      << index << std::endl;
    }
  }

  // test acces getSubwords() sur liste dans param.listOfWords
  std::cout <<  "read list of words..."<< std::endl;
  typedef std::vector<std::pair<string,int> > ListOfHyperWords;
  if( param.listOfHyperWords.size() > 0 )
  {
    ListOfHyperWords listOfHyperWords;
    readListOfHyperWords(param.listOfHyperWords, listOfHyperWords );

    for( ListOfHyperWords::const_iterator phw = listOfHyperWords.begin() ;
         phw != listOfHyperWords.end() ; phw++ )
    {
      Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring((*phw).first);
      int offset = (*phw).second;

      std::cerr << "accessMethod->getSubWordEntries(" << (*phw).first
      << "," << offset << ") =" << std::endl;
      std::pair<AccessSubWordIterator,AccessSubWordIterator> ret=
      accessMethod->getSubWords(offset,word);

      for( ; ret.first != ret.second ; (ret.first)++)
      {
        std::pair<int, uint64_t> val = *(ret.first);
        uint64_t index = val.second;
        std::cerr << "offset  " << val.first << " => " << index << std::endl;
      }
      std::cout << std::endl;
    }
  }

}

int readListOfWords(
  const std::string& listOfWordsFilename,
  std::vector<std::basic_string<char> > & listOfWords )
{
  std::ifstream wList(listOfWordsFilename.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() )
  {
    std::cerr <<  "Cannot open list of words " << listOfWordsFilename << std::endl;
    return EXIT_FAILURE;
  }
  std::cerr <<  "Read list of words" << std::endl;
  char strbuff[200];
  for( int counter = 0 ; ; counter++ )
  {
    vector<string> terms;
    wList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( wList.eof() )
    {
      std::cerr <<  "end of list of words." << std::endl;
      break;
    }
    else
    {
      if( !line.empty() )
      {
        std::cerr << "Got line:" << line << std::endl;
        listOfWords.push_back(line);
      }
    }
  }
  return EXIT_SUCCESS;
}


int readListOfHyperWords(
  const std::string& listOfHyperWordsFilename,
  std::vector<std::pair<string,int> > & listOfHyperWords )
{
  std::ifstream hwList(listOfHyperWordsFilename.c_str(), std::ios::in | std::ios::binary );
  if ( !hwList.is_open() )
  {
    std::cerr <<  "Cannot open list of hyperwords " << listOfHyperWordsFilename << std::endl;
    return EXIT_FAILURE;
  }
  std::cerr <<  "Read list of hyperwords" << std::endl;
  char strbuff[200];
#ifdef DEBUG_CD
  FSAALOGINIT;
#endif
  for( int counter = 0 ; ; counter++ )
  {
    hwList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( line.empty() )
    {
      std::cerr <<  "end of list of hyperwords." << std::endl;
      break;
    }
    else
    {
      string::size_type term_pos = line.find(';');
      if( term_pos == string::npos)
      {
#ifdef DEBUG_CD
        LERROR << "error in list of hyperwords line = " << line;
#endif
        break;
      }
      string term(line, 0, term_pos);
      string strOffset(line, term_pos+1);
      int offset = atoi(strOffset.c_str());
      listOfHyperWords.push_back(std::pair<string,int>(term,offset));
#ifdef DEBUG_CD
      LDEBUG <<  "readListOfHyperWords: push(" << term << "," << offset << ")";
#endif

    }
  }
  return EXIT_SUCCESS;
}
