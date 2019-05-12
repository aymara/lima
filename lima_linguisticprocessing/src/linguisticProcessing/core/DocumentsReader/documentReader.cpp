/************************************************************************
 *
 * @file       documentReader.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Mon Oct 27 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003 by CEA LIST (LVIC)
 *
 ***********************************************************************/

#include "documentReader.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/LimaString.h"

#include "linguisticProcessing/core/XmlProcessingCommon.h"

// Pour la lecture des fichiers de configuration
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"


#include "documentReaderException.h"
#include "documentReaderResources.h"

#include <QtCore/QXmlStreamReader>
#include <QBuffer>
#include <QFile>



using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace DocumentsReader {

class DocumentReaderPrivate
{
public:
  DocumentReaderPrivate (ModuleConfigurationStructure& conf);

  virtual ~DocumentReaderPrivate();

  QXmlStreamReader* m_reader;
  StructuredDocumentXMLParser* m_parser;

  //private functions
  //void init(Common::XMLConfigurationFiles::XMLConfigurationFileParser& configParser)
  void init ( Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf );
  /* OME
    *          void
    *            readFieldTags(Common::XMLConfigurationFiles::XMLConfigurationFileParser& configParser,
    *                          const FieldType type,
    *                          std::vector<std::pair<std::string,FieldType> >& fields);
    */
};

//***********************************************************************
// constructors
//***********************************************************************
/*
DocumentReader::DocumentReader(const std::string &configurationFileName)
{
  DRLOGINIT;

  // Initialize the XML4C2 system
  try
    {
    LDEBUG << "DocumentReader: XMLPlatformUtils::Initialize()...";
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& e)
  {
    handleXercesException(e);
  }

  // Read parameters in config files
  XMLConfigurationFileParser configParser(configurationFileName);
  ModuleConfigurationStructure conf = configParser.getModuleConfiguration("lp-xmlreaderclient");

  init(conf);
}
*/

DocumentReaderPrivate::DocumentReaderPrivate ( ModuleConfigurationStructure& conf ) :
    m_reader(0),
    m_parser(0)
{
  init ( conf );
}

DocumentReaderPrivate::~DocumentReaderPrivate()
{
  delete m_reader;
//   delete m_parser;
}

void DocumentReaderPrivate::init ( ModuleConfigurationStructure& conf )
{
    // to avoid unwanted encoding of text
    setlocale ( LC_ALL,"" );

    DRLOGINIT;
#ifdef DEBUG_LP
    LINFO << "DocumentReader::init";
#endif

    // init factory fo XmlDocumentParser and DocumentProperties
    try
    {
#ifdef DEBUG_LP
        DRLOGINIT;
        LDEBUG << "DocumentReader::init: DocumentsReaderResources::changeable().init(conf)";
#endif
        // init manager of documentsXMLParser
        DocumentsReaderResources::changeable().init ( conf );
#ifdef DEBUG_LP
        LDEBUG << "DocumentReader::init: DocumentsReaderResources::changeable().init(conf) done";
#endif
    }
    catch ( NoSuchModule& e )
    {
        LERROR << "no such module lp-xmlreaderclient";
    }
    // Create and configure the XMLParser
//     LDEBUG << "XMLReaderFactory::createXMLReader()...";
//     m_reader = new QXmlSimpleReader();
//
//     if ( !m_reader )
//     {
//         LERROR << "can't create _sax2XmlReader !!!";
//         return;
//     }

    // Create automate to store parsing context and to handle SAX event
//     LDEBUG << "DocumentReader:;init(): create DocumentXMLParser...";
    m_parser = DocumentsReaderResources::single().getXmlParser ( "documentXMLParser" );
//     m_parser = new StructuredDocumentXMLParser();

//     LDEBUG << "DocumentReader::init(): DocumentParser created";

//     m_reader->setContentHandler ( m_parser );
//     m_reader->setErrorHandler ( m_parser );

}

DocumentReader::DocumentReader ( ModuleConfigurationStructure& conf ) : m_d(new DocumentReaderPrivate(conf))
{
}


//***********************************************************************
// destructor
//***********************************************************************
DocumentReader::~DocumentReader()
{
    delete m_d;
}

void  DocumentReader::setLinguisticXMLDocHandler ( StructuredXmlDocumentHandler* handler )
{
    m_d->m_parser->setLinguisticXMLDocHandler ( handler );
}

//***********************************************************************
// member functions
//***********************************************************************
bool DocumentReader::initFile ( const std::string& filename )
{
#ifdef DEBUG_LP
    DRLOGINIT;

    LDEBUG << "DocumentReader::initFile: " << filename;
#endif
    try
    {
        // First call for progressive parsing
        // see additional call parseNext() in DocumentsStream::nextdocument
        QFile file(filename.c_str());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
          throw Lima::XMLException();
        if (m_d->m_reader != 0)
          delete m_d->m_reader;
        m_d->m_reader = new QXmlStreamReader(&file);
#ifdef DEBUG_LP
        LDEBUG << "DocumentReader: initFile OK";
#endif
        m_d->m_parser->startDocument(getCurrentOffsetFromXmlReader());
        return true;
    }
    catch ( Lima::XMLException &e )
    {
        handleXercesException ( e );
    }

#ifdef DEBUG_LP
    LDEBUG << "DocumentReader: initFile NOT OK";
#endif
    return false;
}

bool DocumentReader::initWithString ( const std::string& text )
{
#ifdef DEBUG_LP
    DRLOGINIT;
    LDEBUG << "DocumentReader::initWithString: " << Lima::LimaString::fromUtf8(text.c_str()).left(100) << (text.size()>100?"(...)":"");
#endif
    try
    {
      QString qtext = QString::fromUtf8(text.c_str());
      if (m_d->m_reader != 0)
        delete m_d->m_reader;
      m_d->m_reader = new QXmlStreamReader(qtext);
      m_d->m_parser->startDocument(getCurrentOffsetFromXmlReader());
      return true;
    }
    catch ( Lima::XMLException &e )
    {
        handleXercesException ( e );
    }

#ifdef DEBUG_LP
    LDEBUG << "DocumentReader: initWithString NOT OK";
#endif
    return false;
}

uint64_t DocumentReader::getCurrentOffsetFromXmlReader()
{
//   DRLOGINIT;
//   LDEBUG << "DocumentReader::getCurrentOffsetFromXmlReader:" << m_d->m_reader->characterOffset() ;
  // take into account the fact that QXmlStreamReader reports a position shifted by 1 to the right
  // when reading character sequencies
  if (m_d->m_reader->tokenType() == QXmlStreamReader::Characters)
    return ( m_d->m_reader->characterOffset() -1 );
  else
    return ( m_d->m_reader->characterOffset() );
}


bool DocumentReader::readXMLDocument()
{
#ifdef DEBUG_LP
    DRLOGINIT;
#endif
    try
    {
        // extract an element
#ifdef DEBUG_LP
        LDEBUG << "DocumentReader::readXMLDocument";
#endif

        while (!m_d->m_reader->atEnd())
        {
#ifdef DEBUG_LP
          LDEBUG <<"DocumentReader::readXMLDocument:"<<getCurrentOffsetFromXmlReader() << "; tokenType:" << m_d->m_reader->tokenType() << "; tokenString:" << m_d->m_reader->tokenString() << "; name:" << m_d->m_reader->name();
#endif
          if (m_d->m_reader->isStartElement())
          {
            m_d->m_parser->startElement ( m_d->m_reader->namespaceUri().toString(), m_d->m_reader->name().toString(), m_d->m_reader->qualifiedName().toString(), m_d->m_reader->attributes(), getCurrentOffsetFromXmlReader() );
          }
          else if (m_d->m_reader->isEndElement())
          {
            m_d->m_parser->endElement( m_d->m_reader->namespaceUri().toString(), m_d->m_reader->name().toString(), m_d->m_reader->qualifiedName().toString(), getCurrentOffsetFromXmlReader() );
          }
          else if (m_d->m_reader->isCharacters())
          {
            m_d->m_parser->characters(m_d->m_reader->text().toString(), getCurrentOffsetFromXmlReader());
          }
          m_d->m_reader->readNext();
          // do processing
        }
        if (m_d->m_reader->hasError())
        {
          // do error handling
          DRLOGINIT;
          LERROR <<"DocumentReader: readXMLDocument: parse error:" << m_d->m_reader->errorString();
          return false;
        }
        m_d->m_parser->endDocument();
    }
    // Handlers exception sur parseNext()
    catch ( Lima::XMLException &e )
    {
      handleXercesException ( e );
      return false;
    }

    return true;
}

} // namespace DocumentsReader
} // end namespace
