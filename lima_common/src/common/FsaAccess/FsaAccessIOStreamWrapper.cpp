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
/*****************************************************************************
 *   Copyright (C) 2006 by  CEA                                              *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                           *
 *                                                                           *
 *  FsaAccessIOStreamBWrapper.cpp  -  description                            *
 *  Utilities for IO on file, stream, buffer of bytes or  blob in SGBDR      *
 *****************************************************************************/

#include "common/FsaAccess/FsaAccessIOStreamWrapper.h"
#include <string.h>


namespace Lima {
namespace Common {
namespace FsaAccess {

const uint64_t AbstractFsaAccessIStreamWrapper::BUFFER_SIZE = 1024*32;
const uint64_t AbstractFsaAccessOStreamWrapper::BUFFER_SIZE = 1024*32;

FsaAccessIStreamWrapper::FsaAccessIStreamWrapper(std::istream &is)
  : m_is(is)
//  , m_pos(0), m_size(AbstractFsaAccessIStreamWrapper::BUFFER_SIZE)
//   , m_buffer( new char[AbstractFsaAccessIStreamWrapper::BUFFER_SIZE] )
{
//    m_is.read( (char *)m_buffer, m_size );
//    m_size = m_is.gcount();
}

FsaAccessIStreamWrapper::~FsaAccessIStreamWrapper()
{
//  delete [] m_buffer;
}

void FsaAccessIStreamWrapper::readData( char *ptr, size_t n ) {
//  for( ; m_pos + n > m_size ; ) {
//    std::cerr << "FsaAccessOStreamWrapper::readData: memcpy (1) " << n << " bytes to pos " << m_pos << std::endl;
//    memcpy( (void *)ptr, m_buffer + m_pos, m_size - m_pos );
//    ptr += (m_size - m_pos);
//    n -= (m_size - m_pos);
//    m_is.read( (char *)m_buffer, m_size );
//    m_pos = 0;
//    m_size = m_is.gcount();
//    std::cerr << "FsaAccessOStreamWrapper::readData read " << m_size << " bytes from stream " << std::endl;
//  }
//  std::cerr << "FsaAccessOStreamWrapper::readData memcpy (2)" << n << " bytes to pos " << m_pos << std::endl;
//  memcpy( ptr, m_buffer + m_pos, n );
//  m_pos += n;
    m_is.read( ptr, n );
}

FsaAccessOStreamWrapper::FsaAccessOStreamWrapper(std::ostream &os)
  : m_os(os), m_pos(0), m_size(AbstractFsaAccessOStreamWrapper::BUFFER_SIZE),
   m_buffer( new char[AbstractFsaAccessOStreamWrapper::BUFFER_SIZE] )
{
}

FsaAccessOStreamWrapper::~FsaAccessOStreamWrapper()
{
  m_os.write( m_buffer, m_pos );
  delete [] m_buffer;
}

void FsaAccessOStreamWrapper::writeData(const char *ptr, size_t n ) {
  for( ; m_pos + n > m_size ; ) {
    memcpy( m_buffer + m_pos, (const char *)ptr, m_size - m_pos );
    ptr += (m_size - m_pos);
    n -= (m_size - m_pos);
    m_os.write( m_buffer, m_size );
    m_pos = 0;
  }
  memcpy( m_buffer + m_pos, ptr, n );
  m_pos += n;
}
  
FsaAccessInputHandlerWrapper::FsaAccessInputHandlerWrapper(FsaAccessDataInputHandler &ih)
  : m_ih(ih), m_pos(0), m_size(AbstractFsaAccessIStreamWrapper::BUFFER_SIZE),
   m_buffer( new char[AbstractFsaAccessIStreamWrapper::BUFFER_SIZE] )
{
}

FsaAccessInputHandlerWrapper::~FsaAccessInputHandlerWrapper()
{
  delete [] m_buffer;
}

void FsaAccessInputHandlerWrapper::readData( char *ptr, size_t n ) {
  for( ; m_pos + n > m_size ; ) {
    memcpy( ptr, m_buffer + m_pos, m_size - m_pos );
    m_ih.restoreData( m_buffer, m_size );
    ptr += (m_size - m_pos);
    n -= (m_size - m_pos);
    m_pos = 0;
  }
  memcpy( ptr, m_buffer + m_pos, n );
}

FsaAccessOutputHandlerWrapper::FsaAccessOutputHandlerWrapper(FsaAccessDataOutputHandler& oh)
  : m_oh(oh), m_pos(0), m_size(AbstractFsaAccessOStreamWrapper::BUFFER_SIZE),
   m_buffer( new char[AbstractFsaAccessOStreamWrapper::BUFFER_SIZE] )
{
}

FsaAccessOutputHandlerWrapper::~FsaAccessOutputHandlerWrapper()
{
  m_oh.saveData( m_buffer, m_pos );
  delete [] m_buffer;
}

void FsaAccessOutputHandlerWrapper::writeData(const char *ptr, size_t n ) {
  for( ; m_pos + n > m_size ; ) {
    memcpy( m_buffer + m_pos, ptr, m_size - m_pos );
    m_oh.saveData( m_buffer, m_size );
    ptr += (m_size - m_pos);
    n -= (m_size - m_pos);
    m_pos = 0;
  }
  memcpy( m_buffer + m_pos, ptr, n );
  m_pos += n;
}


} // namespace FsaAccess
} // namespace Common
} // namespace Lima
