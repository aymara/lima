// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_ABSTRACTPROCESSINGCLIENTFACTORY_H
#define LIMA_ABSTRACTPROCESSINGCLIENTFACTORY_H

#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/AbstractProcessingClient/AbstractProcessingClient.h"
#include <string>
#include <deque>
#include "common/Data/LimaString.h"
#include "common/AbstractProcessingClient/AbstractProcessingClient.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"

namespace Lima
{

/**
* LinguisticProcessingClientFactory is a singleton. It allows configuration 
* and creation of linguisticProcessing clients.
* @brief main entry point of LinguisticProcessing Component
*/
class ProcessingClientFactory 
{

public:

  /**
    * @brief configure the corresponding clientFactory so that it can create clients.
    * @param id @b IN : id of clientFactory to configure
    * @throw LimaException if clientFactory does not exist, or if an \
    *  exception occurs during configuration.
    */
  virtual void configureClientFactory(
    const std::string& id,
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs = std::deque<std::string>(),
    std::deque<std::string> pipelines = std::deque<std::string>()) = 0;

  /**
    * @brief create an Client using the appropriate registered factory.
    * @param id @b IN : classId for the factory
    * @return Client* pointer to the created Client.\
    * Pointer should be deleted when no more used
    * @throw LimaException 
    *
    * ClientFactory must have been configured before this method is called
    * Use configureClientFactory() method to configure.
    */
  virtual std::shared_ptr< AbstractProcessingClient > createClient(const std::string& id) const = 0;

  /**
    * @brief show registered clientId
    * @return list of registered clientId
    */
  std::deque<std::string> getRegisteredFactories() const;

  virtual ~ProcessingClientFactory() {};
  
  ProcessingClientFactory() {};

private:


};

class ProcessingClientFactoryFactory:  public Singleton<ProcessingClientFactoryFactory> 
{
    friend class Singleton<ProcessingClientFactoryFactory> ;

public:
    virtual ~ProcessingClientFactoryFactory() {};
    std::shared_ptr< ProcessingClientFactory > createProcessingClientFactory(const std::string& id) const ;
private:
    ProcessingClientFactoryFactory() {};  
};

class AbstractProcessingClientFactoryFactory:public RegistrableFactory<AbstractProcessingClientFactoryFactory>
{
public:
    virtual ~AbstractProcessingClientFactoryFactory() {};
    virtual std::shared_ptr< ProcessingClientFactory > createProcessingClientFactory() const = 0;
protected:
    AbstractProcessingClientFactoryFactory(const std::string& id): RegistrableFactory<AbstractProcessingClientFactoryFactory>(id)
    {};
};


} // Lima

#endif
