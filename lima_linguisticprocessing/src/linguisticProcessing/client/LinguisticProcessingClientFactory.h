// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGLINGUISTICPROCESSINGCLIENTFACTORY_H
#define LIMA_LINGUISTICPROCESSINGLINGUISTICPROCESSINGCLIENTFACTORY_H

#include "LinguisticProcessingClientExport.h"
#include "AbstractLinguisticProcessingClient.h"
#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/AbstractFactoryPattern/ProcessingClientFactory.h"

namespace Lima
{

namespace LinguisticProcessing
{

/**
* LinguisticProcessingClientFactory is a singleton. It allows configuration 
* and creation of linguisticProcessing clients.
* @brief main entry point of LinguisticProcessing Component
*/
class LIMA_LINGUISTICPROCESSIONGCLIENT_EXPORT LinguisticProcessingClientFactory : public ProcessingClientFactory,public Singleton<LinguisticProcessingClientFactory>
{
friend class Singleton<LinguisticProcessingClientFactory>;

public:

  /**
    * @brief configure the corresponding clientFactory so that it can create clients.
    * @param id @b IN : id of clientFactory to configure
    * @throw LimaException if clientFactory does not exist, or if an \
    *  exception occurs during configuration.
    */
  void configureClientFactory(
    const std::string& id,
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs = std::deque<std::string>(),
    std::deque<std::string> pipelines = std::deque<std::string>()) override ;

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
  std::shared_ptr< AbstractProcessingClient > createClient(const std::string& id) const override;

  /**
    * @brief show registered clientId
    * @return list of registered clientId
    */
  std::deque<std::string> getRegisteredFactories() const;

  ~LinguisticProcessingClientFactory() {};
    
  LinguisticProcessingClientFactory() {};

};

class LIMA_LINGUISTICPROCESSIONGCLIENT_EXPORT LinguisticProcessingClientFactoryFactory : public AbstractProcessingClientFactoryFactory
{
  public:
  ~LinguisticProcessingClientFactoryFactory(){};
  
  std::shared_ptr< ProcessingClientFactory > createProcessingClientFactory() const override
  {
      return std::shared_ptr< ProcessingClientFactory >(new LinguisticProcessingClientFactory());
  }
  
  private:
  LinguisticProcessingClientFactoryFactory():AbstractProcessingClientFactoryFactory("lpFactory"){};
  static std::unique_ptr< LinguisticProcessingClientFactoryFactory > s_instance;
};


} // LinguisticProcessing

} // Lima

#endif
