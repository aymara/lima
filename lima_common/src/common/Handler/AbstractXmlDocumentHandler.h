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
/******************************************************************************
 * File        : abstractXmlDocumentHandler.cpp
 * Author      : CEA LIST
 * Created on  : janvier 2010
 * Copyright   : (c) 2010 by CEA
 * brief       : Manipulateur de données XML
 ******************************************************************************/
#ifndef ABSTRACTXMLDOCUMENTHANDLER_H
#define ABSTRACTXMLDOCUMENTHANDLER_H

#include <sstream>
#include <vector>
#include "common/Handler/structureHandler.h"
#include "common/Handler/contentHandler.h"
#include "common/Handler/AbstractProcessingClientHandler.h"
#include "common/Data/genericDocumentProperties.h"
#include "common/Handler/AbstractXmlAnalysisHandler.h"


namespace Lima {

/**
 * @brief abstract handler for XML processing : just inherits from different handlers
 */
class AbstractXmlDocumentHandler :
            public StructureHandler,
            public AbstractXmlAnalysisHandler,
            public AbstractProcessingClientHandler,
            public ContentHandler<Common::Misc::GenericDocumentProperties>,  
	    public ContentHandler< std::vector<float> >
{
public:
    AbstractXmlDocumentHandler():
            StructureHandler(),
            AbstractXmlAnalysisHandler(),
            AbstractProcessingClientHandler(),
//             AbstractTextualAnalysisHandler(),
            ContentHandler<Common::Misc::GenericDocumentProperties>()
    {}

    virtual ~AbstractXmlDocumentHandler() {};

    int get_parentlastOpenedNode() const {
        return m_parentlastOpenedNode;
    };

    void set_parentlastOpenedNode ( int nodeId ) {
        m_parentlastOpenedNode = nodeId;
    };

    void set_lastNodeId(NODE_ID NID)
    {
        m_lastNode=NID;
    };

    uint64_t get_lastNodeId() const
    {
        return m_lastNode;
    };

    std::string get_lastUri()
    {
        return m_metadata["LastUri"];
    };

    void set_lastUri(std::string uri)
    {
        m_metadata["LastUri"]=uri;
    };
    
    void set_lang(std::string lang)
    {
        m_metadata["Lang"]=lang;
    };
    
    std::string get_lang() const
    {
      if (m_metadata.find("Lang")!=m_metadata.end())
        return m_metadata.find("Lang")->second;
      else
        return ""; //should not suppose a default language. Weird bugs otherwise.
    };

//     std::map<std::string, std::map<int , std::map<int, std::vector< std::pair<int, std::string> > > > >* getToIndex() {
//         return &m_mapToIndex;
//     };
// 
//     void newToIndex() {
//         m_mapToIndex.erase(m_mapToIndex.begin(),m_mapToIndex.end());
//     };
// 
//     std::map<std::string, std::map<int , std::map<int, std::vector< std::pair<int, std::string> > > > > m_mapToIndex;

    virtual void startAnalysis(std::string bloc_type){(void)bloc_type;};

    std::vector<uint64_t> m_openedNodes;
    std::vector<uint64_t>::iterator m_lastOpenedNode;
    uint64_t m_parentlastOpenedNode;
    uint64_t m_lastNode;
    std::map<std::string,std::string> m_metadata;
};

}

#endif
