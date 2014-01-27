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
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  CompoundStringAccess.h  -  description                                 *
 *  AccessByString optimized for lexicon with coupounds terms              *
 *  based on FsaAccesSpare16 for simple words and                          *
 *  std::map<pair<id1,id2>,id3> for compounds.                             *
 ***************************************************************************/
#ifndef FSA_COMPOUNDS_STRING_ACCESS_H
#define FSA_COMPOUNDS_STRING_ACCESS_H


#include "common/LimaCommon.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

// typedef std::map<std::pair<uint64_t,uint64_t>, uint64_t > BuilderCompoundsSet;

// Structure pour la liste candidats par ordre de taille de contexte d�roissant
struct ltTermIdPair
{
  bool operator()(const std::pair<uint64_t,uint64_t>& p1, const std::pair<uint64_t,uint64_t>& p2) const
  {
    if( p1.first < p2.first )
      return true;
    else if( p1.first > p2.first )
      return false;
    else
      return ( p1.second < p2.second);
  }
};


typedef struct _compoundsComposition {
  uint64_t cmp1Id;
  uint64_t cmp2Id;
  uint64_t tId3;
} CompoundsComposition;

template <typename SimpleAccessByString> class CompoundStringAccess
{
  public:
    CompoundStringAccess(bool trie_direction_fwd=true);
    virtual ~CompoundStringAccess();
     
    uint64_t getSize() const;
    int64_t getIndex( const std::vector<Lima::LimaString > & compounds ) const;
    void getSpelling(const uint64_t index, 
      std::vector<Lima::LimaString > & compounds ) const;
    void read ( const std::string & filename );
    void read ( std::istream & is );
    void write( const std::string & filename  );
    void write( std::ostream & os  );
    void readSimple ( const std::string& filename );

    void readCompoundBody( std::istream& is );
//    void writeBody( const std::string & filename, struct stat& sts )
    void writeCompoundBody( std::ostream& os );

    void addWord ( const std::vector<Lima::LimaString > & compounds );
    void addWord ( Lima::LimaString & word );
    
  private:
    int64_t findId3( uint64_t tId1, uint64_t tId2 ) const;
    const CompoundsComposition& insertComposition( uint64_t tId1, uint64_t tId2 );
    int64_t findPos( uint64_t tId1, uint64_t tId2, uint64_t& pos ) const;

//    uint64_t m_maxSimpleId;
    
    SimpleAccessByString m_simpleAccessByString;
    
    // utilisé par le mode builder pour gérer les mots composés (addWord)
    typedef std::map<std::pair<uint64_t,uint64_t>, uint64_t, ltTermIdPair> BuilderCompoundsSet;
    BuilderCompoundsSet m_builderCompoundsSet;
    uint64_t m_nextTid;
    // utilisé par le mode reader pour gérer les mots composés
    typedef std::vector<CompoundsComposition> ReaderCompoundsSet;
    ReaderCompoundsSet m_readerCompoundsSet;
    // utilisé par le mode reader pour la fonction getSpelling    
    std::vector<uint64_t> m_compoundsDecomposition;
};

template <typename SimpleAccessByString>
CompoundStringAccess<SimpleAccessByString>::CompoundStringAccess( bool trie_direction_fwd)
  : m_simpleAccessByString(trie_direction_fwd), m_nextTid(MAX_SIMPLE_TID)
{
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::CompoundStringAccess()" << LENDL;
#endif
}

template <typename SimpleAccessByString>
CompoundStringAccess<SimpleAccessByString>::~CompoundStringAccess()
{
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::~CompoundStringAccess()" << LENDL;
#endif
}
    
template <typename SimpleAccessByString>
uint64_t CompoundStringAccess<SimpleAccessByString>::getSize() const {
  return( m_simpleAccessByString.getSize() + m_readerCompoundsSet.size() );
}
    
template <typename SimpleAccessByString>
int64_t CompoundStringAccess<SimpleAccessByString>::getIndex (
  const std::vector<Lima::LimaString > & compounds ) const {
  
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::getIndex(";
  std::vector<Lima::LimaString >::const_iterator forwardCompoundsIt = compounds.begin();
  if( forwardCompoundsIt != compounds.end() ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*forwardCompoundsIt);
    LDEBUG << text8;
    forwardCompoundsIt++;
  }
  for( ; forwardCompoundsIt != compounds.end() ; forwardCompoundsIt++ ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*forwardCompoundsIt);
    LDEBUG << text8;
  }
  LDEBUG << ")" << LENDL;
#endif
  // Iterateur pour parcourir du dernier au premier les composants
  std::vector<Lima::LimaString >::const_reverse_iterator compoundsIt =
    compounds.rbegin();
  if( compoundsIt == compounds.rend() ) {
    return -1;
  }
  // index du premier composant;
  int64_t tId2 = m_simpleAccessByString.getIndex( *compoundsIt );
#ifdef DEBUG_CD
  LDEBUG << "CompoundStringAccess::getIndex: m_simple.getIndex(" << Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt) << ")= tId2 =" << tId2 << LENDL;
#endif
  compoundsIt++;
  for( ; compoundsIt != compounds.rend(); compoundsIt++ ) {
    uint64_t tId1 = m_simpleAccessByString.getIndex( *compoundsIt );
#ifdef DEBUG_CD
  LDEBUG << "CompoundStringAccess::getIndex: m_simple.getIndex(" << Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt) << ")= tId1 =" << tId1 << LENDL;
#endif
#ifdef DEBUG_CD
  LDEBUG << "CompoundStringAccess::getIndex: findId3(" << tId1 << "," << tId2 << ")= tId2 =";
#endif
    tId2 = findId3(tId1,tId2);
#ifdef DEBUG_CD
  LDEBUG << tId2 << LENDL;
#endif
    if( tId2 == -1 )
      return -1;
  }
  return tId2;
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::addWord (
   Lima::LimaString & word ) {
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::addWord(" << word << ")";
#endif
  m_simpleAccessByString.addWord(word);
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::addWord (
  const std::vector<Lima::LimaString > & compounds ) {
  COMPSTRACCESSLOGINIT;
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::addWord(";
  std::vector<Lima::LimaString >::const_iterator forwardCompoundsIt = compounds.begin();
  if( forwardCompoundsIt != compounds.end() ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*forwardCompoundsIt);
    LDEBUG << text8;
    forwardCompoundsIt++;
  }
  for( ; forwardCompoundsIt != compounds.end() ; forwardCompoundsIt++ ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*forwardCompoundsIt);
    LDEBUG << text8;
  }
  LDEBUG << ")" << LENDL;
#endif
  
  // Iterateur pour parcourir du dernier au premier les composants
  std::vector<Lima::LimaString >::const_reverse_iterator compoundsIt =
    compounds.rbegin();
  if( compoundsIt == compounds.rend() ) {
    return;
  }
  uint64_t tId2 = m_simpleAccessByString.getIndex( *compoundsIt );
  if( tId2 == 0 ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
#ifdef DEBUG_CD
    LERROR << "CompoundStringAccess::addWord: " << text8
           << " is not in m_simpleAccessByString!" << LENDL;
#endif
  }
  compoundsIt++;
  for( ; compoundsIt != compounds.rend() ; compoundsIt++ ) {
    uint64_t tId1 = m_simpleAccessByString.getIndex( *compoundsIt );
    if( tId1 == 0 ) {
      std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
      LERROR << text8 << LENDL;
      return;
    }
//    const CompoundsComposition& composition = insertComposition( tId1, tId2 );
    BuilderCompoundsSet::iterator pos = m_builderCompoundsSet.find(
      std::make_pair(tId1, tId2) );
    uint64_t tId3;
    if( pos != m_builderCompoundsSet.end() ) {
      tId3 = (*pos).second;
    }
    else {
      tId3 = m_nextTid++;
#ifdef DEBUG_CD
      LDEBUG << "CompoundStringAccess::addWord: insert( (" << tId1
             << "," <<  tId2 << ") , " << tId3 << ")" << LENDL;
#endif
//      m_builderCompoundsSet.insert(make_pair(make_pair(tId1, tId2), tId3) );
      m_builderCompoundsSet.insert( std::make_pair ( std::make_pair(tId1, tId2), tId3 ) );
    }
    tId2 = tId3;
  }
}

// recherche dichotomique de tid3
template <typename SimpleAccessByString>
int64_t CompoundStringAccess<SimpleAccessByString>::findId3(
    uint64_t tId1, uint64_t tId2 ) const {
  uint64_t pos;
  return findPos( tId1, tId2, pos );
}

/*
// insertion d'un nouveau composant
template <typename SimpleAccessByString>
const CompoundsComposition& CompoundStringAccess<SimpleAccessByString>::insertComposition(
  uint64_t tId1, uint64_t tId2 ) {
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::insertComposition(" << tId1 << "," << tId2 << ")" << LENDL;
#endif
  
  uint64_t pos;
  uint64_t tId3 = findPos( tId1, tId2, pos );
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::insertComposition: tId3=" << tId3
         << ", pos = " << pos << ")" << LENDL;
#endif
  
  m_compoundsDecomposition.size();
  
  m_compoundsDecomposition.push_back(pos);
  CompoundsComposition composition;
  composition.cmp1Id = tId1;
  composition.cmp2Id = tId2;
  composition.tId3 = m_maxSimpleId + m_compoundsDecomposition.size();
  CompoundsSet::iterator newCompPos = m_compoundsSet.insert( m_compoundsSet.begin() + pos, composition);
  return *newCompPos;
}
*/

// recherche dichotomique de tid3
template <typename SimpleAccessByString>
int64_t CompoundStringAccess<SimpleAccessByString>::findPos(
    uint64_t tId1, uint64_t tId2, uint64_t& pos ) const {
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::findPos(" << tId1 << "," << tId2 << ")" << LENDL;
#endif
  
  uint64_t min = 0;
  uint64_t max = m_readerCompoundsSet.size();
  // test au milieu du segment [min,max[
  for( ; (max - min) > 1 ; ) {
    uint64_t range = (max - min)/2;
    const CompoundsComposition& composition = m_readerCompoundsSet[min+range];
    if( composition.cmp1Id < tId1 ) {
#ifdef DEBUG_CD
      LDEBUG <<  "CompoundStringAccess::findPos: comp.cmp1Id < tId1, range = " << range
             << "," << composition.cmp1Id << "<" << tId1 << ": min <- min + range" << LENDL;
#endif
      min = min + range;
    }
    else if( composition.cmp1Id > tId1 ) {
#ifdef DEBUG_CD
      LDEBUG <<  "CompoundStringAccess::findPos: compcmp1Id > tId1, range = " << range
             << "," << composition.cmp1Id << ">" << tId1 << ": max <- min + range" << LENDL;
#endif
      max = min + range;
    }
    else if( composition.cmp2Id < tId2 ) {
#ifdef DEBUG_CD
      LDEBUG <<  "CompoundStringAccess::findPos: comp.cmp2Id < tId2 , range = " << range
             << "," << composition.cmp2Id << "<" << tId2 << ": min <- min + range" << LENDL;
#endif
      min = min + range;
    }
    else if( composition.cmp2Id > tId2 ) {
#ifdef DEBUG_CD
      LDEBUG <<  "CompoundStringAccess::findPos: comp.cmp2Id > tId2 , range = " << range
             << "," << composition.cmp2Id << ">" << tId2 << ": max <- min + range" << LENDL;
#endif
      max = min + range;
    }
    else {
#ifdef DEBUG_CD
      LDEBUG <<  "CompoundStringAccess::findPos: else , range = " << range
             << "," << "return: tId3 = " << composition.tId3 << LENDL;
#endif
      pos = min+range;
      return composition.tId3;
    }
  }
  if(max == min) {
    pos = min;
    return -1;
  }
  const CompoundsComposition& composition = m_readerCompoundsSet[min];
  if( composition.cmp1Id == tId1 ) {
    if( composition.cmp2Id == tId2 ) {
#ifdef DEBUG_CD
      LDEBUG << "CompoundStringAccess::findPos: comp.cmp1Id == tId1, comp.cmp2Id == tId2"
             << ", return: tId3 = " << composition.tId3 << LENDL;
#endif
      pos = min;
      return composition.tId3;
    }
    else if( composition.cmp2Id < tId1 ) {
      pos = max;
    }
    else if( composition.cmp2Id > tId2 ) {
      pos = min;
    }
  }
  else if( composition.cmp1Id > tId1 ) {
    pos = min;
  }
  else{
    pos = max;
  }
#ifdef DEBUG_CD
  LDEBUG << "CompoundStringAccess::findPos: comp.cmp1Id != tId1  || comp.cmp2Id != tId2"
         << ", return: -1!" << LENDL;
#endif
  return -1;
}

template <typename SimpleAccessByString>
  void CompoundStringAccess<SimpleAccessByString>::getSpelling(uint64_t index, 
    std::vector<Lima::LimaString > & compound ) const {
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::getSpelling(" << index << ")" << LENDL;
#endif
  while( index >= MAX_SIMPLE_TID ) {
    if( (index - MAX_SIMPLE_TID) >= m_compoundsDecomposition.size() ) {
      std::ostringstream ostr;
      ostr << "ERROR! CompoundStringAccess::getSpelling: index (" << index 
           << ") - MAX_SIMPLE_TID (" << MAX_SIMPLE_TID
           << ") >= m_compoundsDecomposition.size() (" << m_compoundsDecomposition.size() << ")";
      throw( AccessByStringOutOfRange(ostr.str()) );
    }

    uint64_t offset = m_compoundsDecomposition[index - MAX_SIMPLE_TID];
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::getSpelling: offset=" << offset << LENDL;
#endif
    const CompoundsComposition& composition = m_readerCompoundsSet[offset];
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::getSpelling: Composition={("
         << composition.tId3 << ") -> "
         << composition.cmp1Id << "," << composition.cmp2Id << LENDL;
#endif
    assert(composition.tId3 == index);
    index = composition.cmp2Id;
    compound.push_back(m_simpleAccessByString.getSpelling(composition.cmp1Id));
  }
  if( index >= m_simpleAccessByString.getSize() ) {
    std::ostringstream ostr;
    ostr << "ERROR! CompoundStringAccess::getSpelling: index (" << index 
         << ") >= m_simpleAccessByString.getSize() (" << m_simpleAccessByString.getSize() << ")";
    throw( AccessByStringOutOfRange(ostr.str()) );
  }
  compound.push_back(m_simpleAccessByString.getSpelling(index));
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::readSimple ( 
  const std::string & filename )
{
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::readSimple(" << filename << ")" << LENDL;
#endif
  m_simpleAccessByString.read(filename);
  
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::read ( 
  const std::string & filename )
{
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::read(" << filename << ")" << LENDL;
#endif
  std::ifstream is(filename.c_str(), std::ios::binary );
  if( is.bad() ) {
    std::string mess = "CompoundStringAccess::read: Can't open file " + filename;
#ifdef DEBUG_CD
    LERROR << mess << LENDL;
#endif
    throw( AccessByStringNotInitialized( mess ) );
  }
  
  CompoundStringAccess<SimpleAccessByString>::read ( is );
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::read ( 
  std::istream& is )
{
#ifdef DEBUG_CD
  COMPSTRACCESSLOGINIT;
  LDEBUG <<  "CompoundStringAccess::read()" << LENDL;
#endif
  
  m_simpleAccessByString.read( is );
  
  readCompoundBody( is );
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::readCompoundBody( 
  std::istream& is )
{
  COMPSTRACCESSLOGINIT;
/*  uint64_t compoundsDataStreampos = m_simpleAccessByString.FsaAccessHeader::getStreamPos();
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody(" << filename << "):"
         <<  " compoundsDataStreampos=" << compoundsDataStreampos << LENDL;
#endif
*/  
  // offset pour atteindre les mots compos
//  uint64_t compoundsDataOffset =
//     m_simpleAccessByString.FsaAccessHeader::getCompoundsDataOffset();
  // nombre de mots composes pour reserver la memoire
//  uint64_t nbCompounds =
//     m_simpleAccessByString.FsaAccessHeader::getNbCompounds();
  
/*
  std::ifstream is(filename.c_str(), std::ios::binary );
  if( is.bad() ) {
    std::string mess = "CompoundStringAccess::readCompoundBody: Can't open file " + filename;
    LERROR << mess << LENDL;
    throw( AccessByStringNotInitialized( mess ) );
  }
  
  // positionnement du fichier sur la zone de donnees des mots composes 
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: seek to "
         << compoundsDataStreampos << LENDL;
#endif
  is.seekg(compoundsDataStreampos,std::ios::beg) ;
*/
  
  //lecture du nombre de mots composés
  uint64_t nbCompounds;
  is.read( (char *)(&(nbCompounds )), sizeof(nbCompounds ) );
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: nbCompounds="
         << nbCompounds << LENDL;
#endif
    
  // reservation memoire
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: reserve memory for m_readerCompoundsSet "
         << LENDL;
#endif
  m_readerCompoundsSet.reserve(nbCompounds);
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: resize m_compoundsDecomposition to "
         << nbCompounds << LENDL;
#endif
  m_compoundsDecomposition.resize(nbCompounds);
  
  // lecture de la composition des mots compose
//  uint64_t size = m_simpleAccessByString.getSize();
  for( uint64_t n = 0 ; n < nbCompounds ; n++ ) {
    CompoundsComposition newElement;
    is.read( (char *)(&(newElement.cmp1Id)), sizeof(newElement.cmp1Id) );
    is.read( (char *)(&(newElement.cmp2Id)), sizeof(newElement.cmp2Id) );
    is.read( (char *)(&(newElement.tId3)), sizeof(newElement.tId3) );
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: Composition= {"
         << newElement.cmp1Id << "," << newElement.cmp2Id << "," << newElement.tId3<< "}" << LENDL;
#endif
    m_readerCompoundsSet.push_back(newElement);
//    assert(newElement.tId3 - m_maxSimpleId < nbCompounds);
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::readCompoundBody: m_compoundsDecomposition["
         << newElement.tId3-MAX_SIMPLE_TID << "]="
         << n << LENDL;
#endif
    m_compoundsDecomposition[newElement.tId3-MAX_SIMPLE_TID] = n;
  }  
  
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::write( const std::string & filename  )
{
  
  std::ofstream os(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app );
  if( os.bad() ) {
    std::string mess = "CompoundStringAccess::write: Can't open file " + filename;
#ifdef DEBUG_CD
    FSAAIOLOGINIT;
    LERROR << LENDL;
#endif
    throw( FsaNotSaved( mess ) );
  }
//  os.seekp(HEADER_SIZE ,std::ios_base::beg );
  CompoundStringAccess<SimpleAccessByString>::write( os );
  
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::write( std::ostream& os ) {
  
  //m_simpleAccessByString.write( os );
  writeCompoundBody( os );
}

template <typename SimpleAccessByString>
void CompoundStringAccess<SimpleAccessByString>::writeCompoundBody(
   std::ostream& os ) {
  
  COMPSTRACCESSLOGINIT;
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::writeCompoundBody()" << LENDL;
#endif
  
  uint64_t nbCompounds = m_builderCompoundsSet.size();
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::writeCompoundBody: write nbCompounds = " << nbCompounds << LENDL;
#endif
  os.write( (const char*)(&nbCompounds), sizeof(nbCompounds) );
  
  // sauvegarde de la composition des mots compose
  BuilderCompoundsSet::const_iterator compIt = m_builderCompoundsSet.begin();
  for( ; compIt != m_builderCompoundsSet.end() ; compIt++ ) {
    uint64_t cmp1Id = (*compIt).first.first;
    uint64_t cmp2Id = (*compIt).first.second;
    uint64_t tId3 = (*compIt).second;
#ifdef DEBUG_CD
  LDEBUG <<  "CompoundStringAccess::writeCompoundBody: write cmp1Id = " << cmp1Id
         <<  ", cmp2Id = " << cmp2Id
         <<  ", tId3 = " << tId3
         << LENDL;
#endif
    os.write( (const char*)(&cmp1Id), sizeof(cmp1Id) );
    os.write( (const char*)(&cmp2Id), sizeof(cmp2Id) );
    os.write( (const char*)(&tId3), sizeof(tId3) );
  }  
}


} // namespace compactDict
} // namespace Common
} // namespace Lima
#include "CompoundStringAccess.tcc"

#endif   //FSA_COMPOUNDS_STRING_ACCESS_H
