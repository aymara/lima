/************************************************************************
 *
 * @file       MultXmlReader.cpp
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     lima_xmlprocessings
 * 
 * Parser for XML representation of Amose analysed multimedia documents
 * 
 ***********************************************************************/

#include "MultXmlReader.h"
#include "MultXmlHandler.h"
#include "common/LimaCommon.h"

using namespace Lima;

namespace Lima {
namespace XmlReader {

class MultXmlReaderPrivate
{
friend class MultXmlReader;
public:
  MultXmlReaderPrivate(const std::string& filename, std::ostream& output); 
  ~MultXmlReaderPrivate();

  QXmlSimpleReader* m_parser;
};

MultXmlReaderPrivate::MultXmlReaderPrivate(const std::string& filename,
                           std::ostream& output):
    m_parser(new QXmlSimpleReader())
{

  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file and catch any exceptions
  //  that propogate out
  //
  try {
    MultXmlHandler handler(output);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
    m_parser->setFeature("http://qt-project.org/xml/features/report-start-end-entity",true);
    QFile file(filename.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      throw XMLException();
    if (!m_parser->parse( QXmlInputSource(&file)))
    {
      throw XMLException();
    }
  }
  catch (const XMLException& e) {
    BOWLOGINIT;
    LERROR << "An XML exception occurred: " << e.what() ;
    throw;
  }
}

MultXmlReaderPrivate::~MultXmlReaderPrivate() 
{
  //  Delete the parser itself.  Must be done prior to calling Terminate
  delete m_parser;
}

//**********************************************************************
// reader functions
//********************************************************************** 
MultXmlReader::MultXmlReader(const std::string& filename,
                           std::ostream& output):
m_d(new MultXmlReaderPrivate(filename,output))
{
}

MultXmlReader::~MultXmlReader() {
  delete m_d;
}

} // end namespace
} // end namespace
