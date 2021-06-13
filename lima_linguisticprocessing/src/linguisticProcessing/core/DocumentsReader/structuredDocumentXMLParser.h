/************************************************************************
 * @file   StructuredDocumentXMLParser.h
 * @author Olivier Mesnard
 * @date   ??
 * @author Romaric Besancon
 * @date Tue Oct 28 18:29:19 2003
 * @author Gael de Chalendar
 * @date Wed Apr 08 2015
 ***********************************************************************/
#ifndef STRUCTUREDDOCUMENTXMLPARSER_H
#define STRUCTUREDDOCUMENTXMLPARSER_H

#include "documentsreader_export.h"
#include "linguisticProcessing/core/abstractReaderResource.h"
#include "DocumentElements.h"

#include <boost/shared_ptr.hpp>

class QXmlStreamAttributes;

namespace Lima {

class StructuredXmlDocumentHandler;

namespace DocumentsReader {

#define MAX_NODE_TYPE NODE_PROPERTY + 1
static const QString tagSemanticItem[MAX_NODE_TYPE] =
{
    "",
    "rootNodes",
    "discardableNodes",
    "indexingNodes",
    "hierarchyNodes",
    "presentationNodes",
    "ignoredNodes",
    "propertyNodes"
};

class DOCUMENTSREADER_EXPORT FieldTypeElement : public std::pair<FieldType,QString>
{
    public:
        FieldTypeElement() :std::pair<FieldType,QString> ( NODE_NOTYPE,"" ) {}
        FieldTypeElement ( const FieldType type, const QString attribute ) :
                std::pair<FieldType,QString> ( type,attribute ) {}
        FieldTypeElement ( const FieldType type ) :
                std::pair<FieldType,QString> ( type,"" ) {}
        const FieldType& getType ( void ) const { return first; }
        const QString& getAttributeName ( void ) const { return second; }
        FieldType& getType ( void ) { return first; }
        QString& getAttribute ( void ) { return second; }
        bool inAttribute ( void ) const { return ( !second.isEmpty() ); }
};

#define STRUCTUREDDOCUMENTXMLPARSER_CLASSID "StructuredDocumentXMLParser"

/**
 * @brief XML parser for source structured documents. Tags and attributes semantics are read in configuration files.
 */
class DOCUMENTSREADER_EXPORT StructuredDocumentXMLParser : public AbstractReaderResource
{
public:

  StructuredDocumentXMLParser();

  virtual ~StructuredDocumentXMLParser ( void );

  // -----------------------------------------------------------------------
  //  propagate SAX events as XMLDocHandler events
  // -----------------------------------------------------------------------
  void setLinguisticXMLDocHandler ( StructuredXmlDocumentHandler* handler )
  {
      m_processor = handler;
  }

  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool startDocument(unsigned int parserOffset);

  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlStreamAttributes & attributes, unsigned int parserOffset);

  bool characters ( const QString & ch, unsigned int parserOffset );

  bool endElement(const QString& namespaceURI, const QString& qsname, const QString& qName, unsigned int parserOffset);

  bool endDocument();

  //----------------------------------------------------------------------
  // private function members
private:
  void setCurrentByteOffset ( const unsigned int offset );

  /**
    * function called when opening document tag is encountered
    */
//         void initElement();
  void initElementOffset ( const QString& name );
  void closeElementOffset ( const QString& name );

  bool isIndexing ( const QString& elementName ) const;
  bool isHierarchy ( const QString& elementName ) const;
  bool isMetaData ( const QString& elementName ) const;
  bool hasMetaData ( const QString& elementName ) const;
  bool isRoot ( const QString& elementName ) const;
  bool isDiscardable ( const QString& elementName ) const;
  DocumentPropertyType getMetaDataFromName ( const QString& elementName ) const;

  void readFieldTags ( Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf,
                        const FieldType type,
                        std::vector<std::pair<QString,FieldType> >& fields );

  void addField ( const QString& fieldName, const FieldType type );

  // to deal with special characters
  bool isSpecialCharacter ( const Lima::LimaChar c );
  unsigned int getSpecialCharSize ( const Lima::LimaChar c );

  void readPropertiesMetadata (
      Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf ) ;


  // helper function for initialization when created
  void init (
      Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager ) override;

  // helper function for test functions on fields (isXXX)
  bool isFieldType ( const QString& elementName, const FieldType& type ) const;
  // helper function for debug messages
  static QString fieldTypeString ( const FieldType type );
  // static function to parse dates
  static void parseDate ( const std::string& dateStr,
                          QDate& dateBegin,
                          QDate& dateEnd );


  // Handler of high level Document events (startIndexingNode, processText..)
  StructuredXmlDocumentHandler* m_processor;
  boost::shared_ptr< ContentStructuredDocument > m_currentDocument;
  bool m_elementPointerHasBeenReturned;

  std::multimap<QString,FieldTypeElement> m_fields;
  // to get property type from element name
  std::map<QString,DocumentPropertyType> m_elementTag2DocumentPropertyType;
  // to get property type from element name
  std::multimap<QString,DocumentPropertyType> m_elementTagOfPair2DocumentPropertyType;
  // to deal with special characters (XML entities)
  std::map<Lima::LimaChar,unsigned int> m_specialCharacterSize;

  bool m_addAbsoluteOffsetToTokens;
};

} // namespace DocumentsReader
} // namespace Lima

#endif
