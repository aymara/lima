/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST - LIC2M                              *
 *                                                                         *
 ***************************************************************************/

#include "XmlReaderClientFactory.h"
// #include "linguisticProcessing/XmlReaderCommon.h"
#include "linguisticProcessing/core/XmlProcessingCommon.h"
#include "linguisticProcessing/core/CoreXmlReaderClient.h"
#include "common/AbstractFactoryPattern/ProcessingClientFactory.h"
#include "XmlReaderException.h"

using namespace Lima;
using namespace Lima::Common;

namespace Lima
{
namespace XmlReader
{

void Lima::XmlReader::XmlReaderClientFactory::configureClientFactory(
  const std::string& id,
  XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
  const std::string& defaultMedia,
  const std::deque<std::string>& medias_to_load,
  const std::deque<std::string>& pipelines)
{
#ifdef DEBUG_LP
    XMLCLIENTFACTORYLOGINIT;
    LDEBUG << "XmlReaderClientFactory::configureClientFactory" << id << medias_to_load.size() << pipelines.size();
#endif

    //-------------------------------------------
    // configure le lecteur XML
    //-------------------------------------------
    m_configuration = std::unique_ptr< Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser >(new Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser(configuration));

    auto factory = AbstractXmlReaderClientFactory::getFactory(id);
    if(factory == 0) {
        XMLCLIENTFACTORYLOGINIT;
        LERROR << "No factory defined for client '" << id << "' !";
        throw XmlReaderException();
    }
    factory->configure(
        configuration,
        defaultMedia,
        medias_to_load,
        pipelines);

    //-------------------------------------------
    // Configure les clients d'analyse
    //-------------------------------------------

    //chercher les id des Factory à trouver

    std::string id2 = "lima-coreclient";

    std::map<std::string, std::string> MapTagMedia = m_configuration->getModuleGroupConfiguration(id, "documentXMLParser").getMapAtKey("indexingNodesCorrespondance");
    for (auto mapTagMediaIt = MapTagMedia.begin();
         mapTagMediaIt != MapTagMedia.end();
         mapTagMediaIt++)
    {
      if ((*mapTagMediaIt).second.empty())
      {
#ifdef DEBUG_LP
        LDEBUG << "XmlReaderClientFactory::configureClientFactory setting MapTagMedia[" << (*mapTagMediaIt).first << "] to" << defaultMedia;
#endif
        (*mapTagMediaIt).second = defaultMedia;
      }
    }
    auto indexingNodes = m_configuration->getModuleGroupConfiguration(id, "documentXMLParser").getListsValueAtKey("indexingNodes");
    for (auto indexingNodesIt = indexingNodes.begin();
         indexingNodesIt != indexingNodes.end();
         indexingNodesIt++)
    {
      if (MapTagMedia.find(*indexingNodesIt) == MapTagMedia.end())
      {
#ifdef DEBUG_LP
        LDEBUG << "XmlReaderClientFactory::configureClientFactory setting MapTagMedia[" << *indexingNodesIt << "] to" << defaultMedia;
#endif
        MapTagMedia.insert(std::make_pair(*indexingNodesIt, defaultMedia));
      }
    }

    factory->setMapTagMedia(MapTagMedia);
    std::map<std::string, std::string> MapMediaFactory = m_configuration->getModuleGroupConfiguration(id, "documentXMLParser").getMapAtKey("availableMediaAndFactories");
    factory->setMapMediaFactory(MapMediaFactory);

    // Correspondance factories -> media name
    // A factory such as lpFactory is used  for several media (all languages)
    std::multimap<std::string, std::string> MapFactoryMedia;
    std::map<std::string, std::string>::iterator ItrIds;
    for(ItrIds = MapMediaFactory.begin(); ItrIds != MapMediaFactory.end(); ItrIds++) {
        MapFactoryMedia.insert(std::make_pair(ItrIds->second, ItrIds->first));
    }

    // On parcourt la liste des media
    // * toutes les factories dans availableMediaAndFactories sont crees
    // * seuls celles indiquée dans medias_to_load sont configurés (resources chargées)
    std::multimap<std::string, std::string>::iterator ItrFact;
    std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ItrFacts;

    for(ItrIds = MapMediaFactory.begin(); ItrIds != MapMediaFactory.end(); ItrIds++) {

        // on ne cree une factory que si elle n'est pas encore dans la liste
        if(factory->getFactoryFromId(ItrIds->second) == 0) {

            // Create the processor factory
            auto factoryfactory =  Lima::AbstractProcessingClientFactoryFactory::getFactory(ItrIds->second);

            if(factoryfactory == 0) {
                XMLCLIENTFACTORYLOGINIT;
                LERROR << "No factoryfactory defined for factory '" << ItrIds->second << "' !";
                throw XmlReaderException();
            }
            std::shared_ptr< Lima::ProcessingClientFactory > factory_processor(factoryfactory->createProcessingClientFactory());

            // On ne configure que si le media est indiqué dans medias_to_load
            //TODO: fabriquer medias en fonction de la factory
            std::deque<std::string> medias;
            ItrFacts = MapFactoryMedia.equal_range(ItrIds->second);

            for(ItrFact = ItrFacts.first; ItrFact != ItrFacts.second; ItrFact++) {
                bool found = 0;
                for(auto it_media = medias_to_load.begin();
                         it_media != medias_to_load.end(); it_media++) {
                    if(it_media->compare(ItrFact->second) == 0) {
                        found = 1;
                    }
                }

                if(found == 1)
                {
                  medias.push_back(ItrFact->second);
#ifdef DEBUG_LP
                  LDEBUG << "XmlReaderClientFactory::configureClientFactory adding media [" << ItrFact->second << "] for the client [" << id2 << "]";
#endif
               }
            }

            if(medias.size() > 0) {

                std::stringstream tmp_list_media("");
                for(std::deque<std::string>::iterator Itrmedia = medias.begin();
                        Itrmedia != medias.end(); Itrmedia++)
                    tmp_list_media << *Itrmedia << " ";
#ifdef DEBUG_LP
                LDEBUG << "XmlReaderClientFactory::configureClientFactory configure clients factory [" << ItrIds->second << "]  factory [" << id2 << "] for media: " << tmp_list_media.str();
#endif

                factory_processor->configureClientFactory(
                    id2 ,
                    configuration,
                    medias,
                    pipelines);
            }

            // Ajout à la liste des media passés en revue
            factory->setFactoryFromId(ItrIds->second, factory_processor);
        }
    }
}

std::shared_ptr< AbstractXmlReaderClient > XmlReaderClientFactory::createClient(
    const std::string &id) const
{
#ifdef DEBUG_LP
    XMLCLIENTFACTORYLOGINIT;
    LDEBUG << "XmlReaderClientFactory: Create client '" << id << "'";
#endif

    const auto factory = AbstractXmlReaderClientFactory::getFactory(id);
    if(factory == 0) {
        XMLCLIENTFACTORYLOGINIT;
        LERROR << "No factory defined for client '" << id << "' !";
        throw XmlReaderException();
    }
    return factory->createClient(m_configuration.get());
}


std::deque<std::string> XmlReaderClientFactory::getRegisteredFactories() const
{
    return AbstractXmlReaderClientFactory::getRegisteredFactories();
}




} // XmlReader

} // Lima


