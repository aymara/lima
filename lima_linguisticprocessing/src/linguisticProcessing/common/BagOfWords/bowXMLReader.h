// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       bowXMLReader.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      XML input/output for BoW elements
 * 
 * 
 ***********************************************************************/

#ifndef BOWXMLREADER_H
#define BOWXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowToken.h"
#include "bowComplexToken.h"
#include "bowText.h"
#include "bowDocument.h"
#include <stack>

#include <QtCore/QDate>
#include <QtXml/QXmlSimpleReader>

namespace Lima {
namespace Common {
namespace BagOfWords {

//**********************************************************************
// XML Reader
//**********************************************************************
class LIMA_BOW_EXPORT BoWXMLReader
{
 public:
  BoWXMLReader(const std::string& filename,
               std::ostream& output); 
  ~BoWXMLReader();

 private:
  QXmlSimpleReader* m_parser;

  BoWXMLReader() {}
  BoWXMLReader(const BoWXMLReader&) {}
  BoWXMLReader& operator=(const BoWXMLReader&) { return *this; }
};

//**********************************************************************
// XML Parser (using xerces)
//**********************************************************************
class LIMA_BOW_EXPORT BoWXMLHandler : public QXmlDefaultHandler
{
 public:
  BoWXMLHandler(std::ostream& output);
  virtual ~BoWXMLHandler();

  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endDocument() override;
  
  bool endElement(const QString & namespaceURI, 
                  const QString & name, 
                  const QString & qName) override;
  
  bool startDocument() override;
  
  bool startElement(const QString & namespaceURI, 
                    const QString & name, 
                    const QString & qName, 
                    const QXmlAttributes& attributes) override;
  
  
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  bool warning(const QXmlParseException & exception) override;
  bool error(const QXmlParseException & exception) override;
  bool fatalError(const QXmlParseException & exception) override;
  
  
  
  // -----------------------------------------------------------------------
  // local functions 

 private:
  class CurrentComplexToken {
  public:
    CurrentComplexToken():token(),currentPart(0),head() {}
    CurrentComplexToken(boost::shared_ptr< BoWComplexToken > tok):
      token(tok),currentPart(0),head(0) {}
    boost::shared_ptr< BoWComplexToken > token;
    uint64_t currentPart;
    uint64_t head;
  };

  std::ostream& m_outputStream;
  BoWDocument m_currentBoWDocument;
  boost::shared_ptr< BoWText > m_currentBoWText;
  Common::Misc::GenericDocumentProperties m_currentProperties;
  std::vector<CurrentComplexToken> m_currentComplexToken;
  std::map<uint64_t, boost::shared_ptr< BoWToken > > m_refMap;
  
  // private functions
  uint64_t 
    getIntAttribute(const QXmlAttributes& attributes,
                    const char* name) const;
  float
    getFloatAttribute(const QXmlAttributes& attributes,
                               const char* name) const;
  
  std::string 
    getStringAttribute(const QXmlAttributes& attributes,
                       const char* name) const;
  LimaString 
    getLimaStringAttribute(const QXmlAttributes& attributes,
                            const char* name) const;
  QDate 
    getDateAttribute(const QXmlAttributes& attributes,
                     const char* name) const;

  bool addProperty(const QXmlAttributes& attributes);

  uint64_t 
    getTokenAttributes(const QXmlAttributes& attributes,
                       LimaString& lemma,
                       LinguisticCode& category,
                       uint64_t& position,
                       uint64_t& length,
                       uint64_t& id) const;
};

} // end namespace
} // end namespace
} // end namespace

#endif
