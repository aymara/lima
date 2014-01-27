/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
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
  ;

  void analyze(const std::string& texte,
               const std::map<std::string,std::string>& metaData,
               const std::string& pipeline,
               const std::map<std::string, AbstractAnalysisHandler*>& handlers,
               const std::set<std::string>& inactiveUnits = std::set<std::string>()) const
  ;
};

class CoreLinguisticProcessingClientFactory : public AbstractLinguisticProcessingClientFactory
{

public:
  
  void configure(
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs,
    std::deque<std::string> pipelines);

  AbstractLinguisticProcessingClient* createClient() const;

  virtual ~CoreLinguisticProcessingClientFactory();

private:
  CoreLinguisticProcessingClientFactory();
  static CoreLinguisticProcessingClientFactory* s_instance;

};


} // LinguisticProcessing

} // Lima

#endif
