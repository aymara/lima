// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "EventHandler.h"

#include <sstream>

using namespace std;

using namespace Lima::LinguisticProcessing;
  
namespace Lima
{

namespace LinguisticProcessing
      
{
namespace EventAnalysis
{

  EventHandler::EventHandler(std::ostream* out)
  : AbstractTextualAnalysisHandler(), m_stream(0), m_writer(0), m_events()
{
  LIMA_UNUSED(out);
}

EventHandler::EventHandler()
  : AbstractTextualAnalysisHandler(), m_stream(0), m_writer(0), m_events()
{}


EventHandler::~EventHandler()
{
  if (m_writer != 0) delete m_writer;
  if (m_stream != 0) delete m_stream;
}

Events& EventHandler::getEvents()
{
  return  m_events;
}

void EventHandler::setOut( std::ostream* out ) {
  m_out = out;
 
}

void EventHandler::startDocument(const Common::Misc::GenericDocumentProperties& props)
{
  LIMA_UNUSED(props);
}

void EventHandler::endDocument()
{}

void EventHandler::endAnalysis()
{
  DUMPERLOGINIT;
  LDEBUG << "EventHandler::endAnalysis()";
  m_writer->endAnalysis();
  istringstream in(m_stream->str()); 
  LDEBUG << "EventHandler::endAnalysis(): m_stream->str= " << m_stream->str();
  m_events.read(in);
}

void EventHandler::startAnalysis()
{
  DUMPERLOGINIT;
  LDEBUG << "EventHandler::startAnalysis()";
  if (m_writer!=0) {
    LDEBUG << "EventHandler::startAnalysis(): delete m_writer";
    delete m_writer;
    m_writer=0;
  }
  if (m_stream!=0) {
    LDEBUG << "EventHandler::startAnalysis(): delete m_stream";
    delete m_stream;
    m_writer=0;
  }
  m_stream= new ostringstream();
  m_writer=new SimpleStreamHandler();
  m_writer->setOut(m_stream);
  LDEBUG << "EventHandler::startAnalysis(): m_stream->StartAnalysis()...";
  m_writer->startAnalysis();
}


void EventHandler::handle(const char* buf,int length) 
{
  DUMPERLOGINIT;
  LDEBUG << "EventHandler::handle( buff, " << length << ")";
  m_writer->handle(buf,length);
}
  
void EventHandler::startNode( const std::string& elementName, bool forIndexing )
{
  LIMA_UNUSED(elementName);
  LIMA_UNUSED(forIndexing);
}
  
void EventHandler::endNode( const Common::Misc::GenericDocumentProperties& props )
{
  LIMA_UNUSED(props);
}

} // closing namespace EventAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
