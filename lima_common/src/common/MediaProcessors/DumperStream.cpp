// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       DumperStream.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "DumperStream.h"
#include <fstream>

using namespace std;

namespace Lima {

//***********************************************************************
// constructors and destructors
DumperStream::DumperStream(AbstractAnalysisHandler* handler):
m_useHandler(true),
m_handler(handler),
m_buffer(0),
m_out(0)
{
//  DUMPERLOGINIT;
//  LDEBUG << "DumperStream constructor: initialize buffer" ;
  // initialize handler stream buffer
  m_buffer=new HandlerStreamBuf(m_handler);
  m_out=new ostream(m_buffer);
  // start communication with handler
//  LDEBUG << "DumperStream constructor: start analysis (handler=" << m_handler << ")";
  m_handler->startAnalysis();
}

DumperStream::DumperStream(const std::string& filename, bool append):
m_useHandler(false),
m_handler(0),
m_buffer(0),
m_out(0)
{
  if (append) {
    m_out=new ofstream(filename.c_str(),ios::app | std::ofstream::binary);
  }
  else {
    m_out=new ofstream(filename.c_str(), std::ofstream::binary);
  }
}

DumperStream::~DumperStream()
{
  if (m_useHandler) {
    m_out->flush();
    m_handler->endAnalysis();
    delete m_out;
    m_out=0;
    // do not delete the handler : allocated outside, should be deleted outside
    //delete m_handler;
    //m_handler=0;
    delete m_buffer;
    m_buffer=0;
  }
  else {
    static_cast<ofstream*>(m_out)->close();
    delete m_out;
    m_out=0;
  }
}

//***********************************************************************

} // end namespace
