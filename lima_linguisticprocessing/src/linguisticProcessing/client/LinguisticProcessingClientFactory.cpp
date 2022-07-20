// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2019 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include <common/AbstractFactoryPattern/InitializableObjectFactory.h>
#include <iostream>
namespace Lima
{

namespace LinguisticProcessing
{

void LinguisticProcessingClientFactory::configureClientFactory(
  const std::string& id,
  Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
  std::deque<std::string> langs,
  std::deque<std::string> pipelines)
{
  LPCLIENTFACTORYLOGINIT;
  LINFO << "LinguisticProcessingClientFactory::configureClientFactory" << id;
  if (id.empty())
  {
    LIMA_LP_EXCEPTION("LinguisticProcessingClientFactory::configureClientFactory Trying to access to factory with empty name!");
  }
  std::shared_ptr<AbstractLinguisticProcessingClientFactory> factory=
    AbstractLinguisticProcessingClientFactory::getFactory(id);
  if (factory==0) {
    LERROR << "No factory defined for client '" << id << "' !";
    LIMA_LP_EXCEPTION("No factory defined for given client");
  }
  std::deque<std::string>::iterator Itrlangs;
//     for (  Itrlangs=langs.begin();  Itrlangs!=langs.end();Itrlangs++)
      //std::cout<<"ici la langue : "<<*Itrlangs<<std::endl;
  factory->configure(
    configuration,
    langs,
    pipelines);
}

std::shared_ptr< AbstractProcessingClient > LinguisticProcessingClientFactory::createClient(
  const std::string& id) const
{
  LPCLIENTFACTORYLOGINIT;
  LINFO << "Create client '" << id << "'";
  if (id.empty())
  {
    LIMA_LP_EXCEPTION("LinguisticProcessingClientFactory::createClient Trying to create client with empty name!");
  }
  const std::shared_ptr<AbstractLinguisticProcessingClientFactory> factory=
    AbstractLinguisticProcessingClientFactory::getFactory(id);
  if (factory==0) {
    LIMA_LP_EXCEPTION("No factory defined for given client");
  }

  return factory->createClient();
}


std::deque<std::string> LinguisticProcessingClientFactory::getRegisteredFactories() const
{
  return AbstractLinguisticProcessingClientFactory::getRegisteredFactories();
}


std::unique_ptr< LinguisticProcessingClientFactoryFactory > LinguisticProcessingClientFactoryFactory::s_instance=std::unique_ptr< LinguisticProcessingClientFactoryFactory >(new LinguisticProcessingClientFactoryFactory());

} // LinguisticProcessing

} // Lima
