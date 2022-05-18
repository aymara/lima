// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @file       xmlConfigurationFileHandler.h
  * @brief      originally in detectlibraries
  * @date       begin Mon Oct, 13 2003 (ven oct 18 2002)
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             copyright (C) 2002-2003 by CEA
  */

#ifndef XMLCONFIGURATIONFILEHANDLER_H
#define XMLCONFIGURATIONFILEHANDLER_H

#include "common/LimaCommon.h"
#include "configurationStructure.h"

#include <QtXml/QXmlDefaultHandler>

#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

/**This is a SAX document handler the  configuration files
  *@author Gael de Chalendar
  */

class LIMA_XMLCONFIGURATIONFILES_EXPORT XMLConfigurationFileHandler : public QXmlDefaultHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors
    // -----------------------------------------------------------------------
    XMLConfigurationFileHandler(    ConfigurationStructure& theConfiguration);
    virtual ~XMLConfigurationFileHandler();

    bool error(const QXmlParseException& e) override;
    bool fatalError(const QXmlParseException& e) override;
    bool warning(const QXmlParseException& e) override;
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    bool endElement(const QString & namespaceURI, const QString & name, const QString & qName) override;
    bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes) override;

private:
    /*
     * The current module group and, possibly, list
     */
    std::string m_moduleName;
    std::string m_groupName;
    std::string m_listName;
    std::string m_mapName;
    
    // bools to handle items with attributes
    bool m_firstItem;
    bool m_itemWithAttributes;

    /*
     * The structure where data will be stored
     */
    ConfigurationStructure& m_configuration;

    /// utility conversion function
    std::string toString(const QString& str);
};

} // closing namespace XMLConfigurationFiles
} // closing namespace Common
} // closing namespace Lima

#endif
