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
#include "BowTextHandler.h"

#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"

using namespace std;
using namespace Lima::Common::BagOfWords;

namespace Lima
{

namespace LinguisticProcessing
{

BowTextHandler::BowTextHandler()
    : AbstractTextualAnalysisHandler(),m_bowstream(0),m_writer(0),m_bowtext()
{
}


BowTextHandler::~BowTextHandler()
{
  if (m_writer!=0) {
    delete m_writer;
  }
  if (m_bowstream!=0) {
    delete m_bowstream;
  }
}

Common::BagOfWords::BoWText& BowTextHandler::getBowText()
{
  return m_bowtext;
}


void BowTextHandler::endAnalysis()
{
  m_writer->endAnalysis();
  istringstream in(m_bowstream->str());
  BoWBinaryReader reader;
  reader.readHeader(in);
  reader.readBoWText(in,m_bowtext);

  // Insertion du bowtext au handler de contenu et de structure
  set_LastContentId(get_LastContentId()+1);
  CONTENT_ID contentId=get_LastContentId();
  ContentHandler< BoWText >::addContent(contentId,m_bowtext);
  // TODO: replace BOW_TEXT_NAME2 with some consistent value
  std::string BOW_TEXT_NAME2("BOW_TEXT_NAME2");
  Node node(BOW_TEXT_NAME2,1,1,contentId,2, "" , "text" , "bow" , "bow" , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 0 );
  add_Node(node);
}


void BowTextHandler::startAnalysis()
{
  m_bowtext.clear();
  if (m_bowstream!=0) {
    delete m_bowstream;
    m_bowstream=0;
  }
  //if (m_writer!=0) {
  //  delete m_writer;
  //  m_writer=0;
  //}
  if (m_writer == 0){
    m_writer=new BowTextWriter();
  }
  m_bowstream=new std::ostringstream();
  //m_writer=new BowTextWriter();
  m_writer->setOut(m_bowstream);
  m_writer->startAnalysis();
//    set_LastStructureId(1);
}

void BowTextHandler::handle(const char* buf, int length)
{
  m_writer->handle(buf,length);
}

void BowTextHandler::endDocument()
{
}

void BowTextHandler::startDocument(const Common::Misc::GenericDocumentProperties&)
{
}

void BowTextHandler::startNode( const std::string& /*elementName*/, bool /*forIndexing*/ )
{
}

void BowTextHandler::endNode(const Common::Misc::GenericDocumentProperties& /*props*/)
{
}

}

}
