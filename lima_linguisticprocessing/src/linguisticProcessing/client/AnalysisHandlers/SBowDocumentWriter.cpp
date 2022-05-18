// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "SBowDocumentWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

namespace Lima
{

namespace LinguisticProcessing
{

SBowDocumentWriter::SBowDocumentWriter()
    : AbstractTextualAnalysisHandler(),m_out(0)
{
}

void SBowDocumentWriter::setOut( std::ostream* out )
{
  m_out = out;
  Common::BagOfWords::BoWBinaryWriter writer;
  writer.writeHeader(*m_out,Common::BagOfWords::BOWFILE_SDOCUMENT);
}

SBowDocumentWriter::~SBowDocumentWriter()
{}

/** notify the start of an analysis content */
void SBowDocumentWriter::startAnalysis()
{
  *m_out << Common::BagOfWords::BOW_TEXT_BLOC;
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
}

/** notify the end of an analysis content */
void SBowDocumentWriter::endAnalysis()
{}

void SBowDocumentWriter::startDocument(const Common::Misc::GenericDocumentProperties& /*props*/)
{
}

/** notify the end of the document */
void SBowDocumentWriter::endDocument()
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "SBowDocumentWriter::endDocument()";
  m_out->flush();
}

/** gives content. Content is a serialized form of the expected resultType */
void SBowDocumentWriter::handle(const char* buf,int length) 
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "SBowDocumentWriter::handle()";
  m_out->write(buf,length);
}

void SBowDocumentWriter::startNode( const std::string& elementName, bool forIndexing )
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "SBowDocumentWriter::startNode(" << elementName << "," << forIndexing << ")";
  if( forIndexing )
    Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::INDEXING_BLOC);
  else
    Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::HIERARCHY_BLOC);
  Lima::Common::Misc::writeStringField(*m_out, elementName);
}
  
  /** notify the end of a hierarchyNode */
void SBowDocumentWriter::endNode( const Common::Misc::GenericDocumentProperties& props )
{
  LPCLIENTSBOWHANDLERLOGINIT;
  LDEBUG << "SBowDocumentWriter::endNode()";
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::NODE_PROPERTIES_BLOC);
  props.write(*m_out);
  Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::END_BLOC);
  m_out->flush();
}

}

}
