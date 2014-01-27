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
