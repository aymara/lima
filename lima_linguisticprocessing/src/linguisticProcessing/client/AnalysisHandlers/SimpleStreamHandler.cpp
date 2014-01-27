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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "SimpleStreamHandler.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima
{

namespace LinguisticProcessing
{
  SimpleFactory<AbstractResource,SimpleStreamHandler> simpleStreamHandlerFactory(SIMPLESTREAMHANDLER_CLASSID);

class SimpleStreamHandlerPrivate
{
  friend class SimpleStreamHandler;
  
  SimpleStreamHandlerPrivate();

  SimpleStreamHandlerPrivate(const SimpleStreamHandlerPrivate& sshp);

  virtual ~SimpleStreamHandlerPrivate();

  std::ostream* m_out;
};

SimpleStreamHandlerPrivate::SimpleStreamHandlerPrivate():
    m_out(0)
{}

SimpleStreamHandlerPrivate::SimpleStreamHandlerPrivate(const SimpleStreamHandlerPrivate& sshp):
    m_out(0)
{
  LIMA_UNUSED(sshp);
}


SimpleStreamHandlerPrivate::~SimpleStreamHandlerPrivate()
{}


SimpleStreamHandler::SimpleStreamHandler()
    : AbstractTextualAnalysisHandler(), m_d(new SimpleStreamHandlerPrivate())
{}

SimpleStreamHandler::SimpleStreamHandler(const SimpleStreamHandler& ssh)
    : AbstractTextualAnalysisHandler(), m_d(new SimpleStreamHandlerPrivate(*ssh.m_d))
{}


SimpleStreamHandler::~SimpleStreamHandler()
{
  delete m_d;
}

void SimpleStreamHandler::startDocument(const Common::Misc::GenericDocumentProperties& /*props*/)
{}

void SimpleStreamHandler::endDocument()
{}

void SimpleStreamHandler::endAnalysis()
{}

void SimpleStreamHandler::startAnalysis()
{}


void SimpleStreamHandler::handle(const char* buf,int length) 
{
  if (m_d->m_out != 0) m_d->m_out->write(buf,length);
}
  
void SimpleStreamHandler::startNode( const std::string& /*elementName*/, bool /*forIndexing*/ )
{}
  
void SimpleStreamHandler::endNode( const Common::Misc::GenericDocumentProperties& /*props*/ )
{}

void SimpleStreamHandler::setOut(std::ostream* out)
{
  m_d->m_out = out;
}

// void SimpleStreamHandler::handleProc(        
//           const std::string& tagName,
//           const std::string& content,
//           const std::map<std::string,std::string>& metaData,
//           const std::string& pipeline,
//           const std::set<std::string>& inactiveUnits)
// {
//   getAnalysisClient("fre")->analyze(content, metaData,pipeline,inactiveUnits);
//   
// }

} // closing namespace LinguisticProcessing

} // closing namespace Lima
