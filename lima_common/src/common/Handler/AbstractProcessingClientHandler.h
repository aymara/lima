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
    
    inline virtual void setAnalysisClient(const std::string& clientId, AbstractProcessingClient* client)
    {
        if (m_clients.find(clientId)!=m_clients.end())
        {
//             ABSTRACTPROCESSINGCLIENTLOGINIT;
//             LERROR << "Handler for handlerId '" << handlerId << "' already exists !" << LENDL;
//             throw LimaException();
            m_clients.erase(clientId);
        }
        m_clients.insert(std::make_pair(clientId, client));
    }
    
    inline virtual AbstractProcessingClient* getAnalysisClient(const std::string& clientId)
    {
        if (m_clients.find(clientId)==m_clients.end())
        {
             ABSTRACTPROCESSINGCLIENTLOGINIT;
//             LERROR << "Handler for handlerId '" << handlerId << "' already exists !" << LENDL;
//             throw LimaException();
	    LERROR << "AbstarctProcessingHandler::no Client for clientId '" << clientId << LENDL;	
            throw LimaException();
	    return NULL;
        }
        else
	  return m_clients[clientId];
    }    
    
    inline virtual std::map<std::string, AbstractProcessingClient*> getAnalysisClients() const {return m_clients;};
    inline virtual void setAnalysisClients(std::map<std::string, AbstractProcessingClient*> clients){m_clients=clients;};   
    
   virtual void handleProc(	 const std::string& tagName,
			 const std::string& content,
                         const std::map<std::string,std::string>& metaData,
                         const std::string& pipeline,
			  const std::map<std::string, AbstractAnalysisHandler*>& handlers = std::map<std::string, AbstractAnalysisHandler*>(),
                         const std::set<std::string>& inactiveUnits = std::set<std::string>())
	{
    ABSTRACTPROCESSINGCLIENTLOGINIT;
    LDEBUG << "handleProc("<<tagName<<") gets " << (void*)getAnalysisClient(tagName) <<" class: "<<typeid(*getAnalysisClient(tagName)).name()<< LENDL;
	  getAnalysisClient(tagName)->analyze(content, metaData,pipeline,handlers,inactiveUnits);
	}

//   inline virtual void setAnalysisHandler(const std::string& handlerId, AbstractAnalysisHandler* handler)
//   {
//     std::map<std::string, AbstractProcessingClient*>::iterator ItrClients = m_clients.begin();
//     for (;ItrClients!=m_clients.end();ItrClients++)
//     {
//       ItrClients->second->setAnalysisHandler(handlerId,handler);
//     }
//   }
	

  private:
  //! @brief list of handlers available
  std::map<std::string, AbstractProcessingClient*> m_clients;
};

}

#endif
