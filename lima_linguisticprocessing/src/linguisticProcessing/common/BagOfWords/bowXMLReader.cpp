/*
    Copyright 2002-2013 CEA LIST

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
/************************************************************************
 *
 * @file       bowXMLReader.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * 
 ***********************************************************************/

#include "bowXMLReader.h"
#include "bowTerm.h"
#include "bowNamedEntity.h"
#include "bowDocumentST.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

using namespace Lima::Common::Misc;
using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {

// #define BOWXMLLOGINIT LOGINIT("BOW::XMLParser");

//***********************************************************************
// local exception
//***********************************************************************
class NoAttributeException : public runtime_error {
public:
  NoAttributeException(const string& mess):runtime_error(mess) {}
};

//**********************************************************************
// reader functions
//********************************************************************** 
BoWXMLReader::BoWXMLReader(const std::string& filename,
                           std::ostream& output):
m_parser()
{
  //
  //  Create a SAX parser object. Then, according to what we were told on
  //  the command line, set it to validate or not.
  //
  m_parser = new QXmlSimpleReader();
//   m_parser->setValidationScheme(SAXParser::Val_Auto);
//   m_parser->setDoNamespaces(false);
//   m_parser->setDoSchema(false);
//   m_parser->setValidationSchemaFullChecking(false);

  //
  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file and catch any exceptions
  //  that propogate out
  //
  try {
    BoWXMLHandler handler(output);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
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
    LERROR << "An XML exception occurred: " << e.getMessage() ;
    throw;
  }
}

BoWXMLReader::~BoWXMLReader() {
  //  Delete the parser itself.  Must be done prior to calling Terminate
  delete m_parser;

  // do not delete document or text (pointers are passed in return) 
}

//**********************************************************************
// xerces handler functions
//********************************************************************** 
BoWXMLHandler::BoWXMLHandler(std::ostream& output):
m_outputStream(output),
m_currentBoWDocument(),
m_currentBoWText(0),
m_currentComplexToken(),
m_refMap()
{
}

BoWXMLHandler::~BoWXMLHandler() {
}

// -----------------------------------------------------------------------
//  SAX ErrorHandler interface
// -----------------------------------------------------------------------
bool BoWXMLHandler::warning(const QXmlParseException& e) {
  BOWLOGINIT;
  LWARN << "Warning at file " << e.systemId()
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << e.message();
  return true;
}
bool BoWXMLHandler::error(const QXmlParseException& e) {
  BOWLOGINIT;
  LERROR << "Error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
  return false;
}
bool BoWXMLHandler::fatalError(const QXmlParseException& e) {
  BOWLOGINIT;
  LERROR << "Fatal Error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
  throw e;
}

// -----------------------------------------------------------------------
//  SAX handler interface
// -----------------------------------------------------------------------

bool BoWXMLHandler::startDocument() {
  BOWLOGINIT;
  LDEBUG << "Start Document";
  return true;
}
bool BoWXMLHandler::endDocument() {
  BOWLOGINIT;
  LDEBUG << "End Document";
  return true;
}

bool BoWXMLHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes& attributes) {
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  const QString&  stringName = name;

  BOWLOGINIT;
  LDEBUG << "Start Element " << stringName;

  LimaString lemma;
  LinguisticCode category = static_cast<LinguisticCode>(0);
  uint64_t position(0);
  uint64_t length(0);
  uint64_t id(0);
  
  if (stringName == "bowDocument") {
    m_currentBoWDocument.clear();
  }
  else if (stringName == "bowText") {
    m_currentBoWText=new BoWText();
  }
  else if (stringName == "hierarchy") {
    m_currentBoWText=new BoWText();
    bool isIndexingNode(false);
    try {
      string indexingNode=getStringAttribute(attributes,"indexingNode");
      if (indexingNode == "yes") {
        isIndexingNode=true;
      }
    } 
    catch (NoAttributeException& ) {
      // do nothing -> is not indexing node
    }
    if (isIndexingNode) {
      m_outputStream << INDEXING_BLOC;
      m_currentBoWText=new BoWText();
    }
    else {
      m_outputStream << HIERARCHY_BLOC;
    }
    string elementName("");
    try {
      elementName=getStringAttribute(attributes,"elementName");
    } 
    catch (NoAttributeException& ) {
      LERROR << "missing attribute elementName in " << stringName;
    }
    Lima::Common::Misc::writeStringField(m_outputStream, elementName);
  }
  else if (stringName == "property") {
    addProperty(attributes);
  }
  else if (stringName == "properties") {
    m_currentProperties.reinit();
  }
  else if (stringName == "tokens") {
    if (m_currentBoWText !=0) {
      m_currentBoWText->clear();
    }
    else {
      LERROR << "<tokens> found whereas no BoWText initialized";
    }
  }
  else if (stringName == "bowToken") {
    getTokenAttributes(attributes,lemma,category,position,length,id);
    BoWToken* token=new BoWToken(lemma,category,position,length);
    m_refMap[id]=token;
    if (m_currentComplexToken.empty()) {
      m_currentBoWText->push_back(token);
    }
    else { 
      if (m_currentComplexToken.back().currentPart == 
          m_currentComplexToken.back().head) {
        m_currentComplexToken.back().token->addPart(token,false,true);
      }
      else {
        m_currentComplexToken.back().token->addPart(token,false);
      }
      m_currentComplexToken.back().currentPart++;
      // token has been cloned in complex token
      delete token;
    }
  }
  else if (stringName == "bowTerm") {
    getTokenAttributes(attributes,lemma,category,position,length,id);
    // use empty lemma: no need to store lemma for compound
    BoWTerm* term=new BoWTerm(LimaString(),category,position,length);
    m_refMap[id]=term;
    m_currentComplexToken.push_back(CurrentComplexToken(term));
  }
  else if (stringName == "bowNamedEntity") {
    getTokenAttributes(attributes,lemma,category,position,length,id);
    LimaString typeName=getLimaStringAttribute(attributes,"type");
    // use empty lemma: no need to store lemma for compound
    BoWNamedEntity* ne=new BoWNamedEntity(LimaString(),category,
                                          MediaticData::MediaticData::single().getEntityType(typeName),
                                          position,length);
    m_refMap[id]=ne;
    m_currentComplexToken.push_back(CurrentComplexToken(ne));
  } 
  else if (stringName == "parts") {
    m_currentComplexToken.back().head=getIntAttribute(attributes,"head");
    m_currentComplexToken.back().currentPart=0;
  }
  else if (stringName == "bowTokenRef") {
    uint64_t refId=getIntAttribute(attributes,"refId");
    m_currentComplexToken.back().token->addPart(m_refMap[refId],true);
  }
  else if (stringName == "feature") {
    std::string name=getStringAttribute(attributes,"name");
    LimaString value=getLimaStringAttribute(attributes,"value");
    static_cast<BoWNamedEntity*>(m_currentComplexToken.back().token)->
      addFeature(name,value);
  }
  return true;
}

bool BoWXMLHandler::endElement(const QString & namespaceURI, const QString & name, const QString & qName) {
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  const QString& stringName = name;

  BOWLOGINIT;
  LDEBUG << "End Element " << stringName;

  if (stringName == "bowNamedEntity" ||
      stringName == "bowTerm") 
  {
    BoWToken* token=m_currentComplexToken.back().token;
    m_currentComplexToken.pop_back();
    if (m_currentComplexToken.empty()) {
      m_currentBoWText->push_back(token);
    }
    else { // this complex token is a part of another
      if (m_currentComplexToken.back().currentPart == 
          m_currentComplexToken.back().head) {
        m_currentComplexToken.back().token->addPart(token,false,true);
      }
      else {
        m_currentComplexToken.back().token->addPart(token,false);
      }
      m_currentComplexToken.back().currentPart++;
      // token has been cloned in addPart => delete it
      delete token;
    }
  }
  else if (stringName == "properties") {
    m_outputStream << NODE_PROPERTIES_BLOC;
    m_currentProperties.write(m_outputStream);
  }
  else if (stringName == "tokens") {
    m_outputStream << BOW_TEXT_BLOC;
    //@todo
    //m_currentBoWText->write(m_outputStream);
  }
  else if (stringName == "hierarchy") {
    m_outputStream << END_BLOC;
    if (m_currentBoWText !=0) {
      BoWBinaryWriter writer;
      writer.writeBoWText(m_outputStream,*m_currentBoWText);
      delete m_currentBoWText;
      m_currentBoWText=0;
    }
  }
  else if (stringName == "bowDocument") {
    //@todo
    // m_currentBoWDocument.write(m_outputStream);
    m_currentBoWText=0;
  }
  return true;
}

// -----------------------------------------------------------------------
//  helper functions for parsing
// -----------------------------------------------------------------------
bool BoWXMLHandler::addProperty(const QXmlAttributes& attributes) {
  string propName=getStringAttribute(attributes,"name");
  string typName=getStringAttribute(attributes,"type");
  try {
    if (typName == "int") {
      m_currentProperties.setIntValue(propName, getIntAttribute(attributes,"value")); 
    }
    else if (typName == "string") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (typName == "date") {
      m_currentProperties.setDateValue(propName, getDateAttribute(attributes,"value")); 
    }
    else if (typName == "dateRange") {
      QDate dateBegin = getDateAttribute(attributes,"valueBegin");
      QDate dateEnd = getDateAttribute(attributes,"valueEnd");
      m_currentProperties.setDateIntervalValue( propName, make_pair(dateBegin, dateEnd));
    }
    else if (typName == "multString") {
      m_currentProperties.addStringValue( propName, getStringAttribute(attributes,"value")); 
    }
    else if (typName == "multWeightedId") {
      string val = getStringAttribute(attributes,"value"); 
      float score = getFloatAttribute(attributes,"weight"); 
      m_currentProperties.addWeightedPropValue( propName, make_pair(val,score));
    }
/*      
    if (propName == "srcePrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "offBegPrpty") {
      m_currentProperties.setIntValue(propName, getIntAttribute(attributes,"value")); 
    }
    else if (propName == "offEndPrpty") {
      m_currentProperties.setIntValue(propName, getIntAttribute(attributes,"value")); 
    }
    else if (propName == "indexDatePrpty") {
      m_currentProperties.setDateValue(propName, getDateAttribute(attributes,"value")); 
    }
    else if (propName == "langPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "encodPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "identPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "docintref") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "titlePrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "date_begin") {
      // ruse!!
      std::pair<std::pair<QDate,QDate>, bool> creationDateRet =
         m_currentProperties.getDateIntervalValue(std::string("datePrpty"));
      
      m_currentProperties.setDateIntervalValue(std::string("datePrpty"),
                                               make_pair(getDateAttribute(attributes,"value"), 
                                                         creationDateRet.first.second ));
    }
    else if (propName == "date_end") {
      std::pair<std::pair<QDate,QDate>, bool> creationDateRet =
         m_currentProperties.getDateIntervalValue(std::string("datePrpty"));
      m_currentProperties.setDateIntervalValue(std::string("datePrpty"),
                                               make_pair(creationDateRet.first.first,
                                                         getDateAttribute(attributes,"value")));
    }
    else if (propName == "typPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "originPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "locationPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "countryPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "senderPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
    else if (propName == "privacyPrpty") {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes,"value")); 
    }
*/  
  }
  catch (exception& e) {
    BOWLOGINIT;
    LERROR << "Error reading doc infos: " << e.what();
  }
  return true;
}

uint64_t 
BoWXMLHandler::getTokenAttributes(const QXmlAttributes& attributes,
                                  LimaString& lemma,
                                  LinguisticCode& category,
                                  uint64_t& position,
                                  uint64_t& length,
                                  uint64_t& id) const {
  lemma=getLimaStringAttribute(attributes,"lemma");
  category=getIntAttribute(attributes,"category");
  position=getIntAttribute(attributes,"position");
  length=getIntAttribute(attributes,"length");
  id=getIntAttribute(attributes,"id");
  return 0;
}

uint64_t 
BoWXMLHandler::getIntAttribute(const QXmlAttributes& attributes,
                               const char* name) const {
  // do not make conversion at once in case the getValue fails
  const QString& chars=attributes.value(name);
  
  if (chars==0) {
    ostringstream oss;
    oss << "expected attribute \""<<name<<"\" not found";
    throw NoAttributeException(oss.str());
  }
  
  return chars.toULong();
}

float
BoWXMLHandler::getFloatAttribute(const QXmlAttributes& attributes,
                               const char* name) const {
  // do not make conversion at once in case the getValue fails
  const QString& chars=attributes.value(name);
  
  if (chars==0) {
    ostringstream oss;
    oss << "expected attribute \""<<name<<"\" not found";
    throw NoAttributeException(oss.str());
  }
  
  return chars.toFloat();
}

std::string 
BoWXMLHandler::getStringAttribute(const QXmlAttributes& attributes,
                                  const char* name) const 
{
  return limastring2utf8stdstring(getLimaStringAttribute(attributes,name));
}

QDate 
BoWXMLHandler::getDateAttribute(const QXmlAttributes& attributes,
                                  const char* name) const {
  
  const QString& chars=attributes.value(name);

  if (chars==0) {
    ostringstream oss;
    oss << "expected attribute \""<<name<<"\" not found";
    throw NoAttributeException(oss.str());
  }

  const QString& strDate=chars;

  if (strDate == "NotADate") {
    return QDate();
  }

  try {
    return QDate::fromString(strDate,"yyyyMMdd");
  }
  catch (exception& e) {
    BOWLOGINIT;
    LERROR << "Error trying to read date: " << e.what(); 
    return QDate();
  }    
}

LimaString
BoWXMLHandler::getLimaStringAttribute(const QXmlAttributes& attributes,
                                       const char* name) const {
  
  QString chars=attributes.value(name);

  if (chars.isEmpty()) {
    ostringstream oss;
    oss << "expected attribute \""<<name<<"\" not found";
    throw NoAttributeException(oss.str());
  }

  return chars;
}

} // end namespace
} // end namespace
} // end namespace
