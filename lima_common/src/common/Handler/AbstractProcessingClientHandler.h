// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ABSTRACTPROCESSINGCLIENTHANDLER_H
#define ABSTRACTPROCESSINGCLIENTHANDLER_H

#include <sstream>
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/AbstractProcessingClient/AbstractProcessingClient.h"
#include "common/Handler/AbstractAnalysisHandler.h"

namespace Lima {

class AbstractProcessingClientHandler
{
  public:
    virtual ~AbstractProcessingClientHandler() {}

    inline virtual void setAnalysisClient(const std::string& clientId,
                                          std::shared_ptr< AbstractProcessingClient > client)
    {
      if (m_clients.find(clientId)!=m_clients.end())
      {
//         ABSTRACTPROCESSINGCLIENTLOGINIT;
//         LERROR << "Handler for handlerId '" << handlerId << "' already exists !" ;
//         throw LimaException();
        m_clients.erase(clientId);
      }
      m_clients.insert(std::make_pair(clientId, client));
    }

    inline virtual std::shared_ptr< AbstractProcessingClient > getAnalysisClient(const std::string& clientId)
    {
      if (m_clients.find(clientId)==m_clients.end())
      {
        ABSTRACTPROCESSINGCLIENTLOGINIT;
        LIMA_EXCEPTION("AbstarctProcessingHandler::no Client for clientId"
                       << clientId.c_str());
      }
      return m_clients[clientId];
    }

    inline virtual std::map<std::string, std::shared_ptr< AbstractProcessingClient > > getAnalysisClients() const
    {
      return m_clients;
    };
    inline virtual void setAnalysisClients(std::map<std::string, std::shared_ptr< AbstractProcessingClient > > clients)
    {
      m_clients=clients;
    };

  virtual void handleProc(const std::string& tagName,
                          const std::string& content,
                          const std::map<std::string,std::string>& metaData,
                          const std::string& pipeline,
                          const std::map<std::string, AbstractAnalysisHandler*>& handlers = std::map<std::string, AbstractAnalysisHandler*>(),
                          const std::set<std::string>& inactiveUnits = std::set<std::string>())
  {
    ABSTRACTPROCESSINGCLIENTLOGINIT;
    auto& r = *getAnalysisClient(tagName).get();
    LDEBUG << "handleProc("<<tagName<<") gets "
            << (void*)getAnalysisClient(tagName).get() <<" class: "
            << typeid(r).name();
    getAnalysisClient(tagName)->analyze(content,
                                        metaData,
                                        pipeline,
                                        handlers,
                                        inactiveUnits);
  }

private:
  //! @brief list of handlers available
  std::map<std::string, std::shared_ptr< AbstractProcessingClient > > m_clients;
};

}

#endif
