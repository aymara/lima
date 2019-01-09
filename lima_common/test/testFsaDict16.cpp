/*
    Copyright 2002-2019 CEA LIST

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
                          testFsaDict16.cpp  -  description
                             -------------------
    begin                : lun jun 2 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
//  ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                         *
 ***************************************************************************/
#include "common/LimaCommon.h"

#include "common/time/traceUtils.h"

// string and file handling Utilities
#include "common/Data/strwstrtools.h"

// dictionaries
#include "common/FsaAccess/FsaAccessBuilder16.h"
#include "common/FsaAccess/FsaAccessBuilderRandom16.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/misc/AbstractAccessByString.h"

#include <QtCore/QCoreApplication>

// for set locale
#include <locale.h>
// for system()
#include <sys/timeb.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <list>
#include <string>

// For ::stat() function
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif
using namespace std;
using namespace Lima;
using namespace Lima::Common;

int logFileSize( const std::string& filename ) {
  struct stat sts;
  if( stat( filename.c_str(), &sts) != 0)
    std::cerr << "logFileSize: error getting info for file " << filename << std::endl;
  std::cerr << "taille fichier: " << filename << "= " << sts.st_size << std::endl;
  return sts.st_size;
}

int logMemsize() {
  int vmSize = 0;
#ifdef WIN32
  return vmSize;
#else
  pid_t pid = getpid();
  ostringstream ostr;
  ostr << "/proc/" << pid << "/status";
  ifstream statusFile(ostr.str().c_str(), std::ifstream::binary);
  char strbuff[200];
  for( ; ; ) {
    string status;
    statusFile.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if(line.empty() )
      break;
    string::size_type composed1_pos = line.find("VmSize:");
    if( composed1_pos != string::npos ) {
      string vmSizeStr(line, composed1_pos+7);
      vmSize = atoi(vmSizeStr.c_str());
    }
  }
#endif
  return vmSize;
}

int getProcStat( const std::string& toLog ) {
#ifdef WIN32
  LIMA_UNUSED(toLog);
  return 0;
#else
  std::string statusFile;

  ostringstream os;
  os << "/proc/" << getpid() << "/status";
  statusFile=os.str();

  ifstream statusIn(statusFile.c_str(),ios::in | std::ifstream::binary);
  string line;
  int val = 0;
  while (!statusIn.eof())
  {
    getline(statusIn,line);
//    std::cerr << "line = " << line << std::endl;
    size_t index=line.find(toLog);
    if( index != std::string::npos ) {
//      std::cerr << "index = " << index << std::endl;
      string valstr=line.substr(index+toLog.size()+1);
//      std::cerr << "valstr = " << valstr << std::endl;
      val = atoi(valstr.c_str());
      std::cerr << toLog << "=" << val <<std::endl;
    }
  }
  return val;
#endif
}

enum LimaCharSize {
  one_byte = sizeof(char),
  two_bytes = sizeof(char16_t),
  four_bytes = sizeof(char32_t) };



// options
typedef struct ParamStruct {
  std::string listOfWords;
  std::string outputDico;
  std::string inputDico;
  bool subWord;
  std::string listOfHyperwords;
  bool superWord;
  bool printGraph;
  bool spareMem;
  LimaCharSize charSize;
  bool withoutTemplate;
  bool trieDirectionForward;
  bool withDebug;
  bool runPerfo;
  bool runIndex;
  bool addWord;
  bool runSpelling;
  int64_t termId;
  bool composed;
  bool withAssert;
  std::string inputDicoComp;
  std::string configDir;
} Param;

int readListOfWords(
  const std::string& listOfWordsFilename,
  std::vector<Lima::LimaString > & listOfWords )
  {
  std::ifstream wList(listOfWordsFilename.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() ) {
    std::cerr <<  "Cannot open list of words " << listOfWordsFilename << std::endl;
    return EXIT_FAILURE;
  }
  std::cerr <<  "Read list of words" << std::endl;
  char strbuff[200];

  for( int counter = 0 ; ; counter++ ) {
    // lecture d'une ligne du fichier
    wList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( line.size() == 0 ) {
      std::cerr <<  "end of list of words. counter=" << counter << std::endl;
      break;
    }
    else {
      // extraction cha�e
      Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(line);
      listOfWords.push_back(word);
    }
  }
  return EXIT_SUCCESS;
}


template <typename dictType>
class DictTester {
  public:
    DictTester(Param param, dictType &dico) : m_param(param), m_dico(dico) {
    }
    void exec( void );
    void testSub(std::vector<Lima::LimaString >& hyperwords,
                 std::vector<int>& offsets,
                 std::vector<std::vector<Lima::LimaString > > &subwords, bool withAssert );
    void testSuper(typename std::vector<Lima::LimaString >::const_iterator begin,
                   typename std::vector<Lima::LimaString >::const_iterator end );
    void testIndex( typename std::vector<Lima::LimaString >::const_iterator begin,
                    typename std::vector<Lima::LimaString >::const_iterator end,
                    const std::vector<int>& indexes );
    void testSpelling( typename std::vector<Lima::LimaString >::const_iterator begin,
                       typename std::vector<Lima::LimaString >::const_iterator end,
                       const std::vector<int>& indexes );
    void addListOfWords();
    void addListOfUnorderedWords();
    void write( void );
  private:
    Param m_param;
    dictType &m_dico;
};


template <typename dictType>
void DictTester<dictType>::addListOfWords() {

  if( !m_param.listOfWords.size() )
    return;

  std::ifstream wList(m_param.listOfWords.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() ) {
    std::cerr <<  "Cannot open list of words " << m_param.listOfWords << std::endl;
    return;
  }
  std::cerr <<  "Read list of words" << std::endl;
  char strbuff[200];

  for( int counter = 0 ; ; counter++ ) {
    if( (counter%10000) == 0 ) {
      std::cerr << "\raddListOfWords counter = " << counter
                << " VmSize: " << logMemsize() << std::flush;
//      std::cerr << "addListOfWords counter = " << counter << std::endl;
    }
    // lecture d'une ligne du fichier
    wList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( wList.eof() )
    {
      std::cerr <<  "end of list of words. counter=" << counter << std::endl
                << std::flush;
      break;
    }
    else if (!line.empty())
    {
//       std::cerr <<  "addListOfWords: (" << line << ")" << std::endl;
      Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(line);
//       std::cerr <<  "addListOfWords: addWord(" << word << ")" << std::endl;
      m_dico.addWord( word );
    }
  }
  std::cerr << std::endl << std::endl;
  m_dico.pack();
}

template <typename dictType>
void DictTester<dictType>::addListOfUnorderedWords() {

  if( m_param.printGraph ) {
   std::cerr <<  "Print graph...." << std::endl;
   m_dico.printGraph(std::cerr);
  }

  if( !m_param.listOfWords.compare(std::string("")) )
    return;

  std::vector<Lima::LimaString > listOfWords;
  readListOfWords(m_param.listOfWords, listOfWords);

  int counter(0);
  for( std::vector<Lima::LimaString >::iterator itWord = listOfWords.begin() ;
     itWord != listOfWords.end() ; itWord++, counter++ ) {
//    if( (counter%10000) == 0 ) {
      std::cerr << "addListOfWords(" << *itWord << "),  counter = " << counter << std::endl;
//    }
    m_dico.addRandomWord( *itWord );
  }

  if( m_param.printGraph ) {
   std::cerr <<  "Print graph...." << std::endl;
   m_dico.printGraph(std::cerr);
  }
//  m_dico.pack();
}

template <typename dictType>
void DictTester<dictType>::testIndex(
  typename std::vector<Lima::LimaString >::const_iterator begin,
  typename std::vector<Lima::LimaString >::const_iterator end,
  const std::vector<int>& indexes ) {
  std::cerr << "testIndex: getSize() = " << m_dico.getSize() << std::endl;

  std::vector<int>::const_iterator indexItr = indexes.begin();
  int index0 = 0;

  for( typename std::vector<Lima::LimaString >::const_iterator lemma = begin ;
        lemma != end ; lemma++ ) {
    // recup�ation de l'index �partir de la cha�e de caract�es
    int index = m_dico.getIndex(*lemma);
    // traces
    if( index%10000 == 0 ) {
      std::cout << "\rtestIndex index = " << index
                << " VmSize: " << logMemsize() << std::flush;
    }
    if( m_param.withDebug ) {
      Lima::LimaString newWord = *lemma;
      std::cerr <<  "testIndex: getIndex("
                << Lima::Common::Misc::limastring2utf8stdstring(newWord)
                <<  ")=" << index << std::endl;
    }
    else {
      if( index%10000 == 1 ) {
        Lima::LimaString newWord = *lemma;
        std::cerr <<  "testIndex: getIndex(" << Lima::Common::Misc::limastring2utf8stdstring(newWord)
                  <<  ")=" << index << std::endl;
      }
    }
    // result verification
    if( m_param.withAssert ) {
      if( indexItr != indexes.end() ) {
//        std::cerr << "check " << index << "!=" << *indexItr << std::endl;
        assert( index == *indexItr);
        indexItr++;
      }
      else {
//        std::cerr << "check " << index << "!=" << index0+1 << std::endl;
        Q_ASSERT( index == index0+1 );
        index0 = index;
      }
    }
  }
  std::cout << std::endl;
  // test sur chaine n'existant pas
  for( typename std::vector<Lima::LimaString >::const_iterator lemma = begin ;
        lemma != end ; lemma++ ) {
    int index = m_dico.getIndex(*lemma);
    Lima::LimaString invertedLemma;
    for( int i = (*lemma).size()-1; i >= 0 ; i-- ) {
      invertedLemma.push_back((*lemma)[i]);
    }
    int invertedIndex = m_dico.getIndex(invertedLemma);
    // traces
    if( index%10000 == 0 ) {
      std::cout << "testIndex inverted ("
           << Lima::Common::Misc::limastring2utf8stdstring(invertedLemma)
           << ")  index = " << invertedIndex << " VmSize: " << logMemsize()
           << std::flush;
    }
  }
}

template <typename dictType>
    void  DictTester<dictType>::testSpelling( typename std::vector<Lima::LimaString >::const_iterator begin,
                      typename std::vector<Lima::LimaString >::const_iterator end,
                      const std::vector<int>& indexes )
//void DictTester<dictType>::testSpelling( int *indexVal, int nbIndex )
{
  LIMA_UNUSED(end);
  typename std::vector<Lima::LimaString >::const_iterator lemma = begin;

  // if size of indexes = 1, we just display the string return by getSpelling()
  std::cerr <<  "testSpelling: getSpelling: indexes.size()=" << indexes.size() << std::endl;
  if( indexes.size() == 1 ) {
    Lima::LimaString spelling;
    spelling = m_dico.getSpelling(indexes[0]);
    std::cerr <<  "testSpelling: getSpelling(" << indexes[0]
                << ")=" << Lima::Common::Misc::limastring2utf8stdstring(spelling) << std::endl;
  }
  // for each id, compare result of getSpelling with element in vector of string [begin,end]
  for( uint32_t i = 0 ; i < indexes.size() ; i++ ) {
    Lima::LimaString spelling;
    try{
      spelling = m_dico.getSpelling(indexes[i]);
      if( i%10000 == 1 ) {
        std::cerr <<  "testSpelling: getSpelling(" << indexes[i]
                << ")=" << Lima::Common::Misc::limastring2utf8stdstring(spelling) << std::endl;
      }
      if( m_param.withAssert ) {
        assert( spelling == (*lemma) );
      }
    }
    catch(std::logic_error e ) {
      std::cerr << "testSpelling exception: " << e.what() << std::endl;
    }
    lemma++;
  }
}

template <typename dictType>
void DictTester<dictType>::testSuper(
       typename std::vector<Lima::LimaString >::const_iterator begin,
       typename std::vector<Lima::LimaString >::const_iterator end ) {

  for( typename std::vector<Lima::LimaString >::const_iterator it = begin ;
         it != end ; it++ ) {
    try{
      Lima::LimaString prefix = *it;
      std::pair<AccessSuperWordIterator, AccessSuperWordIterator> entries =
        m_dico.getSuperWords(prefix);
      std::cerr <<  "testSuper: getSuperWords("
                << Lima::Common::Misc::limastring2utf8stdstring(prefix)
                << ")" << std::endl;
      for( ; entries.first != entries.second ; entries.first++ ) {
        Lima::LimaString superWord = *(entries.first);
        std::cout << Lima::Common::Misc::limastring2utf8stdstring(superWord)
                  << std::endl;
      }
    }
    catch(std::logic_error e ) {
      std::cerr << "testSuper: getSuperWords exception: " << e.what() << std::endl;
    }
  }
}

template <typename dictType>
 void DictTester<dictType>::testSub(
  std::vector<Lima::LimaString > & hyperwords,
  std::vector<int> & offsets,
  std::vector<std::vector<Lima::LimaString > >& subwords, bool withAssert ) {

  typename std::vector<Lima::LimaString >::iterator wordIt;
  std::vector<int>::iterator offsetIt = offsets.begin();
  typename std::vector<std::vector<Lima::LimaString > >::iterator answersIt = subwords.begin();
  for(  wordIt = hyperwords.begin(); wordIt != hyperwords.end() ; wordIt++ ) {
    try{
      Lima::LimaString word = *wordIt;
      std::pair<AccessSubWordIterator,AccessSubWordIterator> entries = m_dico.getSubWords(*offsetIt,word);
      FSAALOGINIT;
      LDEBUG <<  "test getSubWords("
             << ", " << word << ")" ;
      for( AccessSubWordIterator entry = entries.first ; entry != entries.second ; entry++ ) {
        LINFO << "string(" << *offsetIt << "," << (*entry).first << "),  ";
      }
      LINFO ;
      for( AccessSubWordIterator entry = entries.first  ; entry != entries.second ; entry++ ) {
        Lima::LimaString subWord = word.mid(*offsetIt, (*entry).first - *offsetIt);
        LINFO << subWord << ", ";
      }
      LINFO ;
      if( withAssert ) {
        // r�up�ation des r�onses attendues pour v�ifications
        assert( answersIt != subwords.end() );
        std::vector<Lima::LimaString > answers = *(answersIt++);
        typename std::vector<Lima::LimaString >::iterator answerIt = answers.begin();
        for( AccessSubWordIterator entry = entries.first  ; entry != entries.second ; entry++ ) {
          assert( answerIt != answers.end() );
          Lima::LimaString subWord = word.mid(*offsetIt, (*entry).first - *offsetIt);
          assert(!subWord.compare(*answerIt));
          answerIt++;
        }
      }
    }
    catch(std::logic_error e ) {
      std::cerr << "testSub: getSubWords exception: " << e.what() << std::endl;
    }
    offsetIt++;
  }
}

template <typename dictType>
void DictTester<dictType>::exec( void ) {
  if( m_param.withDebug ) {
    std::cerr <<  "Print dictionary...." << std::endl;
    m_dico.print(std::cout);
  }
}

template <typename dictType>
void DictTester<dictType>::write( void ) {
  try {
    if( m_param.outputDico.size() > 0 ) {
      std::cerr <<  "Write dictionary...." << std::endl;
       m_dico.write(m_param.outputDico);
    }
  }
  catch(LimaException e ) {
    std::cout << "write: exception: " << e.what() << std::endl;
  }
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  cerr << argv[0] << " begin..." << endl << "  command line: ";
  for (int i = 0; i < argc; i++)
  {
    std::cerr << argv[i] << " ";
  }
  std::cerr << std::endl;

  setlocale(LC_ALL, "");

  // options reading
  Param param = {
    std::string(),  // listOfWords
    std::string(),  // outputDico
    std::string(),  // inputDico
    false,          // subWord
    std::string(),  // listOfHyperwords
    false,          // superWord
    false,          // printGraph
    false,          // spareMem
    one_byte,       // charSize
    false,          // withoutTemplate
    true,           // trieDirectionForward
    false,          // withDebug
    false,          // runPerfo
    false,          // runIndex
    false,          // addWord
    false,          // runSpelling
    -1,             // termId (-1 means no termId specified by user)
    false,          // composed
    false,          // withAssert
    std::string(),  // inputDico
    std::string()   // configDir
  };

  for (int i = 1 ; i < argc; i++) {
	QString arg = QString::fromUtf8(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cerr << "       " << argv[0]
                << " [--output=<filename>]"
                << " [--input=<filename>]"
                << " [--printGraph]"
                << " [--subWord]"
                << " [--listOfHyperwords=<filename>]"
                << " [--listOfWords=<filename>]"
                << " [--superWord]"
                << " [--spare]"
                << " [--runIndex]"
                << " [--addWord]"
                << " [--runSpelling]"
                << " [--termId=nn"
                << " [--composed=<filename>]"
                << " [--charSize=<1|2|4>]"
                << " [--withoutTemplate"
                << " [--reverse]"
                << " [--withDebug]"
                << " [--runPerfo]"
                << " [--withAssert]"
                << std::endl;
      return 0;
    }
    else if ( (pos = arg.indexOf("--input=")) != -1 ){
      param.inputDico = arg.mid(pos+8).toUtf8().data();
    }
    else if ( (pos = arg.indexOf("--output=")) != -1 ){
      param.outputDico = arg.mid(pos+9).toUtf8().data();
    }
    else if ( arg =="--printGraph" ){
      param.printGraph = true;
    }
    else if ( arg == "--subWord" ){
      param.subWord = true;
    }
    else if ( (pos = arg.indexOf("--listOfHyperwords=")) != -1 ){
		param.listOfHyperwords = arg.mid(pos+19).toUtf8().data();
    }
    else if ( (pos = arg.indexOf("--listOfWords=")) != -1 ){
		param.listOfWords = arg.mid(pos+14).toUtf8().data();
    }
    else if ( arg == "--superWord" ){
      param.superWord = true;
    }
    else if ( arg == "--withDebug" ){
      param.withDebug = true;
    }
    else if ( arg == "--runPerfo" ){
      param.runPerfo = true;
    }
    else if ( arg == "--withoutTemplate" ){
      param.withoutTemplate = true;
    }
    else if ( (pos = arg.indexOf("--charSize=")) != -1 ){
      int charSize = (arg.midRef(pos+11)).toInt();
      switch(charSize) {
        case 1:
          param.charSize = one_byte;
          break;
        case 2:
          param.charSize = two_bytes;
          break;
        case 4:
          param.charSize = four_bytes;
          break;
      }
    }
    else if ( arg == "--spare" ){
      param.spareMem = true;
    }
    else if ( arg == "--runIndex" ){
      param.runIndex = true;
    }
    else if ( arg == "--addWord" ){
      param.addWord = true;
    }
    else if ( arg == "--runSpelling" ){
      param.runSpelling = true;
    }
    else if ( (pos = arg.indexOf("--termId=")) != -1 ){
      param.termId = (arg.midRef(pos+9)).toInt();
    }
    else if ( arg == "--reverse" ){
      param.trieDirectionForward = false;
    }
    else if ( (pos = arg.indexOf("--composed=")) != -1 ){
      param.composed = true;
	  param.inputDicoComp = arg.mid(pos+12).toUtf8().data();
    }
    else if ( arg == "--withAssert" ){
      param.withAssert = true;
    }
    else if ( (pos = arg.indexOf("--configDir=")) != -1 ){
      param.configDir = arg.mid(pos+12).toUtf8().data();
    }
  }

  cerr << argv[0] << ": ";
  if(param.withDebug)
    cerr << "--withDebug ";
  if(param.runPerfo)
    cerr << "--runPerfo ";
  if(param.spareMem)
    cerr << "--spare ";
  if(param.runIndex)
    cerr << "--runIndex ";
  if(param.addWord)
    cerr << "--addWord ";
  if(param.runSpelling)
    cerr << "--runSpelling ";
  if(param.printGraph)
    cerr << "--printGraph ";
  if(!param.trieDirectionForward)
    cerr << "--reverse ";
  if(!param.withoutTemplate)
    cerr << "--withoutTemplate ";
  if(param.subWord) {
    cerr << "--subWord ";
    if(param.listOfHyperwords.size()){
      cerr << "--listOfHyperwords=" << param.listOfHyperwords << " ";
    }
  }
  if(param.composed)
    cerr << "--composed=" << param.inputDicoComp << " ";
  cerr << "--charSize=" << param.charSize;
  if(param.inputDico.size()) {
    cerr << "--input='" << param.inputDico << "' ";
  }
  if(param.outputDico.size()) {
    cerr << "--output='" << param.outputDico << "' ";
  }
  if(param.listOfWords.size()) {
    cerr << "--listOfWords='" << param.listOfWords << "'";
  }
  cerr << endl;

  std::string configPath = (param.configDir.size()>0) ? param.configDir : string("");
  if (configPath.size() == 0)
    configPath = string(getenv("LIMA_CONF"));
  if (configPath.size() == 0)
    configPath = string("/usr/share/config/lima");

  if (QsLogging::initQsLog(QString::fromUtf8(configPath.c_str())) != 0)
  {
    FSAALOGINIT;
    LERROR << "Call to QsLogging::initQsLog(\"" << configPath << "\") failed.";
    return EXIT_FAILURE;
  }

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << argv[0] <<  " begin..." ;
#endif

  if( (!param.spareMem) && (param.addWord) ) {
    // Si Builder avec option addWord: BuilderRandom
    std::cerr <<  "Create BuilderRandom dictionary...." << std::endl;
    Lima::Common::FsaAccess::FsaAccessBuilderRandom16 dico(param.trieDirectionForward);
    if( param.inputDico.size() > 0) {
      std::cerr << "Read dictionary from file... "
                << param.inputDico << "..." << std::endl;
      dico.read(param.inputDico);
    }
    DictTester<Lima::Common::FsaAccess::FsaAccessBuilderRandom16> *wbuilderRandomTester16 = new DictTester<Lima::Common::FsaAccess::FsaAccessBuilderRandom16>( param, dico );
    if( param.listOfWords.size() > 0 ) {
      std::cerr << "addListOfRandomWords "
                << param.listOfWords << "..." << std::endl;
      wbuilderRandomTester16->addListOfUnorderedWords();
    }
    wbuilderRandomTester16->exec();
    wbuilderRandomTester16->write();
    delete wbuilderRandomTester16;
  }

  else if (!param.spareMem) {
    // Si Builder sans option addWord: Builder
    std::cerr << "Create dictionary...." << std::endl;
    Lima::Common::FsaAccess::FsaAccessBuilder16 dico(param.trieDirectionForward);
    if( param.inputDico.size() > 0) {
      std::cerr << "no read operation allowed for FsaAccessBuilder "
                << std::endl;
      return EXIT_FAILURE;
    }

    DictTester<Lima::Common::FsaAccess::FsaAccessBuilder16> *wbuilderTester16 = new DictTester<Lima::Common::FsaAccess::FsaAccessBuilder16>( param, dico );
    if (param.listOfWords.size() > 0) {
      std::cerr << "addListOfWords "
                << param.listOfWords << "..." << std::endl;
      wbuilderTester16->addListOfWords();
    }
    wbuilderTester16->exec();
    wbuilderTester16->write();
    delete wbuilderTester16;
  }
  else {
    int refSize = 1;
    int memSize = 0;
    int memSize0 = 0;
    if( param.runPerfo ) {
      refSize = logFileSize( param.listOfWords );
      logFileSize( param.inputDico );
      memSize0 = getProcStat( std::string("VmSize") );
      std::cout << "procSize before load dico = " << memSize0 << std::endl;
      TimeUtils::updateCurrentTime();
    }
    Lima::Common::FsaAccess::FsaAccessSpare16 dico;
    dico.read(param.inputDico);
    if( param.runPerfo ) {
      TimeUtils::logElapsedTime("load dico");
      memSize = getProcStat( std::string("VmSize") );
      std::cout << "procSize after load dico = " << memSize << std::endl;
      std::cout << "dico size in mem = " << memSize - memSize0 << std::endl;
      std::cout << "compression rate  = " << ((memSize - memSize0)*102400.0)/refSize << "%" << std::endl;
    }
    if( param.printGraph ) {
      std::cerr <<  "Print graph...." << std::endl;
      dico.printGraph(std::cerr);
    }
    DictTester<Lima::Common::FsaAccess::FsaAccessSpare16> *wspareTester16 = new DictTester<Lima::Common::FsaAccess::FsaAccessSpare16>(
      param, dico );

/*
    Lima::LimaString lcwlem0(Misc::utf8stdstring2limastring("b"));
    std::cerr << "lcwlem0=" << lcwlem0 << std::endl;
    Lima::LimaString & stlem0 = lcwlem0;
    Lima::LimaString & stlem1 = lcwlem1;
    Lima::LimaString & stlem2 = lcwlem2;
    Lima::LimaString & stlem3 = lcwlem3;
    Lima::LimaString & stlem4 = lcwlem4;
    Lima::LimaString & stlem5 = lcwlem5;
    Lima::LimaString & stlem6 = lcwlem6;
    Lima::LimaString & stlem7 = lcwlem7;
*/
    if( param.runIndex ) {
      std::cerr << "runIndex" << std::endl;
      std::vector<Lima::LimaString > listOfWords;
      std::vector<int> indexes;

      if( param.listOfWords.size() > 0 ) {
        readListOfWords(param.listOfWords, listOfWords );
      }
      else {
        Lima::LimaString lcwlem1(Misc::utf8stdstring2limastring("béc"));
        Lima::LimaString lcwlem2(Misc::utf8stdstring2limastring("séc"));
        Lima::LimaString lcwlem3(Misc::utf8stdstring2limastring("sél"));
        Lima::LimaString lcwlem4(Misc::utf8stdstring2limastring("sé"));
        Lima::LimaString lcwlem5(Misc::utf8stdstring2limastring("s"));
        Lima::LimaString lcwlem6(Misc::utf8stdstring2limastring("truc"));
        Lima::LimaString lcwlem7(Misc::utf8stdstring2limastring("table"));
        listOfWords.push_back( Lima::LimaString(lcwlem1) );
        indexes.push_back(1);
        listOfWords.push_back( Lima::LimaString(lcwlem2) );
        indexes.push_back(2);
        listOfWords.push_back( Lima::LimaString(lcwlem3) );
        indexes.push_back(3);
        listOfWords.push_back( Lima::LimaString(lcwlem4) );
        indexes.push_back(4);
        listOfWords.push_back( Lima::LimaString(lcwlem5) );
        indexes.push_back(-1);
        listOfWords.push_back( Lima::LimaString(lcwlem6) );
        indexes.push_back(-1);
        listOfWords.push_back( Lima::LimaString(lcwlem7) );
        indexes.push_back(-1);
      };
      std::cerr << "testIndex" << std::endl;
//      for( int i = 10 ; i > 0 ; i-- )
       TimeUtils::updateCurrentTime();
       wspareTester16->testIndex(listOfWords.begin(), listOfWords.end(), indexes );
       uint64_t elapsed = TimeUtils::elapsedTime();
       TimeUtils::logElapsedTime("testIndex");
       std::cout << "key average size = " << (refSize*1.0)/dico.getSize() << " byte" << std::endl;
       std::cout << "testIndex: average time = " << (elapsed*1000.0)/dico.getSize() << std::endl;
    }

    if( param.runSpelling ) {
      std::vector<Lima::LimaString > listOfWords;
      std::vector<int> indexes;

      // case 1: ask for spelling of a word given a termId
      if( param.termId > 0 ) {
        indexes.push_back(param.termId);
	std::cerr << "testSpelling with unique termId " << indexes[0] << std::endl;
      }
      // case 2: check if getSpelling is ok for every id
      // (listOfWords is supposed to contain the complete ordered list of terms
      else if( param.listOfWords.size() > 0 ) {
        readListOfWords(param.listOfWords, listOfWords );
	int index = 1;
	for( std::vector<Lima::LimaString >::const_iterator it = listOfWords.begin() ;
	  it != listOfWords.end() ; it++ ) {
	  indexes.push_back(index++);
	  std::cerr << "testSpelling with list of " << indexes.size() << " words" << std::endl;
	}
      }
      wspareTester16->testSpelling(listOfWords.begin(), listOfWords.end(), indexes );
    }
    if( param.superWord) {
      std::cerr << "runSuper" << std::endl;
      std::vector<Lima::LimaString > listOfWords;
      Lima::LimaString vide;
      listOfWords.push_back(vide);

      if( param.listOfWords.size() > 0 ) {
        readListOfWords(param.listOfWords, listOfWords );
      }
      wspareTester16->testSuper(listOfWords.begin(), listOfWords.end());
    }

    wspareTester16->exec();
    if( param.subWord) {
      // cha�e �d�ouper
      std::vector<Lima::LimaString > hyperwords;
      // offset de localisation de l'hypermot dans la cha�e
      std::vector<int> offsets;
      // r�onses du dictionnaire sur l'appel �getSubword
      std::vector<std::vector<Lima::LimaString > > subwords;
      if( param.listOfHyperwords.size() > 0 ) {
        std::ifstream Hlist(param.listOfHyperwords.c_str(), std::ios::in | std::ios::binary );
        if ( !Hlist.is_open() ) {
          std::cerr <<  "Cannot open list of (hyperword,offset..) " << param.listOfHyperwords << std::endl;
          return EXIT_FAILURE;
        }
        std::cerr <<  "Read hyperword and offset...." << std::endl;
        std::string line;

        for( int counter = 0 ; ; counter++ ) {
          // lecture d'une ligne du fichier de test
          line = Lima::Common::Misc::readLine(Hlist);
          if( line.size() == 0 ) {
            std::cerr <<  "end of list of (hyperword,offset)." << std::endl;
            break;
          }
          else {
            // extraction chaine a decouper
            std::string::size_type hyperword_pos = line.find(';');
            std::string utf8_hyperword(line, 0, hyperword_pos);
            Lima::LimaString hyperword = Lima::Common::Misc::utf8stdstring2limastring(utf8_hyperword);
            hyperwords.push_back(hyperword);
            std::cerr << "push(" << hyperword;
//            std::cerr << "offset=" << hyperword_pos << std::endl;
            // extraction offset
            std::string::size_type offset_pos = line.find(';', hyperword_pos+1);
            std::string offset_str(line, hyperword_pos+1, offset_pos-(hyperword_pos+1));
            int offset = std::atoi(offset_str.c_str());
            offsets.push_back(offset);
            std::cerr << "," << offset;
//            std::cerr << "offset=" << offset_pos << std::endl;
            // extraction liste de r�onses attendues
            std::vector<Lima::LimaString > answers;
            std::string::size_type subword_pos0 = offset_pos;
            std::string::size_type subword_pos = line.find(';', subword_pos0+1);
            for( ; subword_pos != std::string::npos ; subword_pos = line.find(';', subword_pos0+1) ) {
              std::string utf8_answer(line, subword_pos0+1, subword_pos-(subword_pos0+1));
              Lima::LimaString answer = Lima::Common::Misc::utf8stdstring2limastring(utf8_answer);
              answers.push_back(answer);
              std::cerr << "," << answer;
//              std::cerr << "offset=" << subword_pos << std::endl;
              subword_pos0 = subword_pos;
            }
            subwords.push_back(answers);
            std::cerr << ")" << std::endl;
          }
        }
      }
      else {
        Lima::LimaString lcwhyper1(Misc::utf8stdstring2limastring("séc"));
        Lima::LimaString lcwhyper2(Misc::utf8stdstring2limastring("abcséc"));
        Lima::LimaString lcwhyper3(Misc::utf8stdstring2limastring("truc"));
        Lima::LimaString & stlem1 = lcwhyper1;
        Lima::LimaString & stlem2 = lcwhyper2;
        Lima::LimaString & stlem3 = lcwhyper3;

        hyperwords.push_back(Lima::LimaString(stlem1)); // s�
        offsets.push_back(0);
        hyperwords.push_back(Lima::LimaString(stlem2)); // abcs�
        offsets.push_back(3);
        hyperwords.push_back(Lima::LimaString(stlem3));  // truc
        offsets.push_back(0);
      }
      wspareTester16->testSub(hyperwords, offsets, subwords, param.withAssert);
    }
    delete wspareTester16;
//    wspareTester16->write();
  }

  return EXIT_SUCCESS;
}
