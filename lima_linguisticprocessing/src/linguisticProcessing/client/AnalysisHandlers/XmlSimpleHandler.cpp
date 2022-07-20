// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "XmlSimpleHandler.h"
#include "common/Data/DataTypes.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/strwstrtools.h"
#include <sstream>

using namespace Lima;
using namespace Lima;
using namespace std;

XmlSimpleHandler::XmlSimpleHandler(std::shared_ptr<const ShiftFrom> shiftFrom) :
    AbstractXmlDocumentHandler(shiftFrom),
m_out(0),
m_buffer(),
m_indexingNodeStack()
{
}

void XmlSimpleHandler::setOut(std::ostream* out)
{
    std::cerr << "XmlSimpleHandler::setOut" << std::endl;
  m_out = out;
}

void XmlSimpleHandler::startDocument (const Common::Misc::GenericDocumentProperties& props)
{
  std::cerr << "XmlSimpleHandler::startDocument" << std::endl;
  LIMA_UNUSED(props)
  (*m_out) << "<DOCSET>" << endl;
}

void XmlSimpleHandler::endDocument()
{
  std::cerr << "XmlSimpleHandler::endDocument" << std::endl;
  (*m_out) << "</DOCSET>" << endl;
  m_out->flush();
}

void XmlSimpleHandler::handle ( const char* buf,int length )
{
    std::cerr << "XmlSimpleHandler::handle" << std::endl;
  // do not write directly into buffer (need properties to write whole doc)
  m_buffer.write(buf,length);
}

void XmlSimpleHandler::startAnalysis()
{
}

//! @brief called by the document analyzer for a new hierarchy node
// TODO devrait s'appeler startHierarchyNode ?
void XmlSimpleHandler::startNode ( const string& elementName, bool forIndexing )
{
  LIMA_UNUSED(elementName)
  // do nothing: all is done at end node

  // just keep track of indexing nodes
  m_indexingNodeStack.push_back(forIndexing);
}

//! @brief called by the document analyzer at the end of a hierarchy node
// TODO devrait s'appeler endHierarchyNode ?
void XmlSimpleHandler::endNode ( const Common::Misc::GenericDocumentProperties& props )
{
  std::cerr << "XmlSimpleHandler::endNode" << std::endl;
  // only print output if end of indexing node
  if (! m_indexingNodeStack.back()) {
    m_indexingNodeStack.pop_back();
    return;
  }

  m_indexingNodeStack.pop_back();

  // get document id in properties
  string docId= props.getStringValue ( "identPrpty" ).first;
  std::replace( docId.begin(), docId.end(), ',', '_');

  (*m_out)
    << "<DOC id=\"" << docId << "\">" << endl
    << m_buffer.str()
    << "</DOC>" << endl;

  m_out->flush();

  // clear buffer
  m_buffer.str("");
}
