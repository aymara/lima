// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "MetaDataHandler.h"

#include <sstream>

using namespace std;

using namespace Lima::LinguisticProcessing;
  
namespace Lima
{
namespace LinguisticProcessing
     
{

MetaDataHandler::MetaDataHandler(std::ostream* /*out*/):
AbstractTextualAnalysisHandler(),
m_stream(0),
m_writer(0),
m_metadata()
{
}

MetaDataHandler::MetaDataHandler():
AbstractTextualAnalysisHandler(),
m_stream(0),
m_writer(0),
m_metadata()
{}


MetaDataHandler::~MetaDataHandler()
{
  if (m_writer != 0) delete m_writer;
  if (m_stream != 0) delete m_stream;
}

const std::map<std::string,std::string>& MetaDataHandler::getMetaData()
{
  return  m_metadata;
}

void MetaDataHandler::setOut( std::ostream* out ) {
  m_out = out;
}

void MetaDataHandler::startDocument(const Common::Misc::GenericDocumentProperties& /*props*/)
{
}

void MetaDataHandler::endDocument()
{}

void MetaDataHandler::endAnalysis()
{
  DUMPERLOGINIT;
  LDEBUG << "MetaDataHandler::endAnalysis()";
  m_writer->endAnalysis();
  istringstream in(m_stream->str()); 
  LDEBUG << "MetaDataHandler::endAnalysis(): m_stream->str= " << m_stream->str();
  // read the metadata
  uint size;
  string key(""),value("");
  getline(in,value);
  size=std::stoi(value);
  for (uint i(0);i<size;i++) {
    getline(in,key);
    getline(in,value);
    m_metadata[key]=value;
  }
  //for (const auto& m: m_metadata) {
  //  cerr << "MetaDataHandler: metadata[" << m.first << "]=" << m.second << endl;
  //}
}

void MetaDataHandler::startAnalysis()
{
  DUMPERLOGINIT;
  LDEBUG << "MetaDataHandler::startAnalysis()";
  if (m_writer!=0) {
    LDEBUG << "MetaDataHandler::startAnalysis(): delete m_writer";
    delete m_writer;
    m_writer=0;
  }
  if (m_stream!=0) {
    LDEBUG << "MetaDataHandler::startAnalysis(): delete m_stream";
    delete m_stream;
    m_writer=0;
  }
  m_stream= new ostringstream();
  m_writer=new SimpleStreamHandler();
  m_writer->setOut(m_stream);
  LDEBUG << "MetaDataHandler::startAnalysis(): m_stream->StartAnalysis()...";
  m_writer->startAnalysis();
}


void MetaDataHandler::handle(const char* buf,int length) 
{
  DUMPERLOGINIT;
  LDEBUG << "MetaDataHandler::handle( buff, " << length << ")";
  m_writer->handle(buf,length);
}
  
void MetaDataHandler::startNode( const std::string& /*elementName*/, bool /*forIndexing*/ )
{
}
  
void MetaDataHandler::endNode( const Common::Misc::GenericDocumentProperties& /*props*/ )
{
}

} // closing namespace LinguisticProcessing
} // closing namespace Lima
