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
 *                         testLexicon.cpp                                 *
 *  begin                : mon nov 7 2005                                  *
 *  copyright            : (C) 2003-2005 by CEA                            *
 *  email                : olivier.mesnard@cea.fr                          *
 *  description                                                            *
 *  test of lexiconIdGenerator and lexiconIdAccessor                       *
 *                                                                         *
 ***************************************************************************/

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/FsaAccess/lexiconIdGenerator.h"
#include "common/FsaAccess/lexiconIdAccessor.h"
#include "common/FsaAccess/FsaAccessIOStreamWrapper.h"

#include <QtCore/QCoreApplication>

#include <iostream>
#include <fstream>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;


// options
typedef struct ParamStruct {
  std::string listOfWords;
  std::string outputDico;
  std::string inputDico;
  bool doPrint;
} Param;

typedef std::map<LimaString,uint64_t> String2IdMap;
class ByMapStringAccess : public String2IdMap {
 public:
   ByMapStringAccess() : m_size(0) {}
   uint64_t getIndex(const LimaString & word ) const;
   uint64_t getIndex(const LimaString & word );
   uint64_t getSize() const;
   void print(std::ostream& os) const;
   //LimaString getSpelling(const uint64_t& termId);
   void write(Common::FsaAccess::FsaAccessDataOutputHandler& outputHandler) const;
   void read(Common::FsaAccess::FsaAccessDataInputHandler& inputHandler);
 private:
   uint64_t m_size;
};

void ByMapStringAccess::print(std::ostream& os) const {
  os << "ByMapStringAccess = {" << std::endl;
  String2IdMap::const_iterator it = begin();
  if( it != end() ) {
    std::string word = Common::Misc::limastring2utf8stdstring((*it).first);
    os << "(" << word << " -> " << (*it).second << ")";
  }
  it++;
  for( ; it != end() ; it++ ) {
    std::string word = Common::Misc::limastring2utf8stdstring((*it).first);
    os << ", (" << word << " -> " << (*it).second << ")";
  }
  os << std::endl;
}

uint64_t ByMapStringAccess::getSize() const {
  return size();
}

uint64_t ByMapStringAccess::getIndex(const LimaString & word ) const {
  String2IdMap::const_iterator pos = String2IdMap::find( word );
  if( pos != end() ) {
    return( (*pos).second );
  }
  return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
}

uint64_t ByMapStringAccess::getIndex(const LimaString & word ) {
  String2IdMap::const_iterator pos = String2IdMap::find( word );
  if( pos != end() ) {
    return( (*pos).second );
  }
  insert(std::pair<LimaString,uint64_t>(word,++m_size));
  return m_size;
}

void ByMapStringAccess::write(
  Common::FsaAccess::FsaAccessDataOutputHandler& outputHandler) const
{

  uint64_t size = String2IdMap::size();
  std::cerr << "outputHandler.saveData(size," << size << ")" << std::endl;
  outputHandler.saveData((const char*)&size, sizeof(size) );

  String2IdMap::const_iterator it = begin();
  for( ; it != end() ; it++ ) {
    std::string word = Common::Misc::limastring2utf8stdstring((*it).first);
    uint64_t length = word.length();
    std::cerr << "outputHandler.saveData(length," << length << ")" << std::endl;
    outputHandler.saveData((const char*)&length, sizeof(length) );
    std::cerr << "outputHandler.saveData(c_str," << word << ")" << std::endl;
    outputHandler.saveData((const char*)word.c_str(), length );
    std::cerr << "outputHandler.saveData(id," << (*it).second << ")" << std::endl;
    uint64_t termId = (*it).second;
    outputHandler.saveData((const char*)&termId, sizeof(termId) );
  }
}

void ByMapStringAccess::read(
 Common::FsaAccess::FsaAccessDataInputHandler& inputHandler)
{
  inputHandler.restoreData((char *)(&m_size), sizeof(m_size) ) ;
//   String2IdMap::const_iterator it = begin();
  char buff[200];
  for( uint64_t i = 0 ; i < m_size ; i++ ) {
    uint64_t length;
    inputHandler.restoreData((char*)&length, sizeof(length) );
    inputHandler.restoreData((char*)buff, length );
    std::string word8( buff, length );
    LimaString word = Common::Misc::utf8stdstring2limastring(word8);
    uint64_t tId;
    inputHandler.restoreData((char*)&tId, sizeof(tId) );
    insert(std::make_pair(word,tId));
  }
}

class TestDataOutputHandler : public Common::FsaAccess::FsaAccessDataOutputHandler {
public: 
  TestDataOutputHandler( std::string filename );
  ~TestDataOutputHandler();
  void saveData( const char* m_buffer, uint64_t m_size ) override;
private:
  std::ofstream m_os;
};

TestDataOutputHandler::TestDataOutputHandler( std::string filename )
  : m_os(filename.c_str(), std::ios::out | std::ios::binary  ) {
}

TestDataOutputHandler::~TestDataOutputHandler() {
  m_os.close();
}

void TestDataOutputHandler::saveData( const char* m_buffer, uint64_t m_size ) {
  m_os.write( m_buffer, m_size );
}

class TestDataInputHandler : public Common::FsaAccess::FsaAccessDataInputHandler {
public: 
  TestDataInputHandler( std::string filename );
  ~TestDataInputHandler();
  TestDataInputHandler() {}
  void restoreData( char* m_buffer, uint64_t m_size ) override;
private:
  std::ifstream m_is;
};

TestDataInputHandler::TestDataInputHandler( std::string filename )
  : m_is(filename.c_str(), std::ios::in | std::ios::binary  ) {
}

TestDataInputHandler::~TestDataInputHandler() {
  m_is.close();
}

void TestDataInputHandler::restoreData( char* m_buffer, uint64_t m_size )
{
  m_is.read( m_buffer, m_size );
  uint64_t size = m_is.gcount();
  if( size < m_size ) {
    std::ostringstream ostr;
    ostr << "TestDataInputHandler::restoreData("
         << m_size << "): no more data ! gcount = " << size;
    std::cerr << ostr.str() << std::endl;
    throw(AccessByStringNotInitialized(ostr.str()));
  }
}

class MyInformer : public Common::AbstractLexiconIdGeneratorInformer {
public:
  virtual int operator() (
    uint64_t cmp1, uint64_t cmp2, uint16_t type, uint64_t newId) override;
  static AbstractLexiconIdGeneratorInformer* getInstance();

};

int MyInformer::operator() (
    uint64_t cmp1, uint64_t cmp2, uint16_t type, uint64_t newId )
{
  std::cout << "inform(" << cmp1 << "," << cmp2 << "," << type << "," << newId << ")" << std::endl;
  return 1;
}


void test_print() {
  MyInformer myInformer;
  LexiconIdGenerator<ByMapStringAccess> lexiconIdGenerator(&myInformer);
//   LimaString termIncr=Common::Misc::utf8stdstring2limastring("lemmeIncrEntry");
  
//   uint64_t idTIncr = lexiconIdGenerator.getId(termIncr);
  
  lexiconIdGenerator.print( std::cout );
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  std::cerr << "testLexicon begin..." << std::endl;
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "testLexicon begin..." ;
#endif

  // options reading
//   Param param = {
//     std::string(),  // listOfWords
//     std::string(),  // outputDico
//     std::string(),  // inputDico
//     false
//   };

  MyInformer myInformer;
  LexiconIdGenerator<ByMapStringAccess> lexiconIdGenerator(&myInformer);
  // stabilite politique et de la monnaie nationale
  LimaString t1(Common::Misc::utf8stdstring2limastring(std::string("stabilite")));
  LimaString e1(Common::Misc::utf8stdstring2limastring(std::string("politique")));
  LimaString t2_1(Common::Misc::utf8stdstring2limastring(std::string("monnaie")));
  LimaString e2_1(Common::Misc::utf8stdstring2limastring(std::string("nationale")));
  uint64_t idT1 = lexiconIdGenerator.getId(t1);
  std::cerr << "idT1=" << idT1 << std::endl;
  uint64_t idT1_prim = lexiconIdGenerator.getId(t1);
  assert(idT1_prim == idT1);
  uint64_t idE1 = lexiconIdGenerator.getId(e1);
  std::cerr << "idE1=" << idE1 << std::endl;
  uint64_t idE1_prim = lexiconIdGenerator.getId(e1);
  assert(idE1_prim == idE1);
  uint64_t idT2_1 = lexiconIdGenerator.getId(t2_1);
  std::cerr << "idT2_1=" << idT2_1 << std::endl;
  uint64_t idT2_1_prim = lexiconIdGenerator.getId(t2_1);
  assert(idT2_1_prim == idT2_1);
  uint64_t idE2_1 = lexiconIdGenerator.getId(e2_1);
  std::cerr << "idE2_1=" << idE2_1 << std::endl;
  uint64_t idE2_1_prim = lexiconIdGenerator.getId(e2_1);
  assert(idE2_1_prim == idE2_1);
  std::vector<uint64_t> vE2;
  vE2.push_back(idT2_1);
  vE2.push_back(idE2_1);

  uint64_t idE2 = lexiconIdGenerator.getId(vE2);
  std::cerr << "idE2=" << idE2 << std::endl;
  uint64_t idE2_prim = lexiconIdGenerator.getId(vE2);
  assert( idE2_prim == idE2 );
  std::vector<uint64_t> vE1;
  vE1.push_back(idT1);
  vE1.push_back(idE1);
  vE1.push_back(idE2);
  uint64_t idE = lexiconIdGenerator.getId(vE1);
  std::cerr << "idE=" << idE << std::endl;
  uint64_t idE_prim = lexiconIdGenerator.getId(vE1);
  assert( idE_prim == idE );

  std::cerr << "lexiconIdGenerator.getSize() = " << lexiconIdGenerator.getSize() << std::endl;
  std::cerr << "lexiconIdGenerator = " << std::endl;
  lexiconIdGenerator.print(std::cerr);
  std::cerr << std::endl;

  // attempt to get Id in case of word already exists
  const LexiconIdGenerator<ByMapStringAccess>& constLexicon = lexiconIdGenerator;
  uint64_t idT1_second = constLexicon.getId(t1);
  assert(idT1_second == idT1);
  std::cerr << "idT1_second=" << idT1_second << std::endl;

  // create a handler to get lexicon data from the generator (abs save to file!)
  {
    TestDataOutputHandler outputHandler( "simplewords.dat" );
    // store simple word lexicon data in it
    lexiconIdGenerator.write(outputHandler);
  }

  // create a LexiconIdAccessor
  LexiconIdAccessor<ByMapStringAccess> lexiconIdAccessor;
  {
    // create a handler to feed the accessor with the simple word lexicon data
    TestDataInputHandler intputHandler( "simplewords.dat" );
    // fill accessor with siple word data from generator
    lexiconIdAccessor.read(intputHandler);
  }
  
  std::cerr << "lexiconIdAccessor.getSize() = " << lexiconIdAccessor.getSize() << std::endl;
  std::cerr << "lexiconIdAccessor = " << std::endl;
  lexiconIdAccessor.print(std::cerr);
  std::cerr << std::endl;

  LimaString unKnownTerm(Common::Misc::utf8stdstring2limastring(std::string("schsaipa")));
  uint64_t idUnknown = lexiconIdAccessor.getId(unKnownTerm);
  assert( idUnknown == Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID );
  assert( idE2 == lexiconIdAccessor.getId(vE2) );
  assert( idE == lexiconIdAccessor.getId(vE1) );
  // LimaString t1(Common::Misc::utf8stdstring2limastring(std::string("stabilite")));
  // LimaString e1(Common::Misc::utf8stdstring2limastring(std::string("politique")));
  // LimaString t2_1(Common::Misc::utf8stdstring2limastring(std::string("monnaie")));
  // LimaString e2_1(Common::Misc::utf8stdstring2limastring(std::string("nationale")));
  // uint64_t idT1 = lexiconIdGenerator.getId(t1);
  // uint64_t idE1 = lexiconIdGenerator.getId(e1);
  // uint64_t idT2_1 = lexiconIdGenerator.getId(t2_1);
  // uint64_t idE2_1 = lexiconIdGenerator.getId(e2_1);
  
  uint64_t dup_idT1 = lexiconIdAccessor.getId(t1);
  assert( dup_idT1 == idT1);
  uint64_t dup_idE1 = lexiconIdAccessor.getId(e1);
  assert( dup_idE1 == idE1);
  uint64_t dup_idT2_1 = lexiconIdAccessor.getId(t2_1);
  assert( dup_idT2_1 == idT2_1);
  uint64_t dup_idE2_1 = lexiconIdAccessor.getId(e2_1);
  assert( dup_idE2_1 == idE2_1);
  std::vector<uint64_t> dup_vE2;
  dup_vE2.push_back(dup_idT2_1);
  dup_vE2.push_back(dup_idE2_1);
  uint64_t dup_idE2 = lexiconIdAccessor.getId(dup_vE2);
  std::cerr << "dup_idE2=" << dup_idE2 << std::endl;
  assert( dup_idE2 == idE2 );
  std::vector<uint64_t> dup_vE1;
  dup_vE1.push_back(dup_idT1);
  dup_vE1.push_back(dup_idE1);
  dup_vE1.push_back(dup_idE2);
  uint64_t dup_idE = lexiconIdAccessor.getId(dup_vE1);
  assert( dup_idE == idE );
  std::cerr << "dup_idE=" << dup_idE << std::endl;
  
  // create a duplicate of LexiconIdGenerator to test operation read() and add of new word
  LexiconIdGenerator<ByMapStringAccess> increaseLexiconIdGenerator(&myInformer);
  {
    // create a handler to feed the duplicate generator with the lexicon data
    TestDataInputHandler intputHandler( "simplewords.dat" );
    // fill generator with data from generator
    increaseLexiconIdGenerator.read(intputHandler);
  }
  std::cerr << "increaseLexiconIdGenerator.getSize() = " << increaseLexiconIdGenerator.getSize() << std::endl;
  std::cerr << "increaseLexiconIdGenerator = " << std::endl;
  increaseLexiconIdGenerator.print(std::cerr);
  std::cerr << std::endl;

  const LexiconIdGenerator<ByMapStringAccess> constLexiconIdGenerator = increaseLexiconIdGenerator;
  // to test operation read()
  uint64_t const_idT1 = constLexiconIdGenerator.getId(t1);
  assert( const_idT1 == idT1);
  uint64_t const_idE1 = constLexiconIdGenerator.getId(e1);
  assert( const_idE1 == idE1);
  uint64_t const_idT2_1 = constLexiconIdGenerator.getId(t2_1);
  assert( const_idT2_1 == idT2_1);
  uint64_t const_idE2_1 = constLexiconIdGenerator.getId(e2_1);
  assert( const_idE2_1 == idE2_1);
  std::vector<uint64_t> const_vE2;
  const_vE2.push_back(const_idT2_1);
  const_vE2.push_back(const_idE2_1);
  uint64_t const_idE2 = constLexiconIdGenerator.getId(const_vE2);
  std::cerr << "const_idE2=" << const_idE2 << std::endl;
  assert( const_idE2 == idE2 );
  std::vector<uint64_t> const_vE1;
  const_vE1.push_back(const_idT1);
  const_vE1.push_back(const_idE1);
  const_vE1.push_back(const_idE2);
  uint64_t const_idE = constLexiconIdGenerator.getId(const_vE1);
  assert( const_idE == idE );
  std::cerr << "const_idE=" << const_idE << std::endl;
  // to test add a new word
  LimaString t3(Common::Misc::utf8stdstring2limastring(std::string("newHead")));
  uint64_t idT3 = increaseLexiconIdGenerator.getId(t3);
  assert( idT3 > idE2_1);
  LimaString e4(Common::Misc::utf8stdstring2limastring(std::string("newExt")));
  uint64_t idE4 = increaseLexiconIdGenerator.getId(e4);
  // to test add a new compound
  std::vector<uint64_t> vE3;
  vE3.push_back(idT3);
  vE3.push_back(idE4);
  vE3.push_back(idE2);
  uint64_t idNewComp = increaseLexiconIdGenerator.getId(vE3);
  assert( idNewComp > idE);

  test_print();

  return EXIT_SUCCESS;
}
