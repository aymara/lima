/************************************************************************
 * @file   abstractDocumentXMLParser.cpp
 * @author Olivier Mesnard
 * @date   20 Sept 2005
 *
 * common functions of [Structured] and [Simple] DocumentXMLParser
 *
 ***********************************************************************/

#include "documentProperty.h"

#include "linguisticProcessing/core/abstractReaderResource.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

// #include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "linguisticProcessing/core/XmlProcessingCommon.h"

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace DocumentsReader {

class DocumentPropertyTypePrivate
{
  friend class DocumentPropertyType;

public:
  DocumentPropertyTypePrivate();
  DocumentPropertyTypePrivate(const DocumentPropertyTypePrivate& dpt);
  DocumentPropertyTypePrivate& operator=(const DocumentPropertyTypePrivate& dpt);
  ~DocumentPropertyTypePrivate() {}

  StorageType m_storageType;
  long int m_storageSize;
  CardinalityType m_cardinalityType;
  std::deque< QString > m_tagElementNames;
  std::deque< std::pair<QString,QString> > m_tagAttributeNames;
};

DocumentPropertyTypePrivate::DocumentPropertyTypePrivate()
        : m_storageType ( STORAGE_UTF8_STRING ), m_storageSize ( 0 ),
        m_cardinalityType ( CARDINALITY_OPTIONAL ),
        m_tagElementNames(), m_tagAttributeNames()
{
}

DocumentPropertyTypePrivate::DocumentPropertyTypePrivate(const DocumentPropertyTypePrivate& dpt)
        : m_storageType ( dpt.m_storageType ), m_storageSize ( dpt.m_storageSize ),
        m_cardinalityType ( dpt.m_cardinalityType ),
        m_tagElementNames(dpt.m_tagElementNames), m_tagAttributeNames(dpt.m_tagAttributeNames)
{
}

DocumentPropertyTypePrivate& DocumentPropertyTypePrivate::operator=(const DocumentPropertyTypePrivate& dpt)
{
  m_storageType = dpt.m_storageType;
  m_storageSize = dpt.m_storageSize;
  m_cardinalityType = dpt.m_cardinalityType;
  m_tagElementNames = dpt.m_tagElementNames;
  m_tagAttributeNames = dpt.m_tagAttributeNames;
  return *this;
}

DocumentPropertyType::DocumentPropertyType()
        : m_d(new DocumentPropertyTypePrivate())
{
}

DocumentPropertyType& DocumentPropertyType::operator=(const DocumentPropertyType& dpt)
{
  if (this != &dpt)
    *m_d = *dpt.m_d;
  m_id = dpt.m_id;
  return *this;
}

DocumentPropertyType::DocumentPropertyType(const DocumentPropertyType& dpt)
        : m_d(new DocumentPropertyTypePrivate(*dpt.m_d))
{
  m_id = dpt.m_id;
}

DocumentPropertyType::~DocumentPropertyType()
{
  delete m_d;
}

const StorageType& DocumentPropertyType::getStorageType(void) const { return m_d->m_storageType; }
const CardinalityType& DocumentPropertyType::getValueCardinality(void) const { return m_d->m_cardinalityType; }
const long int& DocumentPropertyType::getStorageSize(void) const { return m_d->m_storageSize; }
const std::deque< QString >& DocumentPropertyType::getElementTagNames() const { return m_d->m_tagElementNames; }
const std::deque< std::pair<QString,QString> > & DocumentPropertyType::getAttributeTagNames() const { return m_d->m_tagAttributeNames; }

StorageType& DocumentPropertyType::getStorageType(void) { return m_d->m_storageType; }
CardinalityType& DocumentPropertyType::getValueCardinality(void) { return m_d->m_cardinalityType; }
long int& DocumentPropertyType::getStorageSize(void) { return m_d->m_storageSize; }


void DocumentPropertyType::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* )
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "DocumentPropertyType::init()";
#endif

    // type de l'attribut (string, integer, date)
    try
    {
        QString storageTypeId = QString::fromUtf8(unitConfiguration.getParamsValueAtKey ( "storageType" ).c_str());
        for ( int i = 0 ; static_cast<StorageType> ( i ) < MAX_STORAGE_TYPE ; i++ )
        {
            StorageType storageType = static_cast<StorageType> ( i );
            if ( !storageTypeId.compare ( storageTypeTag[storageType] ) )
            {
                m_d->m_storageType = storageType;
                break;
            }
        }
    }
    catch ( NoSuchParam& e )
    {
        DRLOGINIT;
        LERROR << "no param 'storageType' in DocumentPropertyType group for " << getId();
        throw Lima::InvalidConfiguration();
    }

    // taille pour le stockage (nombre de bytes pour une chaine de caracteres)
    try
    {
        m_d->m_storageSize=atoi ( unitConfiguration.getParamsValueAtKey ( "storageSize" ).c_str() );
    }
    catch ( NoSuchParam& e )
    {
        DRLOGINIT;
        LWARN << "no param 'storageSize' in DocumentPropertyType group for " << getId();
    }

    // cardinalite: valeur obligatoire, optionelle ou multiple
    try
    {
        QString cardinalityTypeId = QString::fromUtf8(unitConfiguration.getParamsValueAtKey ( "cardinality" ).c_str());
        for ( int i = 0 ; static_cast<CardinalityType> ( i ) < MAX_STORAGE_TYPE ; i++ )
        {
            CardinalityType cardinalityType = static_cast<CardinalityType> ( i );
            if ( !cardinalityTypeId.compare ( cardinalityTypeTag[cardinalityType] ) )
            {
                m_d->m_cardinalityType = cardinalityType;
                break;
            }
        }
    }
    catch ( NoSuchParam& e )
    {
        DRLOGINIT;
        LERROR << "no param 'cardinality' in DocumentPropertyType group for " << getId();
        throw Lima::InvalidConfiguration();
    }

    // liste des noms d'elements dont le contenu defini la valeur de l'attribut
    try
    {
        const std::deque< std::string >& list = unitConfiguration.getListsValueAtKey ( "elementNames" );
        Q_FOREACH( const std::string& elem, list)
        {
          m_d->m_tagElementNames.push_back(QString::fromUtf8(elem.c_str()));
        }
    }
    catch ( NoSuchList& e )
    {
#ifdef DEBUG_LP
        LDEBUG << "no list 'elementNames' in DocumentPropertyType group for " << getId();
#endif
    }

    // liste des noms d'elements + attributs dont la valeur defini la valeur de l'attribut
    try
    {
        const std::deque< std::string >& list = unitConfiguration.getListsValueAtKey ( "attributeNames" );
        for ( auto it = list.begin() ; it != list.end() ; it++ )
        {
            //uint64_t spaceOffset=it->find(' '); portage 32 64
            std::string::size_type spaceOffset=it->find ( ' ' );
            if ( spaceOffset == std::string::npos )
            {
                DRLOGINIT;
                LWARN << "DocumentPropertyType::init(): ill formed attributeName for "
                << getId() << ": " << *it;
            }
            else
            {
                unsigned int equalOffset=it->find ( '=',spaceOffset+1 );
                std::string attributeName ( *it,spaceOffset+1,equalOffset-spaceOffset-1 );
                std::string newFieldName ( *it,0,spaceOffset );
#ifdef DEBUG_LP
                LDEBUG << "DocumentPropertyType::init: m_tagAttributeNames.push_back("
                << newFieldName << ", " << attributeName << ")";
#endif
                m_d->m_tagAttributeNames.push_back ( pair<QString,QString> ( QString::fromUtf8(newFieldName.c_str()),QString::fromUtf8(attributeName.c_str()) ) );
            }
        }
    }
    catch ( NoSuchList& e )
    {
 #ifdef DEBUG_LP
       LDEBUG << "no list 'attributeNames' in DocumentPropertyType group for " << getId();
#endif
    }

}

bool DocumentPropertyType::operator<(const DocumentPropertyType& dpt) const
{
  return getId() < dpt.getId();
}


Lima::SimpleFactory<AbstractReaderResource,Lima::DocumentsReader::StandardDocumentPropertyType>
standardPropertyTypeFactory ( STANDARDDOCUMENTPROPERTYTYPE_CLASSID );

StandardDocumentPropertyType::StandardDocumentPropertyType()
{
}

void StandardDocumentPropertyType::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager )
{
    DocumentPropertyType::init (
        unitConfiguration, manager );
}

Lima::SimpleFactory<AbstractReaderResource,Lima::DocumentsReader::ExtensionDocumentPropertyType>
extensionDocumentPropertyTypeFactory ( EXTENSIONDOCUMENTPROPERTYTYPE_CLASSID );

ExtensionDocumentPropertyType::ExtensionDocumentPropertyType()
{
}

void ExtensionDocumentPropertyType::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager )
{
    DocumentPropertyType::init (
        unitConfiguration, manager );
    DRLOGINIT;
    if ( m_d->m_tagAttributeNames.empty() && m_d->m_tagElementNames.empty() )
    {
        LERROR << "no list ('attributeNames' or 'elementNames') in DocumentPropertyType group for " << getId();
        throw Lima::InvalidConfiguration();
    }
}

} // namespace DocumentsReader
} // namespace Lima

