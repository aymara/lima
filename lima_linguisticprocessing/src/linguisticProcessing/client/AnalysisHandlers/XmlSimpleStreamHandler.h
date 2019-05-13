/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST- LIC2M                               *
 *                                                                         *
 ***************************************************************************/
#ifndef FRCEALIC2M_XMLSIMPLESTREAMHANDLER_H
#define FRCEALIC2M_XMLSIMPLESTREAMHANDLER_H

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

    XmlSimpleStreamHandler();
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
