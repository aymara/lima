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
/************************************************************************
 *
 * @file       lexiconIdGenerator.tcc
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Thur March  23 2007
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

// #include "lexiconIdGenerator.h"
#include "common/Data/strwstrtools.h"
#include <assert.h>
#include <cstring>
namespace Lima {
namespace Common {
namespace FsaAccess {

template<typename SimpleStringAccess> 
LexiconIdGenerator<SimpleStringAccess>::LexiconIdGenerator(AbstractLexiconIdGeneratorInformer* informer) 
: AbstractLexiconIdGenerator(informer)
{}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getSize() const {
  uint64_t result = m_simpleWords.getSize();
  result += m_depCompoundLinks.size();
  return result;
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getSimpleTermSize() const {
  uint64_t result = m_simpleWords.getSize();
  return result;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::read(FsaAccessDataInputHandler& inputHandler) {
  // lecture des mots simples
  m_simpleWords.read(inputHandler);
  // lecture des mots composes
  initTransfert();
  char* buff = new char[AbstractLexiconIdAccessor::BUFFER_SIZE];
  uint64_t size;
  do {
    inputHandler.restoreData((char*)&size,sizeof(size));
    inputHandler.restoreData( buff, size );
    feedWithNextDepData(buff, size);
  } while( size > 0 ); 
  do {
    inputHandler.restoreData((char*)&size,sizeof(size));
    inputHandler.restoreData( buff, size );
    feedWithNextExtData(buff, size);
  } while( size > 0 );
  
  closeTransfert();
  delete buff;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::feedWithNextDepData(
  const char* buff, uint64_t buffSize ) {
  uint64_t  size = 0;
  for( ; ; ) {
    if( (size + sizeof(DepCompoundLink) > AbstractLexiconIdAccessor::BUFFER_SIZE)
     || (buffSize <= size ) )
      break;
    uint64_t head;
    memcpy( (char*)&head, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    uint64_t ext;
    memcpy( (char*)&ext, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    uint64_t cmpId;
    memcpy( (char*)&cmpId, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    DepCompound dep(head, ext);
#ifdef DEBUG_CD
    FSAAIOLOGINIT;
    std::ostringstream ostr;
    ostr << "LexiconIdGenerator::feedWithNextDepData: dep=" << dep
         << ", cmpId=" << cmpId;
    LDEBUG << ostr.str().c_str();
#endif
    DepCompoundLinksM::const_iterator pos = m_depCompoundLinks.find(dep);
    if( pos == m_depCompoundLinks.end() ) {
#ifdef DEBUG_CD
      std::ostringstream ostr;
      ostr << "LexiconIdGenerator::feedWithNextDepData: insert (dep=" << dep
           << ", cmpId=" << cmpId << ")";
      LDEBUG << ostr.str().c_str();
#endif
      std::pair<DepCompoundLinksM::iterator,bool> ret = 
        m_depCompoundLinks.insert( std::make_pair(dep,cmpId) );
      if( !ret.second ) {
        FSAAIOLOGINIT;
        std::ostringstream ostr;
        ostr << "!Error in LexiconIdGenerator::feedWithNextDepData: "
               << "dep.insert(" << dep
               << "," << cmpId << ") = false";
        LERROR << ostr.str().c_str();
        throw(AccessByStringNotInitialized(ostr.str()));
      }
    }
    else {
      FSAAIOLOGINIT;
      std::ostringstream ostr;
      ostr << "!Error in LexiconIdGenerator::feedWithNextDepData: "
             << "dep.find(" << dep << ") != end()";
      LERROR << ostr.str().c_str();
      throw(AccessByStringNotInitialized(ostr.str()));
    }
  }
  return;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::feedWithNextExtData(
  const char* buff, 
  uint64_t buffSize) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "LexiconIdAccessor::feedWithNextExtData("
         << buffSize << ")";
#endif
  uint64_t  size = 0;
  for( ; ; ) {
    if( (size + sizeof(DepCompoundLink) > AbstractLexiconIdAccessor::BUFFER_SIZE)
     || (buffSize <= size ) )
      break;
    uint64_t id1;
    memcpy( (char*)&id1, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    uint64_t id2;
    memcpy( (char*)&id2, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    uint64_t cmpId;
    memcpy( (char*)&cmpId, buff+size, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    ExtCompound ext(id1, id2);
#ifdef DEBUG_CD
    std::ostringstream ostr;
    ostr << "LexiconIdGenerator::feedWithNextExtData: push (ext=" << ext
         << ", cmpId=" << cmpId << ")";
    LDEBUG << ostr.str().c_str();
#endif

    ExtCompoundLinksM::const_iterator pos = m_extCompoundLinks.find(ext);
    if( pos == m_extCompoundLinks.end() ) {
#ifdef DEBUG_CD
      std::ostringstream ostr;
      ostr << "LexiconIdGenerator::feedWithNextExtData: insert (ext=" << ext
           << ", cmpId=" << cmpId << ")";
      LDEBUG << ostr.str().c_str();
#endif
      std::pair<ExtCompoundLinksM::iterator,bool> ret = 
        m_extCompoundLinks.insert( std::make_pair(ext,cmpId) );
      if( !ret.second ) {
        FSAALOGINIT;
        std::ostringstream ostr;
        ostr << "!Error in LexiconIdGenerator::feedWithNextExtData: "
               << "ext.insert(" << ext
               << "," << cmpId << ") = false";
        LERROR << ostr.str().c_str();
        throw(AccessByStringNotInitialized(ostr.str()));
      }
    }
    else {
      FSAAIOLOGINIT;
      std::ostringstream ostr;
      ostr << "!Error in LexiconIdGenerator::feedWithNextExtData: "
             << "dep.find(" << ext << ") != end()";
             LERROR << ostr.str().c_str();
      throw(AccessByStringNotInitialized(ostr.str()));
    }
  }
  return;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::print(std::ostream& os) const {
  
  m_simpleWords.print(os);
  
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "LexiconIdGenerator::print: m_depCompoundLinks.size= " << m_depCompoundLinks.size();
#endif
  os << "m_depCompoundLinks = {" << std::endl;
  DepCompoundLinksM::const_iterator links = m_depCompoundLinks.begin();
  if( links != m_depCompoundLinks.end() ) {
    const DepCompound& link = (*links).first;
    os << "(" << link.getHead() << " <- " << link.getExt() << " : " << (*links).second << ")";
    links++;
  }
  for( ; links != m_depCompoundLinks.end() ; links++ ) {
    const DepCompound& link = (*links).first;
    os << ", (" << link.getHead() << " <- " << link.getExt() << " : " << (*links).second << ")";
  }
  os << " }" << std::endl;
  
#ifdef DEBUG_CD
  LDEBUG <<  "LexiconIdGenerator::print: m_extCompoundLinks.size= " << m_extCompoundLinks.size();
#endif
  os << "m_extCompoundLinks = {" << std::endl;
  ExtCompoundLinksM::const_iterator exts = m_extCompoundLinks.begin();
  if( exts != m_extCompoundLinks.end() ) {
    const ExtCompound& conj = (*exts).first;
    os << "(" << conj.getFirst() << " + " << conj.getNext() << " : " << (*exts).second << ")";
    exts++;
  }
  for( ; exts != m_extCompoundLinks.end() ; exts++ ) {
    const ExtCompound& conj = (*exts).first;
    os << ", (" << conj.getFirst() << " + " << conj.getNext() << " : " << (*exts).second << ")";
  }
  os << " }" << std::endl;
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getId(const LimaString & word ) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "LexiconIdGenerator::getId("
         << word << ") const";
#endif
 return m_simpleWords.getIndex(word);
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getId(const LimaString &word )  {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "LexiconIdGenerator::getId("
         << word << ") const";
#endif
 return  m_simpleWords.getIndex(word);
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getId(
  const std::vector<uint64_t>& structure) const {
  assert(structure.size()>1);
  std::vector<uint64_t>::const_reverse_iterator depIt = structure.rbegin();
  uint64_t headId = structure.front();
  uint64_t extId = *depIt++;
  for( ;  ; depIt++ ) {
    uint64_t ext1Id = *depIt;
    if( ext1Id == headId)
      break;
    ExtCompoundLinksM::const_iterator pos = m_extCompoundLinks.find(ExtCompound(ext1Id,extId));
    if( pos == m_extCompoundLinks.end() ) {
      FSAALOGINIT;
      LWARN << "LexiconIdGenerator::getId(vector<...>) const: "
             << "ext.find(" << ext1Id << "," << extId << ") = ext.end()";
      return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
    }
    extId = (*pos).second;
  }

  DepCompoundLinksM::const_iterator pos = m_depCompoundLinks.find(DepCompound(headId,extId));
  if( pos == m_depCompoundLinks.end() ) {
    FSAALOGINIT;
    LWARN << "LexiconIdGenerator::getId(vector<...>) const: "
          << "dep.find(" << headId << "," << extId << ") = dep.end()";
    return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
  }

  return extId = (*pos).second;
}

template<typename SimpleStringAccess>
uint64_t LexiconIdGenerator<SimpleStringAccess>::getId(
   const std::vector<uint64_t>& structure ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "LexiconIdGenerator::getId(std::vector<...>)";
#endif

  assert(structure.size()>1);
  std::vector<uint64_t>::const_reverse_iterator depIt = structure.rbegin();
  uint64_t headId = structure.front();
  uint64_t extId = *depIt++;
//  for( ; depIt != structure.rend() ; depIt++ ) {
  for( ; ; depIt++ ) {
    uint64_t ext1Id = *depIt;
    if( ext1Id == headId)
      break;
    ExtCompound ext(ext1Id,extId);
    ExtCompoundLinksM::const_iterator pos = m_extCompoundLinks.find(ext);
    if( pos == m_extCompoundLinks.end() ) {
#ifdef DEBUG_CD
      std::ostringstream ostr;
      ostr << "LexiconIdGenerator::getId: insert ext (" << ext.getFirst()
           << "," << ext.getNext() << "), "
           <<  MAX_DEP_ID+m_extCompoundLinks.size()+1 << ")";
           LDEBUG << ostr.str().c_str();
#endif
      std::pair<ExtCompoundLinksM::iterator,bool> ret = 
        m_extCompoundLinks.insert( std::make_pair(ext,MAX_DEP_ID+m_extCompoundLinks.size()+1) );
      if( !ret.second ) {
        FSAALOGINIT;
        LERROR << "LexiconIdGenerator::getId(vector<...>): "
               << "ext.insert(" << ext1Id << "," << extId << ") = false";
        return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
      }
      else {
        m_informer->operator()(ext.getFirst(), ext.getNext(),
         AbstractLexiconIdGeneratorInformer::LINK_TYPE_EXT, MAX_DEP_ID+m_extCompoundLinks.size() );
        extId = (*(ret.first)).second;
      }
    }
    else {
      extId = (*pos).second;
    }
  }

  DepCompound dep(headId,extId);
  DepCompoundLinksM::const_iterator pos = m_depCompoundLinks.find(dep);
  if( pos == m_depCompoundLinks.end() ) {
#ifdef DEBUG_CD
    std::ostringstream ostr;
    ostr << "LexiconIdGenerator::getId: insert dep (" << dep.getHead()
         << "," << dep.getExt() << "), "
         <<  MAX_TOKEN_ID+m_depCompoundLinks.size()+1 << ")";
         LDEBUG << ostr.str().c_str();
#endif
    std::pair<DepCompoundLinksM::iterator,bool> ret = 
      m_depCompoundLinks.insert( std::make_pair(dep,MAX_TOKEN_ID+m_depCompoundLinks.size()+1) );
    // store created link in new_compLinks to increment table COMPOUND_WORD
    if( !ret.second ) {
      FSAALOGINIT;
      LERROR << "LexiconIdGenerator::getId(vector<...>): "
             << "dep.insert(" << headId << "," << extId << ") = false";
      return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
    }
    else {
      assert(m_informer != 0);
      m_informer->operator()(dep.getHead(), dep.getExt(),
        AbstractLexiconIdGeneratorInformer::LINK_TYPE_DEP, MAX_TOKEN_ID+m_depCompoundLinks.size() );
      return ((*ret.first)).second;
    }
  }
  return (*pos).second;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::write(FsaAccessDataOutputHandler& outputHandler) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG << "LexiconIdGenerator::write(FsaAccessDataOutputHandler)";
#endif
  // enregistrement des mots simples
  m_simpleWords.write(outputHandler);
  // enregistrement des mots compos�
//  const char* buff = initTransfert();
  initTransfert();
  char* buff = new char[AbstractLexiconIdAccessor::BUFFER_SIZE];

  uint64_t size;
  do {
    size = getNextDepData( buff );
#ifdef DEBUG_CD
    LDEBUG << "LexiconIdGenerator::write: outputHandler.saveData(depData..," 
           << size << ")";
#endif
    outputHandler.saveData((const char*)&size,sizeof(size));
    outputHandler.saveData(buff,size);
  } while( size > 0 ); 
  do {
    size = getNextExtData( buff );
#ifdef DEBUG_CD
    LDEBUG << "LexiconIdGenerator::write: outputHandler.saveData(dextData..," 
           << size << ")";
#endif
    outputHandler.saveData((const char*)&size,sizeof(size));
    outputHandler.saveData(buff,size);
  } while( size > 0 );
  closeTransfert();
  delete buff;
  
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::initTransfert() {
  m_depIt = m_depCompoundLinks.begin();
  m_extIt = m_extCompoundLinks.begin();
//  m_buffer = new char[AbstractLexiconIdAccessor::BUFFER_SIZE];
//  return m_buffer;
}

template<typename SimpleStringAccess> 
void LexiconIdGenerator<SimpleStringAccess>::closeTransfert() {
//  delete m_buffer;
//  m_buffer = 0;
  m_depIt = m_depCompoundLinks.end();
  m_extIt = m_extCompoundLinks.end();
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getNextDepData(char* buff ) {
  int size = 0;
  for( ; m_depIt != m_depCompoundLinks.end() ; ) {
    const DepCompound cmp = (*m_depIt).first;
    const uint64_t cmpId = (*m_depIt).second;
    
    uint64_t head = cmp.getHead();
    memcpy( buff+size, (uint8_t*)&head, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    
    uint64_t ext = cmp.getExt();
    memcpy( buff+size, (uint8_t*)&ext, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    memcpy( buff+size, (uint8_t*)&cmpId, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    m_depIt++;
#ifdef DEBUG_CD
    FSAAIOLOGINIT;
    LDEBUG << "LexiconIdGenerator::getNextDepData: memcpy buffer+"
           << size << " <- " << head << "," << ext << "," << cmpId;
#endif
    if( size + sizeof(DepCompoundLink) > AbstractLexiconIdAccessor::BUFFER_SIZE )
      return size;
  }
  return size;
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdGenerator<SimpleStringAccess>::getNextExtData(char* buff ) {
  int size = 0;
  for( ; m_extIt != m_extCompoundLinks.end() ; ) {

    const ExtCompound ext = (*m_extIt).first;
    const uint64_t extId = (*m_extIt).second;

    uint64_t id1 = ext.getFirst();
    memcpy( buff+size, (uint8_t*)&id1, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    uint64_t id2 = ext.getNext();
    memcpy( buff+size, (uint8_t*)&id2, sizeof(uint64_t) );
    size += sizeof(uint64_t);

    memcpy( buff+size, (uint8_t*)&extId, sizeof(uint64_t) );
    size += sizeof(uint64_t);
    m_extIt++;
#ifdef DEBUG_CD
    FSAAIOLOGINIT;
    LDEBUG << "LexiconIdGenerator::getNextExtData: memcpy buffer+"
           << size << " <- " << id1 << "," << id2 << "," << extId;
#endif
    if( size + sizeof(ExtCompoundLink) > AbstractLexiconIdAccessor::BUFFER_SIZE )
      return size;
  }
  return size;
}


} // end namespace
} // end namespace
} // end namespace
