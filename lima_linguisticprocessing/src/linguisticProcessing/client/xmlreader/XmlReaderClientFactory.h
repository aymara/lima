// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef XMLREADERCLIENTFACTORY_H
#define XMLREADERCLIENTFACTORY_H

#include "XmlReaderClient_export.h"
#include "common/AbstractFactoryPattern/Singleton.h"

#include <string>
#include <deque>
#include <memory>


namespace Lima {
  namespace Common {
    namespace XMLConfigurationFiles {
      class XMLConfigurationFileParser;
    }
  }
}

namespace Lima {
namespace XmlReader {

class AbstractXmlReaderClient;

//! XmlReaderClientFactory is a singleton. It allows configuration
//! and creation of linguisticProcessing clients.
//! @brief main entry point of XmlReader Component
class XMLREADERCLIENT_EXPORT XmlReaderClientFactory : public Lima::Singleton<XmlReaderClientFactory>
{
    friend class Lima::Singleton<XmlReaderClientFactory>;

public:

    //! @brief configure the corresponding clientFactory so that it can create clients.
    //! @param id @b IN : id of clientFactory to configure
    //! @param configuration fichier de configuration, comportant un <module name="lp-structuredXmlreaderclient">
    //! @param media_to_load
    //! @param pipelines
    //! @throw Lima::LimaException if clientFactory does not exist, or if an
    //!  exception occurs during configuration. \n
    //! All the processors in 'availableMediaAndFactories' (cf. config file)
    //! are created, but only those in media_to_load are configured (i.e
    //! their resources are loaded)
    void configureClientFactory (
        const std::string& id,
        Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
        const std::string& defaultMedia,
        const std::deque<std::string>& medias_to_load = std::deque<std::string>(),
        const std::deque<std::string>& pipelines = std::deque<std::string>() );

    //! @brief create an Client using the appropriate registered factory.
    //! @param id @b IN : classId for the factory
    //! @return Client* pointer to the created Client.
    //! Pointer should be deleted when no more used
    //! @throw Lima::LimaException
    //!
    //! ClientFactory must have been configured before this method is called
    //! Use configureClientFactory() method to configure.
    std::shared_ptr< AbstractXmlReaderClient > createClient ( const std::string& id ) const;

    //! @brief show registered clientId
    //! @return list of registered clientId
    std::deque<std::string> getRegisteredFactories() const;

    virtual ~XmlReaderClientFactory() {};

private:

    XmlReaderClientFactory() {};

    std::unique_ptr< Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser > m_configuration;

};

} // XmlReader

} // Lima

#endif
