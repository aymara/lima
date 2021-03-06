/******************************************************************************
 * File        : xmlDocumentHandler.cpp
 * Author      : CEA LIST
 * Created on  : janvier 2010
 * Copyright   : (c) 2010 by CEA
 * brief       : Manipulateur de données XML
 ******************************************************************************/
#ifndef MULTIMEDIADOCUMENTHANDLER_H
#define MULTIMEDIADOCUMENTHANDLER_H

#include "linguisticProcessing/client/AnalysisHandlers/AnalysisHandlersExport.h"
#include "common/Handler/AbstractXmlDocumentHandler.h"

#include <sstream>

namespace Lima {
namespace Handler {

typedef enum {
  BIN_IMA_BLOC,
  BIN_VID_BLOC
} MultimediaFileType;

//! @brief Manipulation de media ; separation du contenu et de la structure
class LIMA_ANALYSISHANDLERS_EXPORT MultimediaDocumentHandler : public Lima::AbstractXmlDocumentHandler
{
public:
  MultimediaDocumentHandler(const QMap< uint64_t,uint64_t >& shiftFrom = QMap< uint64_t,uint64_t >());

  ~MultimediaDocumentHandler() {};

    void handle ( const char* buf,int length ) override;

    void startAnalysis(const std::string& bloc_type) override;

    void startAnalysis() override;

    // OLD : garde la version avec le props car hérite de AbstractTextualAnalysisHandler
    void startDocument (const Lima::Common::Misc::GenericDocumentProperties& props)  override {Lima::Common::Misc::GenericDocumentProperties rien=props;};

    //! @brief called at the end of a document analysis
    void endDocument() override{  m_out->flush();  };

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

private:
  MultimediaDocumentHandler(const MultimediaDocumentHandler&);
  MultimediaDocumentHandler& operator=(const MultimediaDocumentHandler&);
};

}
}

#endif
