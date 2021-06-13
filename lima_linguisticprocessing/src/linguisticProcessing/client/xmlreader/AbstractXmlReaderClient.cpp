/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST - LIC2M                              *
 *                                                                         *
 ***************************************************************************/

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
  m_standardPrprtyConfigs.clear();
  m_extendedPrprtyConfigs.clear();

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

          std::map< std::string, std::string > docPropertyConfig;
          docPropertyConfig["isInternal"] = "false";
          try {
              docPropertyConfig["storageType"] = propertyGroupConf.getParamsValueAtKey ( "storageType" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LWARN << "no param 'storageType' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["cardinality"] = propertyGroupConf.getParamsValueAtKey ( "cardinality" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'cardinality' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["description"] = propertyGroupConf.getParamsValueAtKey ( "description" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'description' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["isInternal"] = propertyGroupConf.getParamsValueAtKey ( "isInternal" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'isInternal' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          m_standardPrprtyConfigs.insert( std::make_pair(docPropertyName , docPropertyConfig ) );
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
          std::map< std::string, std::string > docPropertyConfig;
          docPropertyConfig["isInternal"] = "false";
          try {
              docPropertyConfig["storageType"] = propertyGroupConf.getParamsValueAtKey ( "storageType" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LWARN << "no param 'storageType' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["cardinality"] = propertyGroupConf.getParamsValueAtKey ( "cardinality" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'cardinality' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["description"] = propertyGroupConf.getParamsValueAtKey ( "description" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'description' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          try {
              docPropertyConfig["isInternal"] = propertyGroupConf.getParamsValueAtKey ( "isInternal" );
          }
          catch ( NoSuchParam& e )
          {
#ifdef DEBUG_LP
              XMLREADERCLIENTLOGINIT;
              LDEBUG << "no param 'isInternal' in DocumentPropertyType group for " << docPropertyName;
#endif
          }
          m_extendedPrprtyConfigs.insert( std::make_pair(docPropertyName , docPropertyConfig ) );
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
