/***************************************************************************
 *   Copyright (C) 2009 by CEA - LIST- LIC2M                               *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTCXMLREADERCLIENT_H
#define ABSTRACTCXMLREADERCLIENT_H

#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"
#include "common/AbstractProcessingClient/AbstractProcessingClient.h"
#include "common/Handler/AbstractProcessingClientHandler.h"

namespace Lima {
  class ProcessingClientFactory;
}
namespace Lima
{

namespace XmlReader
{

//! @brief interface publique au client d'analyse de fichier XML
class AbstractXmlReaderClient: public Lima::AbstractProcessingClient
{
public:

    //! @brief define a virtual destructor to ensure concrete client destructors to be called
    virtual ~AbstractXmlReaderClient() { }

    //! @brief Analyse un fichier XML
    //! @param text le texte (xml) a analyser
    //! @param metaData TODO decrire
    //! @param pipeline TODO decrire
    //! @param resultType TODO decrire
    //! @param inactiveUnits TODO decrire
    virtual void analyze(const std::string &content,
                         const std::map<std::string, std::string>& metaData,
                         const std::string& pipeline,
                         const std::map<std::string, Lima::AbstractAnalysisHandler*>& handlers = std::map<std::string, Lima::AbstractAnalysisHandler*>(),
                         const std::set<std::string>& inactiveUnits = std::set<std::string>())  const override = 0;

    virtual void setAnalysisHandler(const std::string& handlerId, Lima::AbstractAnalysisHandler *handler) = 0;

    /** deletes the analysis handler with the given id and removes it */
    virtual void releaseAnalysisHandler(const std::string& handlerId) = 0;

    void setMapTagMedia(const std::map<std::string, std::string>& Ids) {
        m_mapTagMedia = Ids;
    };

    void setDefaultMedia(const std::string& media) {
        m_defaultMedia = media;
    };

    void setDocumentPropertyConfiguration(
        Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser* configuration);

    typedef std::map<std::string, std::map<std::string,std::string> > DocPropertyConfigs;
    void getDocumentPropertyConfiguration(
            DocPropertyConfigs& standardPrprtyConfigs,
            DocPropertyConfigs& extendedPrprtyConfigs) const
    {
      standardPrprtyConfigs = m_standardPrprtyConfigs;
      extendedPrprtyConfigs = m_extendedPrprtyConfigs;
    }

    Lima::AbstractProcessingClientHandler m_processingClientHandler;
protected:

    // Associates a tag name to a media id. Initialized at creation time by
    // the factory which loads the values from its configuration
    std::map<std::string, std::string> m_mapTagMedia;

    // If there is no media associated to the current indexing element and this
    // is set, then this media name will be used
    std::string m_defaultMedia;

    DocPropertyConfigs m_standardPrprtyConfigs;
    DocPropertyConfigs m_extendedPrprtyConfigs;
};

/**
 * A factory for the AbstractXmlReaderClient: contains the
 * registration of all implemented clients that are linked with the
 * program. The factory dynamically creates the actual clients from
 * their names.
 */
class AbstractXmlReaderClientFactory : public Lima::RegistrableFactory<AbstractXmlReaderClientFactory>
{
public:

    /** */
    virtual void configure(
        const Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser &configuration,
        const std::string& defaultMedia,
        const std::deque<std::string>& medias,
        const std::deque<std::string>& pipelines) = 0;

    //! @brief* This function create a XmlReader client
    virtual std::shared_ptr< AbstractXmlReaderClient > createClient(Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser *configuration) const = 0;

    //! @brief virtual destructor of the XmlReader client factory
    virtual ~AbstractXmlReaderClientFactory() {};

    std::shared_ptr< Lima::ProcessingClientFactory > getFactoryFromId(const std::string& id) const {
        if(m_mapFactory.find(id) == m_mapFactory.end())
            return 0;
        else
            return m_mapFactory.find(id)->second;
    }
    void setFactoryFromId(const std::string& id, std::shared_ptr< Lima::ProcessingClientFactory > factory) {
        m_mapFactory[id] = factory;

    }

    void setMapTagMedia(const std::map<std::string, std::string>& Ids) {
        m_mapTagMedia = Ids;
    }
//     std::string getMediaFromTag(std::string id) const {return m_mapTagMedia.find(id)->second;}

    void setMapMediaFactory(const std::map<std::string, std::string>& Ids) {
        m_mapMediaFactory = Ids;
    }

    void setDefaultMedia(const std::string& media) {
        m_defaultMedia = media;
    };

//     std::string getFactoryFromMedia(std::string id) const {return m_mapMediaFactory.find(id)->second;}

protected:
    AbstractXmlReaderClientFactory(const std::string &id) :
        Lima::RegistrableFactory<AbstractXmlReaderClientFactory> (id)
    {};
    std::map<std::string, std::shared_ptr< Lima::ProcessingClientFactory > > m_mapFactory;
    std::map<std::string, std::string> m_mapTagMedia;
    std::map<std::string, std::string> m_mapMediaFactory;
    std::string m_defaultMedia;

};

} // XmlProcessing
} // FrCeaLic2m

#endif
