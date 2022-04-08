/*
    Copyright 2004-2021 CEA LIST

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
#ifndef LIMA_XMLSIMPLESTREAMHANDLER_H
#define LIMA_XMLSIMPLESTREAMHANDLER_H

#include "AnalysisHandlersExport.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "common/Handler/AbstractXmlDocumentHandler.h"
#include <ostream>

namespace Lima {
//! @brief Manipulation de media ; separation du contenu et de la structure
class LIMA_ANALYSISHANDLERS_EXPORT XmlSimpleStreamHandler :
  public Lima::LinguisticProcessing::SimpleStreamHandler,
    public Lima::AbstractXmlDocumentHandler
{
public:

    XmlSimpleStreamHandler(std::shared_ptr<const ShiftFrom> shiftFrom);
    ~XmlSimpleStreamHandler();

    /** notify the start of a new document */
    void startDocument(const Lima::Common::Misc::GenericDocumentProperties& props) override;

    /** notify the end of the document */
    void endDocument() override;

    void endAnalysis() override;

    void startAnalysis() override;

    /** gives content. Content is a serialized form of the expected resultType */
    void handle(const char* buf,int length) override;

    /** notify the start of a new hierarchyNode */
    void startNode( const std::string& elementName, bool forIndexing ) override;

    /** notify the end of a hierarchyNode */
    void endNode( const Lima::Common::Misc::GenericDocumentProperties& props ) override;

    void setOut(std::ostream* out) override;
};

}

#endif
