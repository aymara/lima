/*
    Copyright 2009-2021 CEA LIST

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
#ifndef COREXMLREADERCLIENT_H
#define COREXMLREADERCLIENT_H

#include "CoreXmlReaderClient_export.h"
#include "linguisticProcessing/client/xmlreader/AbstractXmlReaderClient.h"
#include "linguisticProcessing/core/StructuredXmlDocumentHandler.h"
#include "common/Handler/AbstractProcessingClientHandler.h"

namespace Lima {
  class AbstractXmlDocumentHandler;
}

namespace Lima {
namespace DocumentsReader {
  class DocumentReader;
  class ContentStructuredDocument;
  class DocumentPropertyType;
}
namespace XmlReader {

//! @brief client de lecture des fichier XML faisant appel aux clients d'analyse
//! Lors de la lecture d'un fichier XML, les clients d'analyse sont appelés
//! en fonction des tags trouvés. Par exemple:
//! <ima>       appel à l'analyse d'images fixes
//! <fre>       appel à l'analyse linguistique en français
//! <eng>       appel à l'analyse linguistique en fanglais
//! Ce client de lecture étant donné en handler (StructuredXmlDocumentHandler)
//! au programme de lecture XML (DocumentReader) il est a nouveau appelé pour
//! effectuer certaines actions quand des noeuds sont lus.
class COREXMLREADERCLIENT_EXPORT CoreXmlReaderClient :
    public AbstractXmlReaderClient,
    public StructuredXmlDocumentHandler
{
public:
    //! @brief constructeur
    //! @param configuration TODO decrire
    CoreXmlReaderClient ( Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration );

    //! @brief destructeur
    ~CoreXmlReaderClient();

    //-------------------------------------------------------------
    // En tant que client d'analyse (AbstractXmlReaderClient)
    //-------------------------------------------------------------
    //! @brief analyse effective d'un fichier XML.
    //! @param text le texte (xml) a analyser
    //! @param metaData TODO decrire
    //! @param pipeline TODO decrire
    //! @param inactiveUnits TODO decrire
    void analyze(const std::string& content,
                         const std::map<std::string,std::string>& metaData,
                         const std::string& pipeline,
                         const std::map<std::string, Lima::AbstractAnalysisHandler*>& handlers = std::map<std::string, Lima::AbstractAnalysisHandler*>(),
                         const std::set<std::string>& inactiveUnits = std::set<std::string>()) const override;

    //! @brief associe un handler recupérant l'analyse XML
    void setAnalysisHandler(const std::string& handlerId, Lima::AbstractAnalysisHandler* handler) override;

    /** deletes the analysis handler with the given id and removes it */
    void releaseAnalysisHandler(const std::string &handlerId) override;


    //---------------------------------------------------------------
    // En tant que handler de XML (StructuredXmlDocumentHandler)
    //---------------------------------------------------------------

    void startHierarchy ( const DocumentsReader::ContentStructuredDocument& contentDocument ) override;

    void endHierarchy ( const DocumentsReader::ContentStructuredDocument& contentDocument ) override;

    void startIndexing ( const DocumentsReader::ContentStructuredDocument& contentDocument ) override ;
    void endIndexing ( const DocumentsReader::ContentStructuredDocument& contentDocument ) override;

	//! @brief callback to the handler. Actually run the adapted analyzers.
	//! @param contentDocument structure of the document
	//! @param text text content to analyze (may an image path...)
	//! @param offset ??? unused ?
	//! @param tagName XML tag for the text analysed:
	//! should specify the content type (freText, image...)
    void handle (const DocumentsReader::ContentStructuredDocument& contentDocument,
                 const Lima::LimaString& text,
                 unsigned long int offset,
                 const std::string tagName) override;

    void handleProperty ( const DocumentsReader::DocumentPropertyType& property,
                          const std::string& data ) override ;

private:
    void startNode ( const DocumentsReader::ContentStructuredDocument& contentDocument, bool isIndexing );

    //! @brief manipulateur recevant le resultat d'analyse
    Lima::AbstractXmlDocumentHandler* m_handler;

    //! @brief le lecteur de document XML proprement dit
    DocumentsReader::DocumentReader* m_documentReader;

    // arguments pour l'analyseur, initialise avec l'argument metadata d'analyze()
    // contient eventuellement des valeurs pour  "FileName" et "Lang" et "Type" ??
    std::map<std::string,std::string> m_docMetaData;

	//! @brief caracteres consideres comme espace, retrour de ligne, etc
    Lima::LimaString m_emptyTextChars;

	//! @brief liste des handlers pour chaque "media processor"
    std::map<std::string, Lima::AbstractAnalysisHandler*> m_mapHandlers;

    // @brief resultat de l'analyse du fichier XML
//     std::map<std::string, std::map<int , std::map<int, std::vector< std::pair<int, std::string> > > > > m_mapToIndex;
//      LinguisticProcessing::AbstractLinguisticProcessingClient* m_delegate;
};

//! @brief fabrique du client d'analyse de document XML
class COREXMLREADERCLIENT_EXPORT CoreXmlReaderClientFactory : public AbstractXmlReaderClientFactory
{
public:

    void configure(
        const Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser &configuration,
        const std::string& defaultMedia,
        const std::deque<std::string>& medias,
        const std::deque<std::string>& pipelines) override;

    std::shared_ptr< AbstractXmlReaderClient > createClient(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser *configuration) const override;

    virtual ~CoreXmlReaderClientFactory();

private:
    CoreXmlReaderClientFactory();
    static std::unique_ptr< CoreXmlReaderClientFactory > s_instance;

    std::string m_defaultMedia;
};

class COREXMLREADERCLIENT_EXPORT MultimediaProcessingClientHandler: public Lima::AbstractProcessingClientHandler
{
  public:
     MultimediaProcessingClientHandler():AbstractProcessingClientHandler(){};
     ~MultimediaProcessingClientHandler(){};
};

} // XmlReader
} // Lima



#endif
