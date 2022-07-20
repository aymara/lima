// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
                          FsaAccessHeader.cpp  -  description
                             -------------------
    begin                : mon nov 7 2005
    copyright            : (C) 2003-2005 by CEA
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  header for files                                                       *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                        *
 ***************************************************************************/

 // pour la definitin de one_byte, two_bytes, four_bytes
#include "FsaAccessHeader.h"
#include "FsaAccessIOStreamWrapper.h"
#include "common/Data/LimaString.h"
#include <sstream>
#include <string.h>


namespace Lima {
namespace Common {
namespace FsaAccess {

class FsaAccessHeaderPrivate {
  friend class FsaAccessHeader;
  
  FsaAccessHeaderPrivate(bool trie_direction_fwd);
  virtual ~FsaAccessHeaderPrivate();

    uint16_t m_majorVersion;
    uint16_t m_minorVersion;
    uint8_t m_charType;
    uint8_t m_packing;
    uint8_t m_charOrder;
    uint64_t m_nbVertices;
    uint64_t m_nbEdges;
//    uint64_t m_maxSimpleId;
//    uint64_t m_compoundsDataOffset;
//    uint64_t m_nbCompounds;
    /**
     *   direction when reading characters of input key in getIndex
     *   the value is set when access Data (list of keys) is compiled
     *   and is stored in header of file
     *   must be const!!
     *   this is not a pamameter of getIndex() function! (depends on compiled data structure)
     *
     */
    bool m_trie_direction_fwd;
//    long m_compoundsDataPos;
};

FsaAccessHeaderPrivate::FsaAccessHeaderPrivate(bool trie_direction_fwd)
 : m_majorVersion( MAJOR_VERSION_16 ),
   m_minorVersion( MINOR_VERSION_16 ),
   m_packing(BUILDER),
   m_charOrder(FORWARD),
   m_nbVertices(0),
   m_nbEdges(0),
//   m_maxSimpleId(MAX_SIMPLE_TID),
//   m_compoundsDataOffset(0)
//   m_nbCompounds(0),
   m_trie_direction_fwd( trie_direction_fwd )
{
  if( sizeof(LimaChar) == 2 ) {
    m_charType = 2;
  }
  else {
    std::string mess = "FsaAccessBuilder::FsaAccessHeader: sizeof(LimaChar) must be 2!!!" ;
#ifdef DEBUG_CD
    FSAALOGINIT;
    LERROR << mess.c_str();
#endif
    throw( FsaNotSaved( mess ) );
  }
}

FsaAccessHeaderPrivate::~FsaAccessHeaderPrivate()
{
}

FsaAccessHeader::FsaAccessHeader(bool trie_direction_fwd) :
    m_d(new FsaAccessHeaderPrivate(trie_direction_fwd))
{
}

FsaAccessHeader::FsaAccessHeader(const FsaAccessHeader& h) :
    m_d(new FsaAccessHeaderPrivate(*h.m_d))
{
}

FsaAccessHeader::~FsaAccessHeader()
{
  delete m_d;
}

FsaAccessHeader& FsaAccessHeader::operator=(const FsaAccessHeader& h) {*m_d = *(h.m_d); return *this;}

bool FsaAccessHeader::getTrieDirectionForward() const { return m_d->m_trie_direction_fwd; }
uint64_t FsaAccessHeader::getNbVertices() const { return m_d->m_nbVertices; }
uint64_t FsaAccessHeader::getNbEdges() const { return m_d->m_nbEdges; }
void FsaAccessHeader::setPackingStatus(uint8_t packingStatus) { m_d->m_packing = packingStatus; }
void FsaAccessHeader::setNbVertices(uint64_t nbVerts) { m_d->m_nbVertices = nbVerts; }
void FsaAccessHeader::setNbEdges(uint64_t nbEdges) { m_d->m_nbEdges = nbEdges; }
//    uint64_t getMaxSimpleId() const { return m_maxSimpleId; }
//    uint64_t  int getCompoundsDataOffset() const { return m_compoundsDataOffset; }
//    uint64_t getNbCompounds() const { return m_nbCompounds; }
//    void setStreamPos(std::ifstream& is);
//    const long& getStreamPos() { return m_compoundsDataPos; }

void FsaAccessHeader::read( AbstractFsaAccessIStreamWrapper& iw ) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessHeader::read()";
#endif

  char headBuf[HEADER_SIZE+1];
  iw.readData( headBuf, HEADER_SIZE );
  char *dstptr = headBuf;
  
  char magicNumber[13];
  strncpy(magicNumber, dstptr, 12 );
  dstptr += 12;
  magicNumber[sizeof(magicNumber)-1] = '\0';
  if( strcmp(magicNumber,MAGIC_NUMBER) ) {
    std::string mess = "FsaAccessHeader::read: bad magic number '"
      +  std::string(magicNumber) + "'" ;
#ifdef DEBUG_CD
    LERROR << mess.c_str();
#endif
    throw( AccessByStringNotInitialized(mess) );
  }

  char numBuf[6];
  strncpy(numBuf, dstptr, 5 );
  dstptr += 5;
  numBuf[sizeof(numBuf)-1] = '\0';
  sscanf(numBuf, "%hu.%hu", &m_d->m_majorVersion, &m_d->m_minorVersion);
#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_majorVersion = "
         << m_d->m_majorVersion << " m_minorVersion = " << m_d->m_minorVersion
	;
#endif
  uint16_t majorVersion = MAJOR_VERSION_16;
//  uint16_t minorVersion = MINOR_VERSION_16;
  if( m_d->m_majorVersion > majorVersion) {
/*
  if( (m_majorVersion > majorVersion)
   ||((m_majorVersion == majorVersion)
   && (m_minorVersion > minorVersion) ) ) {
*/
    std::string mess = "FsaAccessHeader::read: bad version = "
                      + std::string(numBuf)
                      + ". Get a release of your library or convert your file !!" ;
#ifdef DEBUG_CD
    LERROR << mess.c_str();
#endif
    throw( AccessByStringNotInitialized(mess ) );
  }

  uint8_t cVal= *dstptr++;
  m_d->m_charType = cVal;
  if( (m_d->m_charType != 1)
   && (m_d->m_charType != 2)
   && (m_d->m_charType != 4) ) {
    std::string mess = "FsaAccessHeader::read: byte_per_char must be 1, 2 or 4!!!" ;
#ifdef DEBUG_CD
    LERROR << mess.c_str();
#endif
    throw( AccessByStringNotInitialized(mess) );
  }

#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_charType = "
         << (int)cVal;
#endif

  cVal= *dstptr++;
  m_d->m_packing = cVal;
  if( (m_d->m_packing != BUILDER)
   && (m_d->m_packing != BUILT)
   && (m_d->m_packing != SPARE)) {
    std::string mess = "FsaAccessHeader::read: m_packing type is not recognized!!!" ;
#ifdef DEBUG_CD
    LERROR << mess.c_str();
#endif
    throw( AccessByStringNotInitialized(mess) );
  }
#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_packing = "
         << (m_d->m_packing == BUILDER ? "BUILDER" : (m_d->m_packing == BUILT ? "BUILT" : "SPARE" ));
#endif

  cVal= *dstptr++;
#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_charOrder = "
         << (int)cVal;
#endif
  m_d->m_charOrder = cVal;
  if( (m_d->m_charOrder != FORWARD)
    &&(m_d->m_charOrder != REVERSE) ) {
    std::string mess = "FsaAccessHeader::read: character order is not recognized!!!" ;
#ifdef DEBUG_CD
    LERROR << mess.c_str();
#endif
    throw( AccessByStringNotInitialized(mess) );
  }
  m_d->m_trie_direction_fwd = (m_d->m_charOrder == FORWARD);
#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_charOrder = "
         << (m_d->m_charOrder == FORWARD ? "FORWARD" : "REVERSE" );
#ifdef DEBUG_CD
  LDEBUG << "FsaAccessHeader::read: m_charOrder = "
         << (int)cVal;
#endif
#endif

  char numBuf2[21];
  strncpy(numBuf2, dstptr, 20 );
  dstptr += 20;
  numBuf2[sizeof(numBuf2)-1] = '\0';
  sscanf(numBuf2, "%lu %lu ", &(m_d->m_nbVertices), &(m_d->m_nbEdges) );
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessHeader::read: nbVert = " << m_d->m_nbVertices
                                         << "m_nbEdges = " << m_d->m_nbEdges;
#endif
  
}
  

void FsaAccessHeader::write( AbstractFsaAccessOStreamWrapper& ow) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessHeader::write(";
#endif

  char headBuf[HEADER_SIZE];
  char *dstptr = headBuf;
  memset(dstptr, 0, HEADER_SIZE );
  strncpy(dstptr, MAGIC_NUMBER, 12 );
  dstptr += 12;

  sprintf(dstptr, "%02d.%02d", m_d->m_majorVersion, m_d->m_minorVersion);
  dstptr += 5;

  *dstptr++ = m_d->m_charType;
  *dstptr++ = m_d->m_packing;
  *dstptr++ = m_d->m_charOrder;

  sprintf(dstptr, "%09lu ", m_d->m_nbVertices);
  dstptr += 10;

  sprintf(dstptr, "%09lu ", m_d->m_nbEdges);
  dstptr += 10;

  ow.writeData( headBuf, HEADER_SIZE );
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessHeader::write: os.write headbuf ";
#endif

}



} // namespace FsaAccess
} // namespace Common
} // namespace Lima
