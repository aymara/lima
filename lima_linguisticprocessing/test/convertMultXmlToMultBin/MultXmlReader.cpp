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
  MultXmlReaderPrivate(const std::string& filename, std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom);
  ~MultXmlReaderPrivate();

  QXmlSimpleReader* m_parser;
};

MultXmlReaderPrivate::MultXmlReaderPrivate(const std::string& filename,
                           std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom):
    m_parser(new QXmlSimpleReader())
{

  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file
  MultXmlHandler handler(output, shiftFrom);
  m_parser->setContentHandler(&handler);
  m_parser->setErrorHandler(&handler);
  m_parser->setFeature("http://qt-project.org/xml/features/report-start-end-entity",true);
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    LIMA_EXCEPTION_SELECT_LOGINIT(BOWLOGINIT,
                                  "Cannot open" << filename.c_str(),
                                  XMLException);
  if (!m_parser->parse( QXmlInputSource(&file)))
  {
    LIMA_EXCEPTION_SELECT_LOGINIT(
      BOWLOGINIT,
      "Cannot parse" << filename.c_str() << m_parser->errorHandler()->errorString(),
      XMLException);
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
                           std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom):
m_d(new MultXmlReaderPrivate(filename,output, shiftFrom))
{
}

MultXmlReader::~MultXmlReader() {
  delete m_d;
}

} // end namespace
} // end namespace
