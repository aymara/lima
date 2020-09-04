/************************************************************************
 *
 * @file       MultXmlHandler.cpp
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     lima_xmlprocessings
 *
 * Handler for parsing XML representation of Amose analysed
 * multimedia documents
 *
 ***********************************************************************/

#include "MultXmlHandler.h"

#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"

#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocumentST.h"
#include <linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h>
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentHandler.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::BagOfWords;
using namespace std;
using namespace Lima::Handler;

namespace Lima {
namespace XmlReader {


//***********************************************************************
// local exception
//***********************************************************************
class NoAttributeException : public runtime_error {
public:
  NoAttributeException(const string& mess):runtime_error(mess) {}
};

class MultXmlHandlerPrivate
{
friend class MultXmlHandler;
public:
  MultXmlHandlerPrivate(std::ostream& output);
  ~MultXmlHandlerPrivate();

  class CurrentComplexToken {
  public:
    CurrentComplexToken():token(),currentPart(0),head() {}
    CurrentComplexToken(boost::shared_ptr< BoWComplexToken > tok):
      token(tok),currentPart(0),head(0) {}
    boost::shared_ptr< BoWComplexToken > token;
    uint64_t currentPart;
    uint64_t head;
  };

  MultimediaDocumentHandler m_handler;
  std::ostream& m_outputStream;
  BoWDocument m_currentBoWDocument;
  BoWText* m_currentBoWText;
  boost::shared_ptr< BoWRelation > m_currentBoWRelation;
  Common::Misc::GenericDocumentProperties m_currentProperties;
  std::vector<CurrentComplexToken> m_currentComplexToken;
  std::map<uint64_t, boost::shared_ptr< BoWToken > > m_refMap;
  std::map< boost::shared_ptr< BoWToken >, boost::shared_ptr< BoWRelation > > m_relMap;

  // private functions
  uint64_t getIntAttribute(const QXmlAttributes& attributes,
                    const char* name) const;
  float getFloatAttribute(const QXmlAttributes& attributes,
                               const char* name) const;

  QString getStringAttribute(const QXmlAttributes& attributes,
                       const char* name) const;
  QDate getDateAttribute(const QXmlAttributes& attributes,
                     const char* name) const;

  bool addProperty(const QXmlAttributes& attributes);

  uint64_t getTokenAttributes(const QXmlAttributes& attributes,
                       LimaString& lemma,
                       LinguisticCode& category,
                       uint64_t& position,
                       uint64_t& length,
                       uint64_t& id) const;
};

MultXmlHandlerPrivate::MultXmlHandlerPrivate(std::ostream& output):
m_handler(),
m_outputStream(output),
m_currentBoWDocument(),
m_currentBoWText(0),
m_currentBoWRelation(),
m_currentComplexToken(),
m_refMap()
{
  m_handler.setOut(&output);;
}

MultXmlHandlerPrivate::~MultXmlHandlerPrivate() {
}

//**********************************************************************
// xerces handler functions
//**********************************************************************
MultXmlHandler::MultXmlHandler(std::ostream& output):
    m_d(new MultXmlHandlerPrivate(output))
{
}

MultXmlHandler::~MultXmlHandler()
{
  delete m_d;
}

// -----------------------------------------------------------------------
//  SAX ErrorHandler interface
// -----------------------------------------------------------------------
bool MultXmlHandler::warning(const QXmlParseException& e) {
  BOWLOGINIT;
  LWARN << "Warning at file " << e.systemId()
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << e.message();
  return true;
}
bool MultXmlHandler::error(const QXmlParseException& e) {
  BOWLOGINIT;
  LERROR << "Error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
  return false;
}
bool MultXmlHandler::fatalError(const QXmlParseException& e) {
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

bool MultXmlHandler::startDocument() {
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "Start Document";
#endif
  return true;
}
bool MultXmlHandler::endDocument() {
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "End Document";
#endif
  return true;
}

bool MultXmlHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes& attributes) {
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);

#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlHandler::startElement" << name;
#endif

  LimaString lemma;
  LinguisticCode category = static_cast<LinguisticCode>(0);
  uint64_t position(0);
  uint64_t length(0);
  uint64_t id(0);

  if (name == "MultimediaDocuments")
  {
    m_d->m_handler.startDocument(m_d->m_currentProperties);
  }
  else if (name == "node")
  {
    bool isIndexingNode(false);
    try
    {
      QString indexingNode=m_d->getStringAttribute(attributes,"indexingNode");
      if (indexingNode == "yes")
      {
        isIndexingNode=true;
      }
    }
    catch (NoAttributeException& )
    {
      // do nothing -> is not indexing node
    }
    QString elementName("");
    try
    {
      elementName=m_d->getStringAttribute(attributes,"elementName");
    }
    catch (NoAttributeException& )
    {
      BOWLOGINIT;
      LERROR << "missing attribute elementName in " << name;
    }
    m_d->m_handler.startNode(elementName.toUtf8().constData(),isIndexingNode);
  }
  else if (name == "content")
  {
    try
    {
      QString type=m_d->getStringAttribute(attributes,"type");
      if (type == "tokens")
      {
        m_d->m_currentBoWText=new BoWText();
      }
      // FIXME Handle other medias here
    }
    catch (NoAttributeException& )
    {
    }
  }
  else if (name == "property")
  {
    m_d->addProperty(attributes);
  }
  else if (name == "properties")
  {
    m_d->m_currentProperties.reinit();
  }
  else if (name == "tokens")
  {
    if (m_d->m_currentBoWText !=0)
    {
      m_d->m_currentBoWText->clear();
    }
    else
    {
      BOWLOGINIT;
      LERROR << "<tokens> found whereas no BoWText initialized";
    }
  }
  else if (name == "bowRelation")
  {
    QString realization;
    try {
      realization=m_d->getStringAttribute(attributes,"realization");
    } catch (const NoAttributeException& e) {
    }
    uint64_t type = m_d->getIntAttribute(attributes,"type");
    m_d->m_currentBoWRelation = boost::shared_ptr< BoWRelation >(new BoWRelation(realization,type));
  }
  else if (name == "bowToken")
  {
    m_d->getTokenAttributes(attributes,lemma,category,position,length,id);
    boost::shared_ptr< BoWToken > token(new BoWToken(lemma,category,position,length));
    m_d->m_refMap[id]=token;
    if (m_d->m_currentComplexToken.empty())
    {
      m_d->m_currentBoWText->push_back(token);
    }
    else
    {
      bool isHead = (m_d->m_currentComplexToken.back().currentPart ==
          m_d->m_currentComplexToken.back().head);
      if (m_d->m_currentBoWRelation != 0)
      {
#ifdef DEBUG_LP
        LDEBUG << "addPart with relation";
#endif
        m_d->m_currentComplexToken.back().token->addPart(m_d->m_currentBoWRelation,token,isHead);
        m_d->m_currentBoWRelation = boost::shared_ptr< BoWRelation >();
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "addPart without relation";
#endif
        m_d->m_currentComplexToken.back().token->addPart(token,isHead);
      }
      m_d->m_currentComplexToken.back().currentPart++;
      // token has been cloned in complex token
    }
  }
  else if (name == "bowTerm")
  {
    m_d->getTokenAttributes(attributes,lemma,category,position,length,id);
    // use empty lemma: no need to store lemma for compound
    boost::shared_ptr< BoWTerm > term(new BoWTerm(LimaString(),category,position,length));
    m_d->m_refMap[id]=term;
    m_d->m_currentComplexToken.push_back(MultXmlHandlerPrivate::CurrentComplexToken(term));
#ifdef DEBUG_LP
    LDEBUG << "set relMap["<<m_d->m_currentComplexToken.back().currentPart<<"]="<< *m_d->m_currentBoWRelation;
#endif
    m_d->m_relMap[term]=m_d->m_currentBoWRelation;
    m_d->m_currentBoWRelation = boost::shared_ptr< BoWRelation >();
  }
  else if (name == "bowNamedEntity")
  {
    m_d->getTokenAttributes(attributes,lemma,category,position,length,id);
    QString typeName=m_d->getStringAttribute(attributes,"type");
#ifdef DEBUG_LP
    LDEBUG << "bowNamedEntity typeName=" << typeName;
#endif
    // use empty lemma: no need to store lemma for compound
    boost::shared_ptr< BoWNamedEntity > ne(new BoWNamedEntity(LimaString(),category,
                                          MediaticData::MediaticData::single().getEntityType(typeName),
                                          position,length));
    m_d->m_refMap[id]=ne;
    m_d->m_currentComplexToken.push_back(MultXmlHandlerPrivate::CurrentComplexToken(ne));
    m_d->m_relMap[ne]=m_d->m_currentBoWRelation;
    m_d->m_currentBoWRelation = boost::shared_ptr< BoWRelation >();
  }
  else if (name == "parts")
  {
    m_d->m_currentComplexToken.back().head=m_d->getIntAttribute(attributes,"head");
    m_d->m_currentComplexToken.back().currentPart=0;
  }
  else if (name == "bowTokenRef")
  {
    uint64_t refId=m_d->getIntAttribute(attributes,"refId");
    m_d->m_currentComplexToken.back().token->addPart(m_d->m_refMap[refId],true);
  }
  else if (name == "feature")
  {
    QString name=m_d->getStringAttribute(attributes,"name");
    QString value=m_d->getStringAttribute(attributes,"value");
    boost::dynamic_pointer_cast<BoWNamedEntity>(m_d->m_currentComplexToken.back().token)->
      setFeature(name.toUtf8().constData(),value.toUtf8().constData());
  }
  return true;
}

bool MultXmlHandler::endElement(const QString & namespaceURI, const QString & name, const QString & qName) {
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);

#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlHandler::endElement" << name;
#endif

  if (name == "MultimediaDocuments")
  {
    m_d->m_handler.endDocument();
  }
  else if (name == "node")
  {
    m_d->m_handler.endNode(m_d->m_currentProperties);
    m_d->m_refMap.clear();
    m_d->m_relMap.clear();
  }
  else if (name == "bowNamedEntity" || name == "bowTerm")
  {
    boost::shared_ptr< BoWToken > token=m_d->m_currentComplexToken.back().token;
    uint64_t id = m_d->m_currentComplexToken.back().currentPart;
    boost::shared_ptr< BoWRelation > relation = (m_d->m_relMap.find(token)==m_d->m_relMap.end())?boost::shared_ptr< BoWRelation >():(m_d->m_relMap[token]);
#ifdef DEBUG_LP
    LDEBUG << "id=" << id << "; relation=" << *relation;
#endif
    m_d->m_currentComplexToken.pop_back();
    if (m_d->m_currentComplexToken.empty())
    {
      m_d->m_currentBoWText->push_back(token);
    }
    else
    { // this complex token is a part of another
      bool isHead = (id == m_d->m_currentComplexToken.back().head);
      if (relation != 0)
      {
#ifdef DEBUG_LP
        LDEBUG << "addPart with relation";
#endif
        m_d->m_currentComplexToken.back().token->addPart(relation,token,isHead);
        m_d->m_relMap.erase(token);
      }
      else
      {
#ifdef DEBUG_LP
        LDEBUG << "addPart without relation";
#endif
        m_d->m_currentComplexToken.back().token->addPart(token,isHead);
      }
      m_d->m_currentComplexToken.back().currentPart++;
      // token has been cloned in addPart => delete it
    }
  }
  else if (name == "properties") {
//     m_d->m_outputStream << NODE_PROPERTIES_BLOC;
//     m_d->m_currentProperties.write(m_d->m_outputStream);
    // Closing a properties token with a langPrpty property when
    // m_currentBoWText is not null means that we just read this text
    // properties, and particularly its language. We can now set its lang
    // write it to the stream
    std::pair<std::string,bool> langPrpty = m_d->m_currentProperties.getStringValue("langPrpty");
    if (langPrpty.second && m_d->m_currentBoWText != 0)
    {
      m_d->m_currentBoWText->lang = langPrpty.first;

      Misc::writeOneByteInt(m_d->m_outputStream,BOW_TEXT_BLOC);
      BoWBinaryWriter writer;
      writer.writeBoWText(m_d->m_outputStream,*m_d->m_currentBoWText);
      delete m_d->m_currentBoWText;
      m_d->m_currentBoWText = 0;
    }
  }
  else if (name == "content")
  {
  }
  return true;
}

bool  MultXmlHandler::startEntity(const QString & name)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlHandler::startEntity" << name;
#else
  LIMA_UNUSED(name);
#endif

 return true;
}

bool  MultXmlHandler::endEntity(const QString & name)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlHandler::endEntity" << name;
#else
  LIMA_UNUSED(name);
#endif

  return true;
}

// -----------------------------------------------------------------------
//  helper functions for parsing
// -----------------------------------------------------------------------
bool MultXmlHandlerPrivate::addProperty(const QXmlAttributes& attributes) {
  QString propName=getStringAttribute(attributes,"name");
  QString typName=getStringAttribute(attributes,"type");
  try {
    if (typName == "int") {
      m_currentProperties.setIntValue(propName.toUtf8().constData(), getIntAttribute(attributes,"value"));
    }
    else if (typName == "string") {
      m_currentProperties.setStringValue(propName.toUtf8().constData(), getStringAttribute(attributes,"value").toUtf8().constData());
    }
    else if (typName == "date") {
      m_currentProperties.setDateValue(propName.toUtf8().constData(), getDateAttribute(attributes,"value"));
    }
    else if (typName == "dateRange") {
      QDate dateBegin = getDateAttribute(attributes,"valueBegin");
      QDate dateEnd = getDateAttribute(attributes,"valueEnd");
      m_currentProperties.setDateIntervalValue( propName.toUtf8().constData(), make_pair(dateBegin, dateEnd));
    }
    else if (typName == "multString") {
      m_currentProperties.addStringValue( propName.toUtf8().constData(), getStringAttribute(attributes,"value").toUtf8().constData());
    }
    else if (typName == "multWeightedId") {
      QString val = getStringAttribute(attributes,"value");
      float score = getFloatAttribute(attributes,"weight");
      m_currentProperties.addWeightedPropValue( propName.toUtf8().constData(), make_pair(val.toUtf8().constData(),score));
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

uint64_t MultXmlHandlerPrivate::getTokenAttributes(const QXmlAttributes& attributes,
                                  LimaString& lemma,
                                  LinguisticCode& category,
                                  uint64_t& position,
                                  uint64_t& length,
                                  uint64_t& id) const {
  lemma=getStringAttribute(attributes,"lemma");
  category=getIntAttribute(attributes,"category");
  position=getIntAttribute(attributes,"position");
  length=getIntAttribute(attributes,"length");
  id=getIntAttribute(attributes,"id");
  return 0;
}

uint64_t MultXmlHandlerPrivate::getIntAttribute(const QXmlAttributes& attributes,
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

float MultXmlHandlerPrivate::getFloatAttribute(const QXmlAttributes& attributes,
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

QDate MultXmlHandlerPrivate::getDateAttribute(const QXmlAttributes& attributes,
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

QString MultXmlHandlerPrivate::getStringAttribute(const QXmlAttributes& attributes,
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
