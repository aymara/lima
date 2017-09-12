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
 *  lexiconCompoundElement.h  -  description                                 *
 *  common definition to implement lexiconIdGenerator and lexiconIdAccessor  *
 *****************************************************************************/
#include <iostream>

#ifndef FSA_IO_STREAM_WRAPPER_H
#define FSA_IO_STREAM_WRAPPER_H

#include "common/LimaCommon.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT AbstractFsaAccessIStreamWrapper {
public:
  virtual ~AbstractFsaAccessIStreamWrapper() {}
  virtual void readData( char *ptr, size_t n ) = 0;
  static const uint64_t BUFFER_SIZE;
};

class LIMA_FSAACCESS_EXPORT AbstractFsaAccessOStreamWrapper {
public:
  virtual ~AbstractFsaAccessOStreamWrapper() {}
  virtual void writeData(const char *ptr, size_t n ) = 0;
  static const uint64_t BUFFER_SIZE;
};

class LIMA_FSAACCESS_EXPORT FsaAccessIStreamWrapper : public AbstractFsaAccessIStreamWrapper {
public:
  FsaAccessIStreamWrapper(std::istream &is);
  ~FsaAccessIStreamWrapper();
  void readData( char *ptr, size_t n ) override ;
private:
	FsaAccessIStreamWrapper& operator=(const FsaAccessIStreamWrapper& ) {return *this;}
  std::istream &m_is;
//  uint64_t m_pos;
//  uint64_t m_size;
//  char* m_buffer;
};

class LIMA_FSAACCESS_EXPORT FsaAccessOStreamWrapper : public AbstractFsaAccessOStreamWrapper {
public:
  FsaAccessOStreamWrapper(std::ostream &os);
  ~FsaAccessOStreamWrapper();
  void writeData(const char *ptr, size_t n ) override;
private:
	FsaAccessOStreamWrapper& operator=(const FsaAccessOStreamWrapper& ) {return *this;}
  std::ostream &m_os;
  uint64_t m_pos;
  uint64_t m_size;
  char* m_buffer;
};
    
class LIMA_FSAACCESS_EXPORT FsaAccessDataInputHandler {
public: 
  virtual ~FsaAccessDataInputHandler() {}
  virtual void restoreData( char* m_buffer, uint64_t m_size ) = 0;
private:
  FsaAccessDataInputHandler& operator=(const FsaAccessDataInputHandler& ) {return *this;}
};

class LIMA_FSAACCESS_EXPORT FsaAccessInputHandlerWrapper : public AbstractFsaAccessIStreamWrapper {
public:
  FsaAccessInputHandlerWrapper(FsaAccessDataInputHandler &ih);
  ~FsaAccessInputHandlerWrapper();
  void readData( char *ptr, size_t n ) override;
private:
  FsaAccessInputHandlerWrapper& operator=(const FsaAccessInputHandlerWrapper& ) {return *this;}
  FsaAccessDataInputHandler& m_ih;
  uint64_t m_pos;
  uint64_t m_size;
  char* m_buffer;
};

class LIMA_FSAACCESS_EXPORT FsaAccessDataOutputHandler {
public: 
  virtual ~FsaAccessDataOutputHandler() {}
  virtual void saveData( const char* m_buffer, uint64_t m_size ) = 0;
private:
  FsaAccessDataOutputHandler& operator=(const FsaAccessDataOutputHandler& ) {return *this;}
};

class LIMA_FSAACCESS_EXPORT FsaAccessOutputHandlerWrapper : public AbstractFsaAccessOStreamWrapper {
public:
  FsaAccessOutputHandlerWrapper(FsaAccessDataOutputHandler &oh);
  ~FsaAccessOutputHandlerWrapper();
  void writeData(const char *ptr, size_t n ) override;
private:
  FsaAccessOutputHandlerWrapper& operator=(const FsaAccessOutputHandlerWrapper& ) {return *this;}
  FsaAccessDataOutputHandler& m_oh;
  uint64_t m_pos;
  uint64_t m_size;
  char* m_buffer;
};


} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#endif   //FSA_IO_STREAM_WRAPPER_H
