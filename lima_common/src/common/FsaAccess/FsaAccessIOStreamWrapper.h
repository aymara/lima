// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
