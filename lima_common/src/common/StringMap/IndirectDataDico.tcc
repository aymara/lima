// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/

// For ::stat() function
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif


/*
#include "linguisticProcessing/core/Dictionary/DictionaryEntry.h"
#include "linguisticProcessing/core/Dictionary/DictionaryCode.h"
*/

namespace Lima {
namespace Common {
namespace StringMap {

template <typename accessMethod, typename contentElement>
IndirectDataDico<accessMethod, contentElement>::IndirectDataDico( const contentElement& defaultValue )
    : StringMap<accessMethod, contentElement>( defaultValue ) , m_index2Data(0){
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "IndirectDataDico::IndirectDataDico()";
#endif
}

template <typename accessMethod, typename contentElement>
IndirectDataDico<accessMethod, contentElement>::~IndirectDataDico() {
  if( m_data != 0 )
    delete [] m_data;
  if( m_index2Data != 0 )
    delete [] m_index2Data;
}


template <typename accessMethod, typename contentElement>
void IndirectDataDico<accessMethod, contentElement>::parseData( const std::string& dataFileName )
{
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG << "IndirectDataDico::parseData(" << dataFileName << ")";
#endif

  // create datas memory storage
  struct stat sts;
  uint64_t dataSize = 0;
//  if( stat( dataFileName.c_str(), &sts) != 0)
  stat( dataFileName.c_str(), &sts);
    dataSize += sts.st_size;
  std::cerr << "IndirectDataDico::parseData: sts.st_size = " << sts.st_size << std::endl;
  m_data = new uint8_t [dataSize];
#ifdef DEBUG_CD
  LDEBUG << "IndirectDataDico::parseData: allocate " << dataSize << " bytes";
#endif
  if (m_data == NULL)
  {
    std::string mess = "IndirectDataDico::parseData: memory allocation error";
#ifdef DEBUG_CD
    LERROR << mess;
#endif
    throw( std::logic_error( mess ) );
  }

  // load data
  FILE *dataFile = fopen(dataFileName.c_str(), "rb");
  uint32_t totalDataReadSize = 0;
  uint32_t readSize = 0;
  if (dataFile == NULL)
  {
    LIMA_EXCEPTION_LOGINIT(
      STRINGMAPLOGINIT,
      "IndirectDataDico::parseData error cannot open data file "
      << dataFileName.c_str());
  }

  //    fseek(dataFile, DATA_HEADER_SIZE, SEEK_SET);            // skip header
#ifdef DEBUG_CD
  LDEBUG <<  "IndirectDataDico::parseData: fread(" << totalDataReadSize
         << ", " << dataSize-totalDataReadSize << ")";
#endif
  readSize = fread(m_data+totalDataReadSize, 1, dataSize-totalDataReadSize, dataFile);        //_dataSize = max
#ifdef DEBUG_CD
  LDEBUG <<  "IndirectDataDico::parseData: read " << readSize
         << " bytes from " << dataFileName;
#endif
  totalDataReadSize += readSize;
  fclose(dataFile);
  if (totalDataReadSize != dataSize)
  {
    std::string mess = "IndirectDataDico::parseData: totalDataReadSize != _dataSize ";
#ifdef DEBUG_CD
    LERROR << mess;
#endif
    throw( std::logic_error( mess ) );
  }
  fillIndex2Data();
}

template <typename accessMethod, typename contentElement>
void IndirectDataDico<accessMethod, contentElement>::fillIndex2Data() {
  // fill index2Data
  uint64_t keyCount = this->getSize();
  m_index2Data = new uint64_t[keyCount];
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "IndirectDataDico::parseData: fill index_2data... ";
#endif
  size_t ptrOffset = 0;
  uint8_t *datasAddr = m_data;
  for( uint32_t entry=0 ; entry < keyCount ; entry++ )
  {
#ifdef DEBUG_CD
//    LDEBUG <<  "IndirectDataDico::parseData: offset m_index2Data[" << entry << "]=" << ptrOffset;
#endif
    ptrOffset = datasAddr - m_data;
    m_index2Data[entry] = ptrOffset;
    datasAddr += 2; // ???
//    binaryEntry.nextField(datasAddr);
  }
#ifdef DEBUG_CD
  LDEBUG <<  "IndirectDataDico::parseData: end fill index_2data";
#endif
}


} // namespace StringMap
} // namespace Commmon
} // namespace Lima
