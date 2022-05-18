// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#include "HandlerStreamBuf.h"

#define BUFFER_SIZE 4096

using namespace std;

namespace Lima{


HandlerStreamBuf::HandlerStreamBuf(AbstractAnalysisHandler* handler) :
    m_handler(handler)
{
  m_pbuf = new char[BUFFER_SIZE];
  setp(m_pbuf,m_pbuf+BUFFER_SIZE);
}


HandlerStreamBuf::~HandlerStreamBuf()
{
  sync();
  delete [] m_pbuf;
}

int HandlerStreamBuf::overflow(int c)
{
  if (sync()) return EOF;
  if (c != EOF)
  {
    char cbuf[1];
    cbuf[0] = c;
    m_handler->handle(cbuf,1);
  }
  return 0;
}

int HandlerStreamBuf::sync()
{
  if (pbase()!=pptr())
  {
    flushBuffer();
  }
  return 0;
}

void HandlerStreamBuf::flushBuffer()
{
  streamsize len = (pptr() - pbase());
  m_handler->handle(pbase(),len);
  setp(pbase(), epptr());
}


}
