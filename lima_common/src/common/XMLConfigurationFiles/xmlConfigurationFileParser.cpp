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
/**
  * @file       xmlConfigurationFileParser.cpp
  * @brief      originally in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */

#include "common/LimaCommon.h"

#include "xmlConfigurationFileExceptions.h"
#include "xmlConfigurationFileHandler.h"
#include "xmlConfigurationFileParser.h"
#include "configurationStructure.h"

#include <QtXml/QXmlSimpleReader>

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <iostream>
#include <string>

using namespace std;

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class XMLConfigurationFileParserPrivate
{
    friend class XMLConfigurationFileParser;
    friend std::ostream& operator<<(std::ostream& os, XMLConfigurationFileParser& parser);

    XMLConfigurationFileParserPrivate() {};
    XMLConfigurationFileParserPrivate(const XMLConfigurationFileParserPrivate& p);
    XMLConfigurationFileParserPrivate(const std::string &configurationFileName);
    ~XMLConfigurationFileParserPrivate();

    ConfigurationStructure m_configuration;
    QXmlSimpleReader* m_parser;
    std::string m_configurationFileName;
};

XMLConfigurationFileParserPrivate::XMLConfigurationFileParserPrivate(const XMLConfigurationFileParserPrivate& p)
{
    m_configuration = p.m_configuration;
    m_configurationFileName = p.m_configurationFileName;
//     m_parser = p.m_parser; // TODO sale: SAXParser::operator= n'est pas surchargé. Mais peut marcher...
    m_parser = new QXmlSimpleReader();
    XMLConfigurationFileHandler handler(m_configuration);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
    QFile file(m_configurationFileName.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
      XMLCFGLOGINIT;
      LERROR << "XMLConfigurationFileParser unable to open" << QString::fromUtf8(m_configurationFileName.c_str());
      throw XMLException(std::string("XMLConfigurationFileParser Unable to open ") + m_configurationFileName.c_str());
    }
    if (!m_parser->parse( QXmlInputSource(&file)))
    {
      XMLCFGLOGINIT;
      LERROR << "XMLConfigurationFileParser unable to parse" << QString::fromUtf8(m_configurationFileName.c_str()) << ":" << m_parser->errorHandler()->errorString();
      throw XMLException(std::string("XMLConfigurationFileParser Unable to parse ") + m_configurationFileName + " : " + m_parser->errorHandler()->errorString().toUtf8().constData());
    }
}

XMLConfigurationFileParserPrivate::XMLConfigurationFileParserPrivate(const string &configurationFileName) :
    m_parser(0), m_configurationFileName(configurationFileName)
{
    XMLCFGLOGINIT;
    LINFO << "XMLConfigurationFileParser creating parser for: " << configurationFileName.c_str();

    m_parser = new QXmlSimpleReader();

    //
    //  Create the handler object and install it as the document and error
    //  handler for the parser-> Then parse the file and catch any exceptions
    //  that propogate out
    //
    XMLConfigurationFileHandler handler(m_configuration);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
    QFile file(m_configurationFileName.c_str());
    if (!file.open(QFile::ReadOnly))
    {
      XMLCFGLOGINIT;
      LERROR << "Error opening " << m_configurationFileName.c_str();
      throw XMLException(std::string("XMLConfigurationFileParser Unable to open ") + m_configurationFileName);
    }
    if (!m_parser->parse( QXmlInputSource(&file)))
    {
      LERROR << "Error parsing " << m_configurationFileName.c_str();
      throw XMLException(std::string("XMLConfigurationFileParser Unable to parse ") + m_configurationFileName + " : " + m_parser->errorHandler()->errorString().toUtf8().constData());
    }
    {
      LOGINIT("FilesReporting");
      LINFO << "File parsed:" << m_configurationFileName;
    }
    
}

XMLConfigurationFileParserPrivate::~XMLConfigurationFileParserPrivate()
{
    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete m_parser;
}

XMLConfigurationFileParser::XMLConfigurationFileParser() :
    m_d(new XMLConfigurationFileParserPrivate())
{
}

XMLConfigurationFileParser::XMLConfigurationFileParser(const XMLConfigurationFileParser& p) :
    m_d(new XMLConfigurationFileParserPrivate(*p.m_d))
{
}

XMLConfigurationFileParser::XMLConfigurationFileParser(const string &configurationFileName) :
    m_d(new XMLConfigurationFileParserPrivate(configurationFileName))
{
}

XMLConfigurationFileParser::~XMLConfigurationFileParser()
{
  delete m_d;
}

ConfigurationStructure& XMLConfigurationFileParser::getConfiguration(void)
{
    return m_d->m_configuration;
}

ModuleConfigurationStructure& XMLConfigurationFileParser::getModuleConfiguration(const string& moduleName)
{

    ConfigurationStructure::iterator it = m_d->m_configuration.find(moduleName);

    if (it == m_d->m_configuration.end())
    {
      XMLCFGLOGINIT;
      LDEBUG << "XMLConfigurationFileParser::getModuleConfiguration no such module" << moduleName << "in" << m_d->m_configurationFileName.c_str();
      throw NoSuchModule(m_d->m_configurationFileName+":["+moduleName+"]");
    }
    return (*it).second;
}

GroupConfigurationStructure& XMLConfigurationFileParser::getModuleGroupConfiguration(const string& moduleName,const string& groupName)
{
    return (getModuleConfiguration(moduleName).getGroupNamed(groupName));
}

string& XMLConfigurationFileParser::getModuleGroupParamValue(const string& moduleName,const string& groupName,const string& key)
{
    XMLCFGLOGINIT;
    try
    {
        return (getModuleConfiguration(moduleName).getParamValueAtKeyOfGroupNamed(key, groupName));
    }
    catch(NoSuchModule& nsm)
    {
      std::cerr << nsm.what() << " " << m_d->m_configurationFileName.c_str() << std::endl;
      LWARN << nsm.what() << " " << m_d->m_configurationFileName.c_str();
        //not LERROR because user may want the module to be optional -> no error
        throw NoSuchModule(m_d->m_configurationFileName+":["+moduleName+"]["+groupName+"]["+key+"]");
    }
    catch(NoSuchGroup& nsg)
    {
      std::cerr << nsg.what() << " " << m_d->m_configurationFileName.c_str() << std::endl;
      LWARN << nsg.what() << " " << m_d->m_configurationFileName.c_str();
        throw NoSuchGroup(m_d->m_configurationFileName+":["+moduleName+"]["+groupName+"]["+key+"]");
    }
    catch(NoSuchParam& nsp)
    {
      std::cerr << nsp.what() << " " << m_d->m_configurationFileName.c_str() << std::endl;
      LWARN << nsp.what() << " " << m_d->m_configurationFileName.c_str();
        throw NoSuchParam(m_d->m_configurationFileName+":["+moduleName+"]["+groupName+"]["+key+"]");
    }
    catch(...)
    {
        throw;
    }
}

deque< string >& XMLConfigurationFileParser::getModuleGroupListValues(
    const string& moduleName,
    const string& groupName,
    const string& key)
{
  return (getModuleConfiguration(moduleName).getListValuesAtKeyOfGroupNamed(
            key, groupName));
}

const std::string& XMLConfigurationFileParser::getConfigurationFileName() const
{
  return m_d->m_configurationFileName;
}

std::ostream& operator<<(std::ostream& os, XMLConfigurationFileParser& parser)
{
    return (os << parser.m_d->m_configurationFileName << ":" 
                << parser.m_d->m_configuration);
}

} // closing namespace XMLConfigurationFiles
} // closing namesapce Common
} // closing namespace Lima
