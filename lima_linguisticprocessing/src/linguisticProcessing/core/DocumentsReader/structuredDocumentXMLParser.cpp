/************************************************************************
 * @file   StructuredDocumentXMLParser.cpp
 * @author Olivier Mesnard
 * @date   ??
 *         modified by Romaric Besancon on Tue Oct 28 18:29:19 2003
 *
 *
 ***********************************************************************/
#include "structuredDocumentXMLParser.h"
#include "documentReaderResources.h"
#include "contentDocument.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"

#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "linguisticProcessing/core/XmlProcessingCommon.h"

#include <QXmlStreamAttributes>

#include <iostream>
#include <string>
#include <deque>
#include <cassert>

using namespace std;

using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;

 // to handle dates (parsing)

namespace Lima {
namespace DocumentsReader {


Lima::SimpleFactory<AbstractReaderResource,
Lima::DocumentsReader::StructuredDocumentXMLParser>
structuredDocumentXMLParserFactory ( STRUCTUREDDOCUMENTXMLPARSER_CLASSID );

StructuredDocumentXMLParser::StructuredDocumentXMLParser() :
        m_processor(nullptr),
        m_currentDocument(),
        m_elementPointerHasBeenReturned(false),
        m_fields(),
        m_addAbsoluteOffsetToTokens(true)
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser::StructuredDocumentXMLParser()";
#endif
}

StructuredDocumentXMLParser::~StructuredDocumentXMLParser()
{
}

void StructuredDocumentXMLParser::setShiftFrom(const QMap< uint64_t,uint64_t >* shiftFrom)
{
  m_shiftFrom = shiftFrom;
}

void StructuredDocumentXMLParser::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* )
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser::init(): readPropertiesMetadata ...";
#endif
    readPropertiesMetadata ( unitConfiguration ) ;

    // Read list of tag for identified fields in document
    vector<pair<QString,FieldType> > fields;
    for ( unsigned int f=0; f < MAX_NODE_TYPE; f++ )
    {
#ifdef DEBUG_LP
        LDEBUG << "StructuredDocumentXMLParser::init(): readFieldTags " << f;
#endif
        readFieldTags ( unitConfiguration,static_cast<FieldType> ( f ),fields );
    }

    for ( vector<pair<QString,FieldType> >::const_iterator tag=fields.begin();
            tag!=fields.end(); tag++ )
    {
#ifdef DEBUG_LP
        LDEBUG << "StructuredDocumentXMLParser::init(): addField "
        << ( *tag ).first << " to list of type " << ( *tag ).second;
#endif
        addField ( ( *tag ).first, ( *tag ).second );
    }

    // read parameters for treatment of XML entities
//  map<Lima::LimaChar,unsigned int> specialCharSize;
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::init(): getMapAtKey(specialCharacterSize)";
#endif
    try
    {
        const map<string,string>& specialCharSizeMap=unitConfiguration.
                getMapAtKey ( "specialCharacterSize" );
        map<string,string>::const_iterator
        it=specialCharSizeMap.begin(),
           it_end=specialCharSizeMap.end();
        for ( ; it!=it_end; it++ )
        {
            m_specialCharacterSize[ ( *it ).first[0]]=atoi ( ( *it ).second.c_str() );
        }
    }
    catch ( NoSuchMap& e )
    {
        DRLOGINIT;
        LWARN << "StructuredDocumentXMLParser::init: no such map specialCharacterSize";
        // ignored: keep empty map
    }

    try
    {
      if (unitConfiguration.getParamsValueAtKey ( "addAbsoluteOffsetToTokens" ) == "no")
        m_addAbsoluteOffsetToTokens = false;
    }
    catch ( NoSuchParam& e )
    {
        DRLOGINIT;
        LINFO << "StructuredDocumentXMLParser: No parameter addAbsoluteOffsetToTokens. Using default 'yes'";
    }

#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::init() done";
#endif
}

//**********************************************************************
// functions to deal with special characters (predefined XML entities)
bool StructuredDocumentXMLParser::
isSpecialCharacter ( const Lima::LimaChar c )
{
    return ( m_specialCharacterSize.find ( c )
             != m_specialCharacterSize.end() );
}

unsigned int StructuredDocumentXMLParser::
getSpecialCharSize ( const Lima::LimaChar c )
{
    return m_specialCharacterSize[c];
}


void StructuredDocumentXMLParser::readPropertiesMetadata (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& groupConf )
{
#ifdef DEBUG_LP
    DRLOGINIT;
#endif

    // Read list of standard properties
    try
    {
        deque<string> standardPropertyList = groupConf.getListsValueAtKey ( "standard-properties-list" );
        // get list of identifier of attributes from config file
        for ( deque<string>::const_iterator attributeIdentifier = standardPropertyList.begin();
                attributeIdentifier != standardPropertyList.end(); attributeIdentifier++ )
        {
            // build attribute from identifier
            DocumentPropertyType attr = *DocumentsReaderResources::single().getProperty ( *attributeIdentifier );
            // add new property to map elementTag->property*
            const std::deque<QString> elementTagSet = attr.getElementTagNames();
            for ( std::deque<QString>::const_iterator it = elementTagSet.begin() ;
                    it != elementTagSet.end() ; it++ )
            {
                const QString elementTagName = *it;
                auto ite = m_elementTag2DocumentPropertyType.find( elementTagName );
                if( ite != m_elementTag2DocumentPropertyType.end() ){
                    if( ite->second.getId() != attr.getId() ) {
                      DRLOGINIT;
                      LWARN << "ElementTag" << elementTagName << "cannot be used for"<< attr.getId() <<", it is already used for Document Property" << ite->second.getId() << ". Skip";
                    }
                    continue;
                }
#ifdef DEBUG_LP
                LDEBUG << "ElementTag" << elementTagName << "is used for property" << attr.getId();
#endif
                m_elementTag2DocumentPropertyType.insert ( std::make_pair( elementTagName, attr ) );
            }
            // add property to map elementTag->property*
            const std::deque< std::pair<QString,QString> >& elementTagOfPairSet = attr.getAttributeTagNames();
            for ( std::deque< std::pair<QString,QString> >::const_iterator it = elementTagOfPairSet.begin() ;
                    it != elementTagOfPairSet.end() ; it++ )
            {
                const QString elementTagName = (*it).first;
                const QString attributeTagName = (*it).second;
                const auto ite = m_elementTagOfPair2DocumentPropertyType.find( elementTagName );
                if( ite != m_elementTagOfPair2DocumentPropertyType.end() ){
                    if( ite->second.getId() != attr.getId() ) {
                        DRLOGINIT;
                        LWARN << "ElementTag" << elementTagName << "AttributeTagName" << attributeTagName << "cannot be used for"<< attr.getId() <<", it is already used for Document Property" << ite->second.getId() << ". Skip";
                    }
                    continue;
                }
#ifdef DEBUG_LP
                LDEBUG << "ElementTag" << elementTagName << "AttributeTagName" << attributeTagName << "is used for property" << attr.getId();
#endif
                m_elementTagOfPair2DocumentPropertyType.insert ( std::make_pair( elementTagName, attr ) );
            }
        }
    }
    catch ( NoSuchList & e )
    {
        DRLOGINIT;
        LWARN << "StructuredDocumentXMLParser: NoSuchList Exception: standard-properties-list";
    }

    // Read list of specific attribute
    try
    {
        deque<string> extendedPropertyList= groupConf.getListsValueAtKey ( "extended-properties-list" );
        // get list of identifier of attributes from config file
        for ( deque<string>::const_iterator attributeIdentifier = extendedPropertyList.begin();
                attributeIdentifier != extendedPropertyList.end(); attributeIdentifier++ )
        {
            // build attribute from identifier
            DocumentPropertyType attr = *DocumentsReaderResources::single().getProperty ( *attributeIdentifier );
            // add new property to map elementTag->property*
            const std::deque<QString> elementTagSet = attr.getElementTagNames();
            for ( std::deque<QString>::const_iterator it = elementTagSet.begin() ;
                    it != elementTagSet.end() ; it++ )
            {
                const QString elementTagName = *it;
                const auto ite = m_elementTag2DocumentPropertyType.find( elementTagName );
                if( ite != m_elementTag2DocumentPropertyType.end() ){
                    if( ite->second.getId() != attr.getId() ) {
                      DRLOGINIT;
                      LWARN << "ElementTag" << elementTagName << "cannot be used for"<< attr.getId() <<", it is already used for Document Property" << ite->second.getId() << ". Skip";
                    }
                    continue;
                }
#ifdef DEBUG_LP
                LDEBUG << "ElementTag" << elementTagName << "is used for property" << attr.getId();
#endif

                m_elementTag2DocumentPropertyType.insert ( std::make_pair( elementTagName, attr ) );
            }
            // add property to map elementTag.attribute -> property*
            const std::deque< std::pair<QString,QString> > elementTagOfPairSet = attr.getAttributeTagNames();
            for ( std::deque< std::pair<QString,QString> >::const_iterator it = elementTagOfPairSet.begin() ;
                    it != elementTagOfPairSet.end() ; it++ )
            {
                const QString elementTagName = (*it).first;
                const QString attributeTagName = (*it).second;
                const auto ite = m_elementTagOfPair2DocumentPropertyType.find( elementTagName );
                if( ite != m_elementTagOfPair2DocumentPropertyType.end() ){
                    if( ite->second.getId() != attr.getId() ) {
                      DRLOGINIT;
                      LWARN << "ElementTag" << elementTagName << "AttributeTagName" << attributeTagName << "cannot be used for"<< attr.getId() <<", it is already used for Document Property" << ite->second.getId() << ". Skip";
                    }
                    continue;
                }
#ifdef DEBUG_LP
                LDEBUG << "ElementTag" << elementTagName << "AttributeTagName" << attributeTagName << "is used for property" << attr.getId();
#endif
                m_elementTagOfPair2DocumentPropertyType.insert ( std::make_pair( elementTagName, attr ) );
            }
        }
    }
    catch ( NoSuchList & e )
    {
        DRLOGINIT;
        LWARN << "StructuredDocumentXMLParser: NoSuchList Exception: extended-properties-list";
    }

}

void StructuredDocumentXMLParser::readFieldTags (
    GroupConfigurationStructure& groupConf, const Lima::DocumentsReader::FieldType type, vector< pair< QString, Lima::DocumentsReader::FieldType > >& fields )
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser::readFieldTags" << type;
#endif
    QString errMess ( "" );

    const QString& fieldTagListName=tagSemanticItem[type];
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::readFieldTags fieldTagListName:" << fieldTagListName;
#endif

    // Read list of tag for XML root
    try
    {
        deque<string> tagList= groupConf.getListsValueAtKey ( fieldTagListName.toUtf8().constData() );
        for ( deque<string>::const_iterator tag=tagList.begin();
                tag!=tagList.end(); tag++ )
        {
            fields.push_back ( make_pair (  QString::fromUtf8(tag->c_str()),type ) );
        }
    }
    catch ( NoSuchGroup & e )
    {
        errMess = QString ( "NoSuchGroup Exception: tagSemantic" );
        //errMess = e.what();
    }
    catch ( NoSuchList & e )
    {
        errMess = QString ( "NoSuchList Exception: " ) +fieldTagListName;
        //errMess = e.what();
    }
    if ( errMess.compare ( "" ) )
    {
#ifdef DEBUG_LP
        DRLOGINIT;
        LWARN << "DocumentReader: " << errMess;
#endif
    }
}

void StructuredDocumentXMLParser::addField ( const QString& fieldName, const Lima::DocumentsReader::FieldType type )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "StructuredDocumentXMLParser::addField" << fieldName << type;
#endif
    //uint64_t spaceOffset=fieldName.find(' '); portage 32 64
    int spaceOffset=fieldName.indexOf ( ' ' );
    if ( spaceOffset == -1 )
    {
        m_fields.insert ( std::pair<QString,FieldTypeElement> ( fieldName,FieldTypeElement ( type ) ) );
    }
    else
    {
        int equalOffset=fieldName.indexOf ( '=',spaceOffset+1 );
        QString attributeName ( fieldName.mid(spaceOffset+1,equalOffset-spaceOffset-1) );
        QString newFieldName ( fieldName.left(spaceOffset) );

        m_fields.insert ( std::pair<QString,FieldTypeElement> ( newFieldName,FieldTypeElement ( type,attributeName ) ) );
    }
}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------
bool StructuredDocumentXMLParser::startDocument(unsigned int parserOffset)
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser: startDocument()";
#endif

    m_currentDocument = boost::shared_ptr< ContentStructuredDocument >(new ContentStructuredDocument);
    // creation d'un element artificiel = ROOT
    DocumentPropertyType noProperty;
    QString rootName ( "ROOT" );
    m_currentDocument->pushHierarchyChild ( rootName, parserOffset, noProperty );
    setCurrentByteOffset ( parserOffset );
    return true;
}

bool StructuredDocumentXMLParser::endDocument()
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser: endDocument()";
#endif

//       assert( m_currentDocument->size() == 1);
//       AbstractStructuredDocumentElement* currentElement = m_currentDocument->back();
//       delete currentElement;
//       currentElement->pop_back();
//     delete m_currentDocument; m_currentDocument = 0;
  return true;
}

// -----------------------------------------------------------------------
//  Implementations of the SAX DocumentHandler interface
// -----------------------------------------------------------------------

bool StructuredDocumentXMLParser::startElement ( const QString& namespaceURI, const QString& name, const QString& qName, const QXmlStreamAttributes& attributes, unsigned int parserOffset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
#endif
  LIMA_UNUSED(namespaceURI)
  LIMA_UNUSED(qName)

#ifdef DEBUG_LP
  LDEBUG << "StructuredDocumentXMLParser::startElement(" << name << parserOffset << "), document.size="
          << m_currentDocument->size();
#endif

  AbstractStructuredDocumentElement* newElement = 0;

  // Trois situations contextuelles:
  //  - la pile est vide,
  //  - le dernier element est "discardable" ou
  //  - il y a un element "non discardable"

#ifdef DEBUG_LP
  assert(!m_currentDocument->empty());
#endif


  DocumentPropertyType propType;
  if ( isMetaData ( name ) )
  {
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: " << name << " is metadata" ;
#endif
    // recupere la propriete liee a ce tag
    propType = getMetaDataFromName ( name );
  }


  AbstractStructuredDocumentElement* currentElement = m_currentDocument->back();
  switch (currentElement->nodeType())
  {
  case NODE_DISCARDABLE:
    // current element is discardable
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: current"  << currentElement->getElementName() << "is discardable, discards element " << name;
#endif
    newElement = m_currentDocument->pushDiscardableChild ( name, parserOffset );
    break;
  // current element is indexing
  case NODE_INDEXING:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: current"  << currentElement->getElementName() << "is indexing";
#endif
    // field is metadata (exemple: <TITLE>document 1</TITLE> or <filename>d1.xml<filename>
    // exemple: <USELESS>.....</USELESS>
    if ( isDiscardable ( name ) )
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement:" << name << "discardable element" ;
#endif
      // Element discardable. Don't move
      newElement = m_currentDocument->pushDiscardableChild ( name, parserOffset );
    }
    else // current element is indexing. All its not discardable children are presentation
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement:" << name << "presentation element" ;
#endif
      newElement = m_currentDocument->pushPresentationChild ( name, parserOffset );
    }
    break;
  // current element is hierarchy
  case NODE_HIERARCHY:
    // hierarchy elements can have indexing, hierarchy and ignored children
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: current"  << currentElement->getElementName() << "is hierarchy";
#endif
    if ( isHierarchy ( name ) )
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement: " << name << "hierarchy element" ;
#endif
      newElement = m_currentDocument->pushHierarchyChild ( name, parserOffset, propType );
      m_processor->startHierarchy ( *m_currentDocument );
    }
    else if ( isIndexing ( name ) )
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement: " << name << "indexing element" ;
#endif
      newElement = m_currentDocument->pushIndexingChild ( name, parserOffset, propType );
      m_processor->startIndexing ( *m_currentDocument );
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement: " << name << "ignored element" ;
#endif
      newElement = m_currentDocument->pushIgnoredChild ( name, parserOffset, propType );
    }
    break;
  case NODE_PRESENTATION:
    // presentation elements can only have discardable and presentation children
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: current"  << currentElement->getElementName() << "is presentation";
#endif
    if ( isDiscardable ( name ) )
    {
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement: " << name << " is discardable " ;
#endif
      // Element discardable. Don't move
      newElement = m_currentDocument->pushDiscardableChild ( name, parserOffset );
    }
    else // child is presentation
    {
      currentElement->addSpaces(parserOffset-currentElement->getOffset());
      newElement = m_currentDocument->pushPresentationChild ( name, parserOffset );
    }
    break;
  case NODE_IGNORED:
    // all ignored elements children are also ignored
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: current"  << currentElement->getElementName() << "is ignored. Ignoring also" << name;
#endif
    newElement = m_currentDocument->pushIgnoredChild ( name, parserOffset, propType );
    break;
  default:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::startElement: all inheritance cases are handled for current"  << currentElement->getElementName() << ". We should not get here while handling" << name;
    // All inheritance cases are handled. We should not get here
    assert(false);
#endif
    ;
  }
#ifdef DEBUG_LP
  assert ( newElement );
#endif
  if ( hasMetaData ( name ) && newElement->nodeType() != NODE_DISCARDABLE  )
  {
    // TODO: changer l'implementation, trop complique!
    // field has metadata (example: <document id="number_one">qsdfhqmsfjh</document>
 #ifdef DEBUG_LP
   LDEBUG << "StructuredDocumentXMLParser::startElement: " << name  << " has metadata";
#endif
    auto range = m_elementTagOfPair2DocumentPropertyType.equal_range ( name );
    // iteration sur toutes les proprietes suggerees par le nom de l'element
    for ( auto propIt = range.first ; propIt !=  range.second ; ( propIt ) ++ )
    {
      DocumentPropertyType& propType = ( *propIt ).second;
#ifdef DEBUG_LP
      LDEBUG << "StructuredDocumentXMLParser::startElement: try " << propType.getId();
#endif
      auto attrAndElementNames = propType.getAttributeTagNames();
      for (auto namesIt = attrAndElementNames.begin(); namesIt != attrAndElementNames.end() ; namesIt++ )
      {
        if ( ! ( *namesIt ).first.compare ( name ) )
        {
          // field contain metadata in attribute
            QString attributeName =  (*namesIt ).second;
            Lima::LimaString lic2mValue=attributes.value ( attributeName ).toString();
            std::string utf8Value = Misc::limastring2utf8stdstring ( lic2mValue );
#ifdef DEBUG_LP
          LDEBUG << "StructuredDocumentXMLParser::startElement: found " << ( *namesIt ).first
                  << " as element with attribute " << ( *namesIt ).second
                  << " and value " << utf8Value;
#endif
          m_currentDocument->setDataToLastElement ( propType, utf8Value, m_processor );
        }
      }
    }
  }

#ifdef DEBUG_LP
  LDEBUG << "StructuredDocumentXMLParser::startElement() end";
#endif
  return true;
}

bool StructuredDocumentXMLParser::endElement ( const QString& namespaceURI, const QString& qsname, const QString& qName , unsigned int parserOffset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
#endif
  LIMA_UNUSED(namespaceURI)
  LIMA_UNUSED(qName)

  AbstractStructuredDocumentElement* currentElement = m_currentDocument->back();
#ifdef DEBUG_LP
  LDEBUG << "StructuredDocumentXMLParser::endElement" << qsname << parserOffset << ". currentElement=" << currentElement->getElementName();

  assert ( currentElement->getElementName() == qsname );
#endif
  switch (currentElement->nodeType())
  {
  case NODE_DISCARDABLE:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop discardable element " << qsname;
#endif
    m_currentDocument->popDiscardableElement(parserOffset);
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement(" << qsname
    << "), document.size = " << m_currentDocument->size() << " before return";
#endif
    return true;
    break;
  case NODE_INDEXING:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop indexing element " << qsname;
    assert(currentElement->size() > 0);
#endif

#ifndef DEBUG_LP
    try {
#endif
        m_processor->handle ( *m_currentDocument, currentElement->front()->getText(), m_addAbsoluteOffsetToTokens ? currentElement->front()->getOffset() : 0, qsname.toUtf8().constData());
#ifndef DEBUG_LP
    }
    catch(const LinguisticProcessing::LinguisticProcessingException& e)
    {
        DRLOGINIT;
        LERROR << "StructuredDocumentXMLParser::endElement: error while handling indexing element"<< qsname<< "absolute offset:" << currentElement->front()->getOffset();
    }
#endif

#ifdef DEBUG_LP
   LDEBUG << "StructuredDocumentXMLParser::endElement: pop indexing element handled" << qsname;
#endif
    m_processor->endIndexing ( *m_currentDocument ); //m_processor = CoreXmlReaderClient
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop indexing element ended" << qsname;
#endif
    m_currentDocument->popIndexingElement(parserOffset);
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop indexing element poped" << qsname;
#endif
    return true;
    break;
  case NODE_HIERARCHY:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop hierarchy element " << qsname;
#endif
    m_processor->endHierarchy ( *m_currentDocument );
    m_currentDocument->popHierarchyElement(parserOffset);
    return true;
    break;
  case NODE_PRESENTATION:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop presentation element " << qsname;
#endif
    m_currentDocument->popPresentationElement ( parserOffset );
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement(" << qsname
    << "), document.size = " << m_currentDocument->size() << " before return";
#endif
    return true;
    break;
  case NODE_IGNORED:
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement: pop ignored element " << qsname;
#endif
    if (currentElement->getPropType().getValueCardinality() != CARDINALITY_NONE && !currentElement->empty() )
      m_currentDocument->setDataToElement(currentElement, currentElement->getPropType(), currentElement->back()->getText().toUtf8().constData(), m_processor);
    m_currentDocument->popIgnoredElement ( parserOffset );
#ifdef DEBUG_LP
    LDEBUG << "StructuredDocumentXMLParser::endElement(" << qsname << "), document.size = "
            << m_currentDocument->size() << " before return";
#endif
    return true;
    break;
  default:
    // All inheritance cases are handled. We should not get here
    DRLOGINIT;
    LERROR << "All inheritance cases are handled. We should not get here";
    assert(false);
    return false;
  }
  return true;
}


// -----------------------------------------------------------------------
//  Handling of offset
// -----------------------------------------------------------------------

void StructuredDocumentXMLParser::setCurrentByteOffset ( const unsigned int offset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "StructuredDocumentXMLParser::setCurrentByteOffset(" << offset << ")";
#else
  LIMA_UNUSED(offset);
#endif
  if ( m_currentDocument->size() != 0 )
  {
    // AbstractStructuredDocumentElement* currentElement = m_currentDocument->back();
    // currentElement->moveByteReaderPosTo ( offset );
  }
}

//**********************************************************************
bool StructuredDocumentXMLParser::characters (const QString& ch,
                                              unsigned int parserOffset)
{
    QString value = ch;
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "StructuredDocumentXMLParser::characters" << value.left(50)
            << "(...), length=" << value.size() << ", parserOffset=" << parserOffset;
#endif
    auto currentElement = m_currentDocument->back();
    currentElement->setOffset(parserOffset);
    if ( !value.isEmpty() )
    {
        Lima::LimaChar firstChar=value[0];
        if ( m_shiftFrom->contains(parserOffset) && isSpecialCharacter ( firstChar ) )
        {
#ifdef DEBUG_LP
            LDEBUG << "StructuredDocumentXMLParser::characters m_shiftFrom:"
                    << *m_shiftFrom;
            LDEBUG << "StructuredDocumentXMLParser::characters: first char "
                    << firstChar << " is special character: add "
                    << getSpecialCharSize ( firstChar )-1 << " spaces";
#endif
            Lima::LimaString spaces ( getSpecialCharSize ( firstChar )-1,' ' );
            value.insert ( 1,spaces );
        }
        else
        {
#ifdef DEBUG_LP
            LDEBUG << "StructuredDocumentXMLParser::characters: first char "
                    << firstChar << "(" << firstChar
                    << ") is not a special character";
#endif
        }
        currentElement->addToCurrentOffset ( value );
    }

    return true;
}

bool StructuredDocumentXMLParser::isFieldType ( const QString& elementName, const Lima::DocumentsReader::FieldType& type ) const
{
    auto range=m_fields.equal_range ( elementName );
    for ( auto it=range.first; it!=range.second; it++ )
    {
        if ( ( *it ).second.getType() == type )
        {
            return true;
        }
    }
    return false;
}

bool StructuredDocumentXMLParser::isHierarchy ( const QString& elementName ) const
{
    return isFieldType ( elementName,NODE_HIERARCHY );
}

bool StructuredDocumentXMLParser::isIndexing ( const QString& elementName ) const
{
    return isFieldType ( elementName,NODE_INDEXING );
}

bool StructuredDocumentXMLParser::isRoot ( const QString& elementName ) const
{
    return isFieldType ( elementName,NODE_ROOT );
}

bool StructuredDocumentXMLParser::isDiscardable ( const QString& elementName ) const
{
    return isFieldType ( elementName,NODE_DISCARDABLE );
}

// test si le nom de l'element correspond a
// 1) un element ayant des attributs dont la valeur deviendra une propriete du document
//            (ex <file source="doc0.txt"> ou <document id="d1"> ...</document>
bool StructuredDocumentXMLParser::hasMetaData ( const QString& elementName ) const
{
    auto range=m_elementTagOfPair2DocumentPropertyType.equal_range ( elementName );

    if ( range.first !=  range.second )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// test si le nom de l'element correspond a
//  2) un element contenant du texte dont la valeur est la propriete du document
//            (ex <filename>doc0.txt<filename>)
bool StructuredDocumentXMLParser::isMetaData ( const QString& elementName ) const
{
    auto pos = m_elementTag2DocumentPropertyType.find ( elementName );
    if ( pos !=  m_elementTag2DocumentPropertyType.end() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// retourne la propriete associee au nom de l'element
DocumentPropertyType StructuredDocumentXMLParser::getMetaDataFromName ( const QString& elementName ) const
{

    auto pos = m_elementTag2DocumentPropertyType.find ( elementName );
    if ( pos !=  m_elementTag2DocumentPropertyType.end() )
    {
#ifdef DEBUG_LP
        DRLOGINIT;
        LDEBUG << "StructuredDocumentXMLParser::getMetaDataFromName(" << elementName << ")="
        << ( ( *pos ).second ).getId();
#endif
        return ( *pos ).second;
    }
    else
    {
        DRLOGINIT;
        LWARN << "StructuredDocumentXMLParser::getMetaDataFromName: no property associated with element name "
        << elementName;
        return DocumentPropertyType();
    }
}


} // namespace DocumentsReader
} // namespace Lima

