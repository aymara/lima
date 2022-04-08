/******************************************************************************
 * File        : xmlSimpleHandler.cpp
 * Author      : CEA LIST
 * Created on  : janvier 2010
 * Copyright   : (c) 2010 by CEA
 * brief       : Manipulateur de données XML
 ******************************************************************************/
#ifndef XMLSIMPLEHANDLER_H
#define XMLSIMPLEHANDLER_H

#include "AnalysisHandlersExport.h"
#include <sstream>
#include "common/Handler/AbstractXmlDocumentHandler.h"

namespace Lima {
//! @brief Manipulation de media ; separation du contenu et de la structure
class LIMA_ANALYSISHANDLERS_EXPORT XmlSimpleHandler : public Lima::AbstractXmlDocumentHandler
{
public:
    XmlSimpleHandler(std::shared_ptr<const ShiftFrom> shiftFrom);

    ~XmlSimpleHandler() {};

    void handle ( const char* buf,int length ) override;

    void startAnalysis() override;

    // OLD : garde la version avec le props car hérite de AbstractTextualAnalysisHandler
    void startDocument (const Lima::Common::Misc::GenericDocumentProperties& props) override;

    //! @brief called at the end of a document analysis
    void endDocument() override;

    /** notify the end of an analysis content */
    void endAnalysis() override {}

    //! @brief called by the document analyzer for a new hierarchy node
    // TODO devrait s'appeler startHierarchyNode ?
    void startNode ( const std::string& elementName, bool forIndexing ) override;

    //! @brief called by the document analyzer at the end of a hierarchy node
    // TODO devrait s'appeler endHierarchyNode ?
    void endNode ( const Lima::Common::Misc::GenericDocumentProperties& props ) override;

    void setOut(std::ostream* out) override;

    std::ostream* m_out;
    std::ostringstream m_buffer;
    std::vector<bool> m_indexingNodeStack;
};

}

#endif
