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
#include "BowDocumentWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/readwritetools.h"

namespace Lima
{

namespace LinguisticProcessing
{

BowDocumentWriter::BowDocumentWriter(std::ostream* out)
    : AbstractTextualAnalysisHandler(),m_out(out)
{
  Common::BagOfWords::BoWBinaryWriter writer;
  writer.writeHeader(*out,Common::BagOfWords::BOWFILE_DOCUMENT);
}


BowDocumentWriter::~BowDocumentWriter()
{}

/** notify the start of an analysis content */
void BowDocumentWriter::startAnalysis()
{
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
}

/** notify the end of an analysis content */
void BowDocumentWriter::endAnalysis()
{}

void BowDocumentWriter::startDocument(const Common::Misc::GenericDocumentProperties& props)
{
  // enum type with small number of values coded on one byte;
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::DOCUMENT_PROPERTIES_BLOC);
  props.write(*m_out);
}

/** notify the end of the document */
void BowDocumentWriter::endDocument()
{
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::END_BLOC);
  m_out->flush();
}

/** gives content. Content is a serialized form of the expected resultType */
void BowDocumentWriter::handle(const char* buf,int length) 
{
  m_out->write(buf,length);
}

void BowDocumentWriter::startNode( const std::string& /*elementName*/, bool /*forIndexing*/ )
{
}
  
  /** notify the end of a hierarchyNode */
void BowDocumentWriter::endNode( const Common::Misc::GenericDocumentProperties& /*props*/ )
{
}

}

}
