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
/**
 * @date        April, 2009
 */
#include "CoreXmlReaderClient.h"

#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/client/xmlreader/XmlReaderException.h"
#include "linguisticProcessing/core/XmlProcessingCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/DocumentsReader/contentDocument.h"
#include "linguisticProcessing/core/DocumentsReader/DocumentElements.h"
#include "linguisticProcessing/core/DocumentsReader/documentReader.h"
#include "common/Handler/AbstractXmlDocumentHandler.h"
#include "common/AbstractFactoryPattern/ProcessingClientFactory.h"
#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <cassert>

using namespace Lima;
using namespace std;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::DocumentsReader;

namespace Lima
{
namespace XmlReader
{

CoreXmlReaderClient::CoreXmlReaderClient(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser &configuration) :
/*m_delegate(0),*/m_handler(nullptr)
{
#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClient::CoreXmlReaderClient";
#endif
    auto& conf = configuration.getModuleConfiguration("lp-structuredXmlreaderclient");
    m_documentReader = new DocumentsReader::DocumentReader(conf);
    m_documentReader->setLinguisticXMLDocHandler(this);
    m_emptyTextChars = utf8stdstring2limastring(" \t\n");

}

CoreXmlReaderClient::~CoreXmlReaderClient()
{
    delete m_documentReader;
}

void CoreXmlReaderClient::startHierarchy(const DocumentsReader::ContentStructuredDocument &contentDocument)
{
    m_handler->m_parentlastOpenedNode = 0;
    m_handler->set_LastStructureId(m_handler->get_LastStructureId() + 1);
    m_handler->set_lastNodeId(1);
    m_handler->m_openedNodes.clear();
    m_handler->m_openedNodes.push_back(0);
    startNode(contentDocument, false);
}

void CoreXmlReaderClient::endHierarchy(const DocumentsReader::ContentStructuredDocument &contentDocument)
{
    // last element
    auto absElement = contentDocument.back();
    auto element  =
        dynamic_cast<DocumentsReader::HierarchyDocumentElement *>(absElement);

    // must be a structured Element
    assert(element);
    // get byte offset before the beginning of the closing tag
    element->setIntValue("offEndPrpty", element->getOffset());


#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClient::endHierarchy("
           << element->getElementName() << ")"
           << " offBegPrpty "
           << element->getIntValue("offBegPrpty").first
           << " offEndPrpty "
           << element->getIntValue("offEndPrpty").first;
#endif

    // register properties and close block
    m_handler->endNode(*element);
}

void CoreXmlReaderClient::startIndexing(
  const DocumentsReader::ContentStructuredDocument &contentDocument)
{
    startNode(contentDocument, true);
}

void CoreXmlReaderClient::endIndexing(
  const DocumentsReader::ContentStructuredDocument &contentDocument)
{
#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClient::endIndexing";
 #endif
   // last element
    auto absElement = contentDocument.back();
    auto element =
      dynamic_cast<DocumentsReader::IndexingDocumentElement *>(absElement);

    // must be a structured Element
    if (element == nullptr)
    {
      XMLREADERCLIENTLOGINIT;
      LERROR << "CoreXmlReaderClient::endIndexing must be a structured Element" ;
      assert(false);
    }
    // get byte offset before the beginning of the closing tag
    element->setIntValue("offEndPrpty", element->getOffset());


#ifdef DEBUG_LP
    LDEBUG << "CoreXmlReaderClient::endIndexing("
           << element->getElementName() << ")"
           << " offBegPrpty "
           << element->getIntValue("offBegPrpty").first
           << " offEndPrpty "
           << element->getIntValue("offEndPrpty").first;
#endif

    // register properties and close block
    m_handler->endNode(*element);
}


void CoreXmlReaderClient::handle(
    const DocumentsReader::ContentStructuredDocument &contentDocument,
    const Lima::LimaString &text,
    unsigned long int offset,
    const string  tagName)
{
#ifdef DEBUG_LP
  XMLREADERCLIENTLOGINIT;
#endif

    if(std::string(text.toUtf8().constData()).find_first_not_of(m_emptyTextChars.toUtf8().constData()) == string::npos) {
#ifdef DEBUG_LP
        LDEBUG << "CoreXmlReaderClient::empty text, not analyzed";
#endif
        return;
    }

    AbstractStructuredDocumentElement* absElement = contentDocument.back();
    DocumentsReader::IndexingDocumentElement* element  =
        dynamic_cast<DocumentsReader::IndexingDocumentElement*>(absElement);
    std::string elementName = element->getElementName().toUtf8().constData();

#ifdef DEBUG_LP
    if( logger.loggingLevel() == QsLogging::DebugLevel )
    {
      LDEBUG << "CoreXmlReaderClient::handle"
            << "[" << text << "], offset =" << offset
            << ", tagName =" << tagName << ", element name =" << elementName ;
    }
    else if( logger.loggingLevel() == QsLogging::InfoLevel )
#endif
    {
      // Chercher les analyses diponibles
      XMLREADERCLIENTLOGINIT;
      LINFO << "CoreXmlReaderClient::handle"
            << "[" << text.left(50) << "], offset =" << offset
            << ", tagName =" << tagName ;
    }
    ostringstream os;
    os << offset;
    m_docMetaData["StartOffset"] = os.str();
    m_docMetaData["ElementName"] = tagName;
    // Set the language to the one associated at init time to the current tag
    if (m_mapTagMedia.find(elementName) != m_mapTagMedia.end())
    {
#ifdef DEBUG_LP
      LDEBUG << "CoreXmlReaderClient::handle using media" << m_mapTagMedia[elementName];
#endif
      m_docMetaData["Lang"] =  m_mapTagMedia[elementName];
    }
    else if (!m_defaultMedia.empty())
    {
#ifdef DEBUG_LP
      LDEBUG << "CoreXmlReaderClient::handle using default media" << m_defaultMedia;
#endif
      m_docMetaData["Lang"] =  m_defaultMedia;
    }
    else
    {
      XMLREADERCLIENTLOGINIT;
      LERROR << "CoreXmlReaderClient::handle no media associated to tag"
          << elementName
          << "and no default media is set. metadata Lang will not be set.";
    }
    // cast element to GenericDocumentProperties
//     Common::Misc::GenericDocumentProperties &props = *element;
    // get byte offset after end of element
    auto offsetIndexingNode = element->getIntValue("offBegPrpty").first;
    ostringstream os2;
    os2 << offsetIndexingNode;
    m_docMetaData["StartOffsetIndexingNode"] = os2.str();

    auto strText = text.toStdString();

    m_handler->handleProc(
        tagName,
        strText,
        m_docMetaData,
        m_docMetaData["pipeline"],
        m_mapHandlers,
        std::set<std::string>());
}


void CoreXmlReaderClient::handleProperty(
    const DocumentsReader::DocumentPropertyType &property,
    const std::string &data)
{

    if(!property.getId().compare("langPrpty")) {
        if(data.compare(m_docMetaData["Lang"])) {
//       LWARN << "StructuredXmlReaderLinguisticProcessingClient::handleProperty: "
//         << "it is forbidden to change language inside an indexing unit!";
//       LWARN << data << " != " << m_docMetaData["Lang"];
        } else {
            m_docMetaData["Lang"] = data;
        }
    }
    if(!property.getId().compare("identPrpty")) {
        std::string ident = data;
        std::replace( ident.begin(), ident.end(), ',', '_');
        m_docMetaData["Filename"].append(data);
        m_docMetaData["docid"] = ident;
    }
    if(!property.getId().compare("pipelineProperty"))
        m_docMetaData["pipeline"].append(data);

    // added location
    if(!property.getId().compare("locationPrpty")) {
            m_docMetaData["location"] = data;
    }
}

//!@brief analyse effective d'un fichier XML
//! Fait appel a m_handler (type xmlDocumentHandler)
void CoreXmlReaderClient::analyze(
    const std::string &text,
    const std::map<std::string, std::string>& metaData,
    const std::string &pipeline,
    const std::map<std::string, Lima::AbstractAnalysisHandler *>& handlers,
    const std::set<std::string>& inactiveUnits) const
{
    (void) handlers;      // avoid warning
    (void) inactiveUnits; // avoid warning


#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;

    LDEBUG << "CoreXmlReaderClient::analyze(... pipeline='" << pipeline << "')";
#endif
    if (m_handler==0)
    {
        XMLREADERCLIENTLOGINIT;
        LERROR << "Error XMLreader: no handler set. It will crash.";
    }
//     if ( !m_documentReader->initWithString ( text ) )
//     {
//         LERROR << "CoreXmlReaderClient::analyze: can't init reader with text ! ";
//         throw XmlReaderException();
//     }
//     LDEBUG << "CoreXmlReaderClient::analyze after initWithString";
//
    std::map<std::string, std::string>* docMetadataPtr =
        const_cast<std::map<std::string, std::string>*>(&m_docMetaData);
    *docMetadataPtr = metaData;
    (*docMetadataPtr) ["pipeline"] = pipeline;

    Common::Misc::GenericDocumentProperties dummyProperties;
    m_handler->startDocument(dummyProperties);   // NB : ne fait rien!
#ifdef DEBUG_LP
    LDEBUG << "CoreXmlReaderClient::analyze: start analyze"
            << (*docMetadataPtr) ["filePath"] << " ; pipeline='"
            << std::string((*docMetadataPtr) ["pipeline"]) << "' ; language='"
            << std::string((*docMetadataPtr)["Lang"]) << "'";
#endif
    m_handler->set_lastUri(string((*docMetadataPtr) ["filePath"]));
    m_handler->set_lang(string((*docMetadataPtr) ["Lang"]));

    if(!m_documentReader->initWithString(text)) {
        XMLREADERCLIENTLOGINIT;
        LERROR << "CoreXmlReaderClient::analyze: can't init reader with text !";
        throw XmlReaderException("CoreXmlReaderClient::analyze: can't init reader with text !");
    }
#ifdef DEBUG_LP
    LDEBUG << "CoreXmlReaderClient::analyze after initWithString";
#endif

#ifndef DEBUG_LP
     try {
#endif
        m_documentReader->readXMLDocument();
#ifndef DEBUG_LP
     } catch (const Lima::LinguisticProcessing::LinguisticProcessingException& e) {
         XMLREADERCLIENTLOGINIT;
         LERROR << "Error in XMLreader: " << e.what();
     } catch (const Lima::LimaException& e) {
         XMLREADERCLIENTLOGINIT;
         LERROR << "Error in XMLreader: " << e.what();
     }
#endif
     m_handler->endDocument();
}

void CoreXmlReaderClient::startNode(const DocumentsReader::ContentStructuredDocument &contentDocument, bool isIndexing)
{
    AbstractStructuredDocumentElement *absElement = contentDocument.back();
    DocumentsReader::AbstractStructuredDocumentElementWithProperties *element  =
        dynamic_cast<DocumentsReader::AbstractStructuredDocumentElementWithProperties *>(absElement);
    assert(element);
#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClient::startNode(" << element->getElementName() << ")";
#endif

    // nom du fichier de la ligne de commande prioritaire
    element->GenericDocumentProperties::setStringValue("srcePrpty", m_docMetaData["FileName"]);
    // set the language property to the value associated to the current tag
    if(m_mapTagMedia.find(element->getElementName().toUtf8().constData()) != m_mapTagMedia.end()) {
#ifdef DEBUG_LP
        LDEBUG << "CoreXmlReaderClient::startNode() set langPrpty to: " << m_mapTagMedia[element->getElementName().toUtf8().constData()];
#endif
        element->GenericDocumentProperties::setStringValue("langPrpty", m_mapTagMedia[element->getElementName().toUtf8().constData()]);
    }
    element->GenericDocumentProperties::setStringValue("encodPrpty", "UTF8");
    // set date as today
    element->GenericDocumentProperties::setDateValue("indexDatePrpty", QDate::currentDate());

#if 0
    if(!m_docMetaData["type"].empty()) {
        element->GenericDocumentProperties::setStringValue("typPrpty", m_docMetaData["type"]);
#ifdef DEBUG_LP
        LDEBUG << "CoreXmlReaderClient::startNode() set typPrpty to: " << m_docMetaData["type"];
#endif
    }
#endif


#ifdef DEBUG_LP
    LDEBUG << "CoreXmlReaderClient::startNode m_handler::startNode(" << element->getElementName() << ")";
#endif
    m_handler->startNode(element->getElementName().toUtf8().constData(), isIndexing);

    element->GenericDocumentProperties::setIntValue("StructureId", m_handler->get_LastStructureId());
    if(m_handler->m_openedNodes.empty()) {
        // no lastOpenedNode : may happen if no hierarchy node in file (root node is indexing node)
#ifdef DEBUG_LP
        LDEBUG << "Warning: No hierarchy node => no NodeId in document properties";
#endif
    } else {
        element->GenericDocumentProperties::setIntValue("NodeId", *m_handler->m_openedNodes.rbegin());
    }
    element->GenericDocumentProperties::setIntValue("ContentId", m_handler->get_LastContentId());
}

void CoreXmlReaderClient::setAnalysisHandler(const std::string &handlerId, Lima::AbstractAnalysisHandler *handler)
{
#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClient::setAnalysisHandler (" << handlerId << ", "
            << handler << ")";
#endif
    if (m_mapHandlers.find(handlerId) != m_mapHandlers.end()) {
      XMLREADERCLIENTLOGINIT;
      LERROR << "Error: CoreXmlReaderClient::setAnalysisHandler already contains a handler with the ID"
              << handlerId;
      return;
    }
    auto abstractHandler = dynamic_cast<AbstractXmlDocumentHandler*>(handler);
    if(abstractHandler != 0) {
        // lien avec les clients d'analyse
        m_handler = abstractHandler;
        m_handler->setAnalysisClients(m_processingClientHandler.getAnalysisClients());
        m_documentReader->setShiftFrom(&m_handler->shiftFrom());
        m_mapHandlers.insert(make_pair(handlerId, handler));
    } else {
        XMLREADERCLIENTLOGINIT;
        LERROR << "Error: handler " << handlerId
                << " is not compatible with XML analysis. Segmentation fault to come...";
    }
}

void CoreXmlReaderClient::releaseAnalysisHandler(const std::string &handlerId)
{
#ifdef DEBUG_LP
  XMLREADERCLIENTLOGINIT;
  LDEBUG << "CoreXmlReaderClient::releaseAnalysisHandler (" << handlerId << ")";
#endif
  if (m_mapHandlers.find(handlerId) == m_mapHandlers.end()) {
    XMLREADERCLIENTLOGINIT;
    LERROR << "Error: CoreXmlReaderClient::releaseAnalysisHandler does not contain a handler with the ID" << handlerId;
    return;
  }
  delete m_mapHandlers[handlerId];
  m_mapHandlers.erase(handlerId);
}

//===========================================================================
//                              Factory
//===========================================================================
CoreXmlReaderClientFactory::CoreXmlReaderClientFactory() :
    AbstractXmlReaderClientFactory("lp-structuredXmlreaderclient")
{}

CoreXmlReaderClientFactory::~CoreXmlReaderClientFactory()
{}

std::unique_ptr< CoreXmlReaderClientFactory > CoreXmlReaderClientFactory::s_instance(new CoreXmlReaderClientFactory());

//-----------------------------------------
// Configuration du lecteur XML
//-----------------------------------------
void CoreXmlReaderClientFactory::configure(
    const Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser &configuration,
    const std::string& defaultMedia,
    const std::deque<std::string>& medias,
    const std::deque<std::string>& pipelines)
{
    (void) configuration; // avoid warning
    m_defaultMedia = defaultMedia;  // avoid warning
    (void) medias;        // avoid warning
    (void) pipelines;     // avoid warning
}

//-----------------------------------------
// Creation du client de lecture XML
// Creation des clients d'analyse
// Associe
//-----------------------------------------
std::shared_ptr< AbstractXmlReaderClient > CoreXmlReaderClientFactory::createClient(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser *configuration) const
{
#ifdef DEBUG_LP
    XMLREADERCLIENTLOGINIT;
    LDEBUG << "CoreXmlReaderClientFactory::createClient";
#endif

    std::shared_ptr< AbstractXmlReaderClient > client(new CoreXmlReaderClient(*configuration));

    // containeur (map) permettant de les retrouver un client d'analyse
    // a partir  du nom de la fabrique
    std::map<std::string, std::shared_ptr< AbstractProcessingClient > > mapMediaClient;

    // parcourt des media disponibles: crée les client d'analyse (PC) nécessaires
    for(auto ItrMedia = m_mapMediaFactory.cbegin();
        ItrMedia != m_mapMediaFactory.cend(); ItrMedia++)
    {
        string media = ItrMedia->first;
        string PCfactoryName = ItrMedia->second;
#ifdef DEBUG_LP
        LDEBUG << "Media [" << media << "] is analyzed by [" << PCfactoryName
                << "]";
#endif
        std::shared_ptr< Lima::ProcessingClientFactory >PCfactory = getFactoryFromId(PCfactoryName);

        if(mapMediaClient.find(PCfactoryName) == mapMediaClient.end())
        {
          std::shared_ptr< AbstractProcessingClient > limaClient(PCfactory->createClient("lima-coreclient"));
          mapMediaClient[PCfactoryName] = limaClient;
//             LDEBUG << "mapMediaClient insert " << PCfactoryName << " (" << (void *)PCfactory << ") " << media << " " << client << " " << mapMediaClient[FactoryName];
        }
    }

    // lien des noms de balises (tags) aux clients d'analyse
    for(auto ItrTags = m_mapTagMedia.begin(); ItrTags != m_mapTagMedia.end(); ItrTags++) {
        string TagName = ItrTags->first;
        string media = ItrTags->second;
        if (m_mapMediaFactory.find(media) == m_mapMediaFactory.end())
        {
          XMLREADERCLIENTLOGINIT;
          LINFO << "Tag [" << TagName << "] corresponding to media [" << media << "] has no factory associated";
          continue;
        }
        std::string PCfactoryName = m_mapMediaFactory.find(media)->second;
#ifdef DEBUG_LP
        LDEBUG << "Tag [" << TagName << "] correspond to media [" << media << "], analysed by [" << PCfactoryName << "]";
#endif

        std::shared_ptr< AbstractProcessingClient > PCclient = mapMediaClient[m_mapMediaFactory.find(media)->second];

        client->m_processingClientHandler.setAnalysisClient(TagName, PCclient);
    }
    // Gives the tag to language map to the client to allow it to retrieve the language at analysis time
    client->setMapTagMedia(m_mapTagMedia);
    client->setDefaultMedia(m_defaultMedia);
    client->setDocumentPropertyConfiguration(configuration);

    return client;
}

} // XmlReader
} // Lima
