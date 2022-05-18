// Copyright (C) 2003 by CEA LIST (LVIC)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "tokensstream.h"


namespace Lima {
namespace DocumentsReader {

//**********************************************************************
// constructors,destructor,copy assignment
//**********************************************************************
TokensStream::TokensStream():
std::deque< IndexToken* >()
{
}


TokensStream::TokensStream(const TokensStream& t):
std::deque< IndexToken* >() 
{
  copy(t);
}

TokensStream& TokensStream::operator = (const TokensStream& t)
{
    if (this != &t)
    {
        clear();
        copy(t);
    }
    return *this;
}

TokensStream::~TokensStream()
{
    clear();
}

//********************
// helper functions
void TokensStream::copy(const TokensStream& t) {
  for (TokensStream::const_iterator i(t.begin());
       i != t.end(); i++) {
    push_back((*i)->clone());
  }
}
void TokensStream::clear() {
  for (TokensStream::const_iterator i(begin());
       i != end(); i++) {
    delete (*i);
  }
  std::deque< IndexToken* >::clear();
}

//**********************************************************************

void TokensStream::push(const IndexToken& t) {
  push_back(t.clone());
}

/**
  * Tests if a token is available
  */
bool TokensStream::hasToken() const
{
  return (!empty());
}

/**
  * Returns the next token. It will be no more available
  */
IndexToken* TokensStream::nextToken()
{
  if (empty())
  {
    throw std::runtime_error("nextToken in an empty tokens stream.");
  }
  IndexToken* res = front();
  pop_front();
  return res;
}

//**********************************************************************
// input/output functions
/*
std::wostream& operator << (std::wostream& os, TokensStream& stream) {
  for (TokensStream::const_iterator i(stream.begin()); 
       i != stream.end(); i++) {
    os << *i << std::endl;
  }
  os << CODE_ENDOFSTREAM;
  return os;
}

std::wistream& operator >> (std::wistream& is, TokensStream& stream) {
  bool endOfStream(false);
  while (! endOfStream) {
    IndexToken* token=readFromStream(is);
    if (token == NULL) {
      endOfStream=true;
    }
    else { 
      stream.push_back(token);
    }
  }
  return is;
}
*/

} // namespace DocumentsReader
} // namespace Lima
