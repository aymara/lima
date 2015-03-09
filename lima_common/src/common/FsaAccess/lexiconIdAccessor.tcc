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
 * @file       lexiconIdAccessor.tcc
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Thur March  23 2007
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include <algorithm>
#include <cstring>
 
// #include "lexiconIdAccessor.h"
#include "common/Data/strwstrtools.h"
#include <assert.h>
namespace Lima {
namespace Common {
namespace FsaAccess {


template<typename SimpleStringAccess> 
uint64_t LexiconIdAccessor<SimpleStringAccess>::getSize() const {
  uint64_t result = m_simpleWords.getSize();
  result += m_depCompoundLinks.size();
  return result;
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdAccessor<SimpleStringAccess>::getSimpleTermSize() const {
  uint64_t result = m_simpleWords.getSize();
  return result;
}

template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::print(std::ostream& os) const {
  
  m_simpleWords.print(os);
  
  os << "m_depCompoundLinks = {" << std::endl;
  DepCompoundLinksV::const_iterator links = m_depCompoundLinks.begin();
  if( links != m_depCompoundLinks.end() ) {
    const DepCompoundLink& link = *links;
    os << "(" << link.getDepCompound().getHead() << " <- " << link.getDepCompound().getExt() << " : " << link.getId() << ")";
  }
  links++;
  for( ; links != m_depCompoundLinks.end() ; links++ ) {
    const DepCompoundLink& link = *links;
    os << ", (" << link.getDepCompound().getHead() << " <- " << link.getDepCompound().getExt() << " : " << link.getId() << ")";
  }
  os << " }" << std::endl;
  
  os << "m_extCompoundLinks = {" << std::endl;
  ExtCompoundLinksV::const_iterator exts = m_extCompoundLinks.begin();
  if( exts != m_extCompoundLinks.end() ) {
    const ExtCompoundLink& ext = *exts;
    os << "(" << ext.getExtCompound().getFirst() << " + " << ext.getExtCompound().getNext() << " : " << (*exts).second << ")";
  }
  exts++;
  for( ; exts != m_extCompoundLinks.end() ; exts++ ) {
    const ExtCompoundLink& ext = *exts;
    os << ", (" << ext.getExtCompound().getFirst() << " + " << ext.getExtCompound().getNext() << " : " << (*exts).second << ")";
  }
  os << " }" << std::endl;
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdAccessor<SimpleStringAccess>::getId(const LimaString & word ) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE << "LexiconIdAccessor::getId("
         << word << ")";
#endif
 return m_simpleWords.getIndex(word);
}

template<typename SimpleStringAccess> 
uint64_t LexiconIdAccessor<SimpleStringAccess>::getId(
  const std::vector<uint64_t>& structure) const {
  assert(structure.size()>1);
#ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "LexiconIdAccessor::getId(std::vector<...>)" << LENDL;
  LTRACE <<  "LexiconIdAccessor::getId m_extCompoundLinks.size()="
         << m_extCompoundLinks.size() << LENDL;
  LTRACE <<  "LexiconIdAccessor::getId m_depCompoundLinks.size()="
         << m_depCompoundLinks.size() << LENDL;
#endif

  std::vector<uint64_t>::const_reverse_iterator depIt = structure.rbegin();
  uint64_t headId = structure.front();
  uint64_t extId = *depIt++;
  for( ; ; depIt++ ) {
    uint64_t ext1Id = *depIt;
    if( ext1Id == headId)
      break;

    ExtCompound ext(ext1Id,extId);
#ifdef DEBUG_CD
    std::ostringstream ostr;
    ostr << "LexiconIdAccessor::getId: ext=" << ext;
    LTRACE << ostr.str().c_str() << LENDL;
#endif
    ExtCompoundLink extlink(ext, 0);
    ExtCompoundLinkLtOp extLinkLtOp;
    ExtCompoundLinksV::const_iterator pos = lower_bound(
      m_extCompoundLinks.begin(), m_extCompoundLinks.end(), extlink, extLinkLtOp );
    if( ( pos ==  m_extCompoundLinks.end() ) ||
        ( (*pos).getExtCompound() != ext ) ) {
      return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
    }
    extId = (*pos).getId();
  }

  DepCompound conj(headId,extId);
  DepCompoundLink depLink(conj, 0);
  DepCompoundLinkLtOp depLinkLtOp;
  DepCompoundLinksV::const_iterator pos = lower_bound(
    m_depCompoundLinks.begin(), m_depCompoundLinks.end(), depLink, depLinkLtOp );
#ifdef DEBUG_CD
  std::ostringstream ostr;
  ostr << "LexiconIdAccessor::getId: conj=" << conj;
  LTRACE << ostr.str().c_str() << LENDL;
#endif
  if (( pos ==  m_depCompoundLinks.end() ) ||
      ( (*pos).getDepCompound() != conj )) {
    return Common::AbstractLexiconIdAccessor::NOT_A_LEXICON_ID;
  }
  return (*pos).getId();
}
    
template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::read(FsaAccessDataInputHandler& inputHandler)
{
  // lecture des mots simples
  m_simpleWords.read(inputHandler);
  // lecture des mots compos�
  initTransfert();
  char* buff = new char[AbstractLexiconIdAccessor::BUFFER_SIZE];
  uint64_t size;
  do {
    inputHandler.restoreData((char*)&size,sizeof(size));
    assert(size <= AbstractLexiconIdAccessor::BUFFER_SIZE);
    inputHandler.restoreData( buff, size );
    feedWithNextDepData(buff, size);
  } while( size > 0 ); 
  do {
    inputHandler.restoreData((char*)&size,sizeof(size));
    assert(size <= AbstractLexiconIdAccessor::BUFFER_SIZE);
    inputHandler.restoreData( buff, size );
    feedWithNextExtData(buff, size);
  } while( size > 0 );
  
  closeTransfert();
  delete[] buff;
  
}

template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::initTransfert() {
  m_depIt = m_depCompoundLinks.begin();
  m_extIt = m_extCompoundLinks.begin();
}

template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::closeTransfert() {
  m_depIt = m_depCompoundLinks.end();
  m_extIt = m_extCompoundLinks.end();
}

template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::feedWithNextDepData(
  const char* buff, uint64_t buffSize )
  {
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
  ostr << "LexiconIdAccessor::feedWithNextDepData: dep=" << dep
       << ", cmpId=" << cmpId;
  LTRACE << ostr.str().c_str() << LENDL;
#endif
    DepCompoundLink link(dep, cmpId);
    m_depCompoundLinks.push_back( link );
  }
  return;
}

template<typename SimpleStringAccess> 
void LexiconIdAccessor<SimpleStringAccess>::feedWithNextExtData(
  const char* buff, uint64_t buffSize){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "LexiconIdAccessor::feedWithNextExtData("
         << buffSize << ")" << LENDL;
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
    ostr << "LexiconIdAccessor::feedWithNextDepData: ext=" << ext
         << ", cmpId=" << cmpId;
    LTRACE << ostr.str().c_str() << LENDL;
#endif
    ExtCompoundLink link(ext, cmpId);
    m_extCompoundLinks.push_back( link );
  }
  return;
}

} // end namespace
} // end namespace
} // end namespace
