/*
    Copyright 2004-2021 CEA LIST

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

#include "AbstractXmlReaderClient.h"
#include "linguisticProcessing/core/XmlProcessingCommon.h"
#include "linguisticProcessing/core/CoreXmlReaderClient.h"
#include "common/AbstractFactoryPattern/ProcessingClientFactory.h"
#include "XmlReaderException.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace XmlReader
{

void AbstractXmlReaderClient::setDocumentPropertyConfiguration(
    XMLConfigurationFileParser* configuration)
{
  m_standardPrprtyInfos.clear();
  m_extendedPrprtyInfos.clear();

  // Read list of declared standard properties
  try
  {
      GroupConfigurationStructure& groupConf = configuration->getModuleGroupConfiguration("lp-structuredXmlreaderclient",
                                                                         "documentXMLParser" );
      std::deque<std::string> standardPropertyList= groupConf.getListsValueAtKey ( "standard-properties-list" );
      // get list of identifier of attributes from config file
      for ( std::deque<std::string>::const_iterator attributeIdentifier = standardPropertyList.begin();
              attributeIdentifier != standardPropertyList.end(); attributeIdentifier++ )
      {
          const std::string docPropertyName = *attributeIdentifier;
          GroupConfigurationStructure& propertyGroupConf = configuration->getModuleGroupConfiguration("lp-structuredXmlreaderclient",
                                                                                     docPropertyName );
          DocPropertyPublicInfo docPropertyInfo;
          docPropertyInfo.isInternal = false;
          try {
              docPropertyInfo.storageType = propertyGroupConf.getParamsValueAtKey ( "storageType" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LWARN << "no param 'storageType' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyInfo.cardinality = propertyGroupConf.getParamsValueAtKey ( "cardinality" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'cardinality' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyInfo.description = propertyGroupConf.getParamsValueAtKey ( "description" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'description' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              std::string val = propertyGroupConf.getParamsValueAtKey ( "isInternal" );
              if (val=="true" || val=="yes" || val=="1")
                docPropertyInfo.isInternal = true;
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'isInternal' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          m_standardPrprtyInfos.insert( std::make_pair(docPropertyName , docPropertyInfo ) );
      }
  }
  catch ( NoSuchGroup & e )
  {
      XMLREADERCLIENTLOGINIT;
      LWARN << "AbstractXmlReaderClient: NoSuchGroup Exception: " << e.what();
  }
  catch ( NoSuchList & e )
  {
      XMLREADERCLIENTLOGINIT;
      LWARN << "AbstractXmlReaderClient: NoSuchList Exception: standard-properties-list";
  }

  // Read list of specific attribute
  try
  {
      GroupConfigurationStructure& groupConf = configuration->getModuleGroupConfiguration("lp-structuredXmlreaderclient",
                                                                         "documentXMLParser" );
      std::deque<std::string> extendedPropertyList= groupConf.getListsValueAtKey ( "extended-properties-list" );
      // get list of identifier of attributes from config file
      for ( std::deque<std::string>::const_iterator attributeIdentifier = extendedPropertyList.begin();
              attributeIdentifier != extendedPropertyList.end(); attributeIdentifier++ )
      {
          const std::string docPropertyName = *attributeIdentifier;
          GroupConfigurationStructure& propertyGroupConf = configuration->getModuleGroupConfiguration("lp-structuredXmlreaderclient",
                                                                                     docPropertyName );
          DocPropertyPublicInfo docPropertyInfo;
          docPropertyInfo.isInternal = false;
          try {
              docPropertyInfo.storageType = propertyGroupConf.getParamsValueAtKey ( "storageType" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LWARN << "no param 'storageType' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyInfo.cardinality = propertyGroupConf.getParamsValueAtKey ( "cardinality" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'cardinality' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyInfo.description = propertyGroupConf.getParamsValueAtKey ( "description" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'description' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              std::string val = propertyGroupConf.getParamsValueAtKey ( "isInternal" );
              if (val=="true" || val=="yes" || val=="1")
                docPropertyInfo.isInternal = true;
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'isInternal' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          m_extendedPrprtyInfos.insert( std::make_pair(docPropertyName , docPropertyInfo ) );
      }
    }
    catch ( NoSuchGroup & e )
    {
        XMLREADERCLIENTLOGINIT;
        LWARN << "AbstractXmlReaderClient: NoSuchGroup Exception: " << e.what();
    }
    catch ( NoSuchList & e )
    {
        XMLREADERCLIENTLOGINIT;
        LWARN << "AbstractXmlReaderClient: NoSuchList Exception: extended-properties-list";
    }


}

} // XmlReader
} // Lima
