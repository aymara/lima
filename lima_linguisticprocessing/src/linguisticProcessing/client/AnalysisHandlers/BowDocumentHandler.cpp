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
#include "BowDocumentHandler.h"
#include "StructuredBoWToBoWDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"

using namespace std;
using namespace Lima::Common::BagOfWords;

namespace Lima {

namespace LinguisticProcessing {

BowDocumentHandler::BowDocumentHandler(): 
m_bowdocs(),
m_bowstream(0),
m_writer(0)
{
}


BowDocumentHandler::~BowDocumentHandler()
{
}

std::string BowDocumentHandler::getSBoWContent() const
{
  if (m_bowstream!=0) {
    return m_bowstream->str();
  }
  return "";
}

void BowDocumentHandler::startDocument(const Common::Misc::GenericDocumentProperties& props)
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::startDocument()";
  
  m_bowstream=new ostringstream();
  m_writer=new SBowDocumentWriter();
  m_writer->setOut(m_bowstream);
  m_writer->startDocument(props);
}

void BowDocumentHandler::endDocument()
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::endDocument()";
  m_writer->endDocument();

  // build bow documents from the XML-structured BoWDocument stream:
  // flatten structure to fit standard BoWDocuments, using a writer
  // implementing the AbstractBoWXMLWriter 
  
  istringstream in(m_bowstream->str());

  BoWBinaryReader reader;
  reader.readHeader(in);

  StructuredBoWToBoWDocument structuredBowHandler(&m_bowdocs);
  BoWDocument* document=new BoWDocument();
  while (in.good()) {
    // read Part( istream, AbstractBoWXMLWriter writer, bool useIterator)
    // do not use iterator, 
    // std::cout is unused
    reader.readBoWDocumentBlock(in, *document, structuredBowHandler, false, false);
  }

  delete m_bowstream;
  m_bowstream=0;
  delete m_writer;
  m_writer=0;
}
  
void BowDocumentHandler::startNode( const std::string& elementName, bool forIndexing )
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::startNode()";

  m_writer->startNode(elementName, forIndexing );
}

void BowDocumentHandler::endNode(const Common::Misc::GenericDocumentProperties& props)
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::endNode()";
  m_writer->endNode(props);
}

void BowDocumentHandler::startAnalysis() 
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::startAnalysis()";
  m_writer->startAnalysis();
}

void BowDocumentHandler::endAnalysis()
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::endAnalysis()";
  m_writer->endAnalysis();
}
  
void BowDocumentHandler::handle(const char* buf,int length) 
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "BowDocumentHandler::handleText()";
  m_writer->handle(buf,length);
}


}

}
