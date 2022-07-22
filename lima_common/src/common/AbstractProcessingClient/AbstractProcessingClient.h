// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_ABSTRACTPROCESSINGCLIENT_H
#define LIMA_ABSTRACTPROCESSINGCLIENT_H

#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include <set>
#include <deque>

namespace Lima
{

//! @brief mother class of (abstract) clients that analyse content.
class AbstractProcessingClient
{
public:

    //! @brief Define the destructor virtual to ensure concrete client destructors to be called
    virtual ~AbstractProcessingClient() {}

    //! @brief analyze an image, given the pipeline and the expected resultType
    //! @param content path of the file or text to analyze in string format
    //! @param metaData additive information
    //! @param pipeline analysis pipeline to use (an analysis pipeline is 

    //!                      a chain of processUnit)
    //! @param inactiveUnits ??? (un truc pour les texteux)
    virtual void analyze(const std::string& content,
                         const std::map<std::string,std::string>& metaData,
                         const std::string& pipeline,
                         const std::map<std::string, AbstractAnalysisHandler*>& handlers,
                         const std::set<std::string>& inactiveUnits = std::set<std::string>()) const = 0;

};


/**
 * A factory for the AbstractLinguisticProcessingClient: contains the
 * registration of all implemented clients that are linked with the
 * program. The factory dynamically creates the actual clients from
 * their names.
 */
class AbstractProcessingClientFactory
{
public:

  /**
   * This function configures the factory, using an XML configuration file
   * S2-lp.xml
   *
   * @param configuration the result of the parsing of the XML
   *                      configuration file
   *
   * @param langs the languages to configure: several languages may be
   *              configured in the same linguistic analyzer client,
   *              the actual language of the analysis is given in the
   *              analyze() function of the client.
   *
   * @param pipelines the pipelines to configure: several pipelines
   *                  may be configured in the same linguistic
   *                  analyzer client, the actual language of the
   *                  analysis is given in the analyze() function of
   *                  the client. Each pipeline configured will
   *                  initialize all needed processing units and the
   *                  corresponding linguistic resources.
   *
   */
  virtual void configure(
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs,
    std::deque<std::string> pipelines) = 0;

  /**
   * This function create a LinguisticProcessing client 
   */
  virtual std::shared_ptr< AbstractProcessingClient > createClient() const = 0;

  /**
   * virtual destructor of the LinguisticProcessing client factory
   */
  virtual ~AbstractProcessingClientFactory() {};

protected:
  AbstractProcessingClientFactory(const std::string& /*id*/) {};

};

}

#endif
