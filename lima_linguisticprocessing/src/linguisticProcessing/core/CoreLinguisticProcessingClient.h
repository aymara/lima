// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSINGCORELINGUISTICPROCESSINGCLIENT_H
#define LIMA_LINGUISTICPROCESSINGCORELINGUISTICPROCESSINGCLIENT_H

#include "CoreLinguisticProcessingClientExport.h"
#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <list>

namespace Lima
{

namespace LinguisticProcessing
{

/**
@author Benoit Mathieu
*/
class LIMA_CORELINGUISTICPROCESSINGCLIENT_EXPORT CoreLinguisticProcessingClient : public AbstractLinguisticProcessingClient
{
public:
  CoreLinguisticProcessingClient();

  virtual ~CoreLinguisticProcessingClient();

  void analyze(const LimaString& texte,
               const std::map<std::string,std::string>& metaData,
               const std::string& pipeline,
               const std::map<std::string, AbstractAnalysisHandler*>& handlers,
               const std::set<std::string>& inactiveUnits = std::set<std::string>()) const
   override;

  void analyze(const std::string& texte,
               const std::map<std::string,std::string>& metaData,
               const std::string& pipeline,
               const std::map<std::string, AbstractAnalysisHandler*>& handlers,
               const std::set<std::string>& inactiveUnits = std::set<std::string>()) const override
  ;
};

class CoreLinguisticProcessingClientFactory : public AbstractLinguisticProcessingClientFactory
{

public:

  void configure(
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs,
    std::deque<std::string> pipelines) override;

  std::shared_ptr< AbstractProcessingClient > createClient() const override;

  virtual ~CoreLinguisticProcessingClientFactory();

private:
  CoreLinguisticProcessingClientFactory();
  static std::unique_ptr<CoreLinguisticProcessingClientFactory> s_instance;

};


} // LinguisticProcessing

} // Lima

#endif
