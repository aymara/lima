/************************************************************************
 *
 * @file       MultXmlHandler.h
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     lima_xmlprocessings
 * 
 * Handler for parsing XML representation of Amose analysed 
 * multimedia documents
 * 
 ***********************************************************************/

#ifndef MULTXMLHANDLER_H
#define MULTXMLHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowComplexToken.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include <stack>
#include <QtXml/QXmlSimpleReader>

namespace Lima {
namespace XmlReader {

class MultXmlHandlerPrivate;
/**
 * @brief Handler for parsing XML representation of Amose analysed multimedia documents
 */
class MultXmlHandler : public QXmlDefaultHandler
{
 public:
  MultXmlHandler(std::ostream& output);
  virtual ~MultXmlHandler();

  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endDocument() override;
  
  bool endElement(const QString & namespaceURI, const QString & name, const QString & qName) override;
  
  bool startDocument() override;
  
  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes& attributes) override;
  
  bool  startEntity(const QString & name) override;
  
  bool  endEntity(const QString & name) override;
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  bool warning(const QXmlParseException & exception) override;
  bool error(const QXmlParseException & exception) override;
  bool fatalError(const QXmlParseException & exception) override;
  
private:
  MultXmlHandlerPrivate* m_d;
};

} // end namespace
} // end namespace

#endif
