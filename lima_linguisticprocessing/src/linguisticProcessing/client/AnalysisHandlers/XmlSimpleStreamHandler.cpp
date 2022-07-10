// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "XmlSimpleStreamHandler.h"

using namespace Lima;
using namespace Lima::LinguisticProcessing;

namespace Lima {

XmlSimpleStreamHandler::XmlSimpleStreamHandler(std::shared_ptr<const ShiftFrom> shiftFrom):
SimpleStreamHandler(),
AbstractXmlDocumentHandler(shiftFrom)
{}

XmlSimpleStreamHandler::~XmlSimpleStreamHandler()
{}

/** notify the start of a new document */
void XmlSimpleStreamHandler::startDocument(const Lima::Common::Misc::GenericDocumentProperties& props) {
    SimpleStreamHandler::startDocument(props);
}

/** notify the end of the document */
void XmlSimpleStreamHandler::endDocument() {
    SimpleStreamHandler::endDocument();
}

void XmlSimpleStreamHandler::endAnalysis() {
    SimpleStreamHandler::endAnalysis();
}

void XmlSimpleStreamHandler::startAnalysis() {
    SimpleStreamHandler::startAnalysis();
}

/** gives content. Content is a serialized form of the expected resultType */
void XmlSimpleStreamHandler::handle(const char* buf,int length) {
    SimpleStreamHandler::handle(buf,length);
}

/** notify the start of a new hierarchyNode */
void XmlSimpleStreamHandler::startNode( const std::string& elementName, bool forIndexing ) {
    SimpleStreamHandler::startNode(elementName,forIndexing);
}

/** notify the end of a hierarchyNode */
void XmlSimpleStreamHandler::endNode( const Lima::Common::Misc::GenericDocumentProperties& props ) {
    SimpleStreamHandler::endNode(props);
}

void XmlSimpleStreamHandler::setOut(std::ostream* out) {
    SimpleStreamHandler::setOut(out);
}

} // namespace Lima
