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

#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"

#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocumentST.h"
#include <linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h>
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentHandler.h"

#include <QDate>
#include <QFile>
#include <QXmlStreamReader>

using namespace Lima::Common::BagOfWords;
using namespace Lima;
using namespace Lima::Handler;

namespace Lima {
namespace XmlReader {

class NoAttributeException : public std::runtime_error {
public:
  NoAttributeException(const std::string& mess): std::runtime_error(mess) {}
};

class MultXmlReaderPrivate
{
friend class MultXmlReader;
public:
  MultXmlReaderPrivate(const std::string& filename, std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom);
  ~MultXmlReaderPrivate() = default;

  bool parse(QIODevice *device);
  QString errorString() const;

  void readMultimediaDocuments();
  void readNode();
  void readContent();
  void readProperty();
  void readProperties();
  void readTokens();
  void readBowRelation();
  void readBowToken();
  void readBowTerm();
  void readBowNamedEntity();
  void readParts();
  void readBowTokenRef();
  void readFeature();

  class CurrentComplexToken
  {
  public:
    CurrentComplexToken(): token(), currentPart(0), head() {}
    CurrentComplexToken(boost::shared_ptr< BoWComplexToken > tok):
      token(tok), currentPart(0), head(0) {}
    boost::shared_ptr< BoWComplexToken > token;
    uint64_t currentPart;
    uint64_t head;
  };

  LimaString lemma;
  LinguisticCode category;
  uint64_t position;
  uint64_t length;
  uint64_t id;

  QXmlStreamReader m_reader;
  MultimediaDocumentHandler m_handler;
  std::ostream& m_outputStream;
  BoWDocument m_currentBoWDocument;
  boost::shared_ptr< BoWText > m_currentBoWText;
  boost::shared_ptr< BoWRelation > m_currentBoWRelation;
  Common::Misc::GenericDocumentProperties m_currentProperties;
  std::vector<CurrentComplexToken> m_currentComplexToken;
  std::map<uint64_t, boost::shared_ptr< BoWToken > > m_refMap;
  std::map< boost::shared_ptr< BoWToken >, boost::shared_ptr< BoWRelation > > m_relMap;

  // private functions
  uint64_t getIntAttribute(const QXmlStreamReader& attributes, const char* name) const;
  float getFloatAttribute(const QXmlStreamReader& attributes, const char* name) const;
  std::string getStringAttribute(const QXmlStreamReader& attributes, const char* name) const;
  QString getQStringAttribute(const QXmlStreamReader& attributes, const char* name) const;
  QDate getDateAttribute(const QXmlStreamReader& attributes, const char* name) const;

  bool addProperty(const QXmlStreamReader& attributes);

  uint64_t
    getTokenAttributes(const QXmlStreamReader& attributes,
                       LimaString& lemma,
                       LinguisticCode& category,
                       uint64_t& position,
                       uint64_t& length,
                       uint64_t& id) const;
};

MultXmlReaderPrivate::MultXmlReaderPrivate(const std::string& filename,
                           std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom):
    m_reader(),
    m_handler(shiftFrom),
    m_outputStream(output),
    m_currentBoWDocument(),
    m_currentBoWText(0),
    m_currentBoWRelation(),
    m_currentComplexToken(),
    m_refMap()
{
  m_handler.setOut(&output);;

  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    LIMA_EXCEPTION_SELECT_LOGINIT(BOWLOGINIT,
                                  "Cannot open" << filename.c_str(),
                                  XMLException);
  if (!parse(&file))
  {
    LIMA_EXCEPTION_SELECT_LOGINIT(
      BOWLOGINIT,
      "Cannot parse" << filename.c_str() << errorString,
      XMLException);
  }
}

MultXmlReader::MultXmlReader(const std::string& filename, std::ostream& output,
                             std::shared_ptr<const ShiftFrom> shiftFrom):
    m_d(new MultXmlReaderPrivate(filename, output, shiftFrom))
{
}

MultXmlReader::~MultXmlReader()
{
  delete m_d;
}

bool MultXmlReaderPrivate::parse(QIODevice *device)
{
  BOWLOGINIT;
  LDEBUG << "MultXmlReaderPrivate::parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("MultimediaDocuments"))
      {
          readMultimediaDocuments();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA BoW XML file."));
      }
  }
  return !m_reader.error();
}

// <?xml-stylesheet type="text/xsl" href="bow.xslt"?>
// <MultimediaDocuments>
//   <node elementName="DOCSET">
//     <node elementName="DOC">
//       <node elementName="TI" indexingNode="yes">
//         <content type="tokens">
//           <tokens>
//             <bowNamedEntity id="1" lemma="Italie" category="NP" position="280" length="6" type="Location.LOCATION">
//               <parts head="0">
//                 <bowToken id="2" lemma="Italie" category="NP" position="280" length="6"/>
//               </parts>
//               <feature name="name" value="Italie"/>
//             </bowNamedEntity>
//             <bowToken id="3" lemma="" category="NP" position="280" length="6"/>
//             <bowTerm id="9" lemma="monarchiste_démocratique" category="NC" position="308" length="24">
//               <parts head="0">
//                 <bowToken id="10" lemma="monarchiste" category="NC" position="321" length="11"/>
//                 <bowRelation realization="" type="3"/>
//                 <bowToken id="11" lemma="démocratique" category="ADJ" position="308" length="12"/>
//               </parts>
//             </bowTerm>
//           </tokens>
//           <properties>
//             <property name="ContentId" type="int" value="1"/>
//             …
//           </properties>
//         </content>
//         <properties>
//           <property name="ContentId" type="int" value="1"/>
//           …
//         </properties>
//       </node>
//       <properties>
//         <property name="ContentId" type="int" value="0"/>
//         …
//       </properties>
//     </node>
//       <properties>
//         <property name="ContentId" type="int" value="137"/>
//         …
//       </properties>
//     </node>
//     <properties>
//       <property name="ContentId" type="int" value="0"/>
//       …
//     </properties>
//   </node>
// </MultimediaDocuments>
void MultXmlReaderPrivate::readMultimediaDocuments()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowDocument" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("MultimediaDocuments"));

  m_currentBoWDocument.clear();

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("node"))
          readNode();
      else if (m_reader.name() == QLatin1String("properties"))
          readProperties();
      else
          m_reader.raiseError(QObject::tr("Expected a bowText or a hierarchy but got a %1.").arg(m_reader.name()));
  }
  m_currentBoWText=boost::shared_ptr <Lima::Common::BagOfWords::BoWText >();
  m_handler.endDocument();
}

//   <node elementName="DOCSET">
//     <node elementName="DOC">
//       <node elementName="TI" indexingNode="yes">
//         <content type="tokens">
void MultXmlReaderPrivate::readNode()
{

  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readNode" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("node"));

  auto indexingNode = (getStringAttribute(m_reader, "indexingNode") == "yes");
  auto elementName = getQStringAttribute(m_reader, "elementName");
  m_handler.startNode(elementName.toStdString(), indexingNode);

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("node"))
          readNode();
      else if (m_reader.name() == QLatin1String("content"))
          readContent();
      else if (m_reader.name() == QLatin1String("properties"))
          readProperties();
      else
          m_reader.raiseError(QObject::tr("Expected a tokens but got a %1.").arg(m_reader.name()));
  }
  m_handler.endNode(m_currentProperties);
  m_refMap.clear();
  m_relMap.clear();
}

//         <content type="tokens">
//           <tokens>
//             <bowNamedEntity id="1" lemma="Italie" category="NP" position="280" length="6" type="Location.LOCATION">
//               <parts head="0">
//                 <bowToken id="2" lemma="Italie" category="NP" position="280" length="6"/>
//               </parts>
//               <feature name="name" value="Italie"/>
//             </bowNamedEntity>
//             <bowToken id="3" lemma="" category="NP" position="280" length="6"/>
//             <bowTerm id="9" lemma="monarchiste_démocratique" category="NC" position="308" length="24">
//               <parts head="0">
//                 <bowToken id="10" lemma="monarchiste" category="NC" position="321" length="11"/>
//                 <bowRelation realization="" type="3"/>
//                 <bowToken id="11" lemma="démocratique" category="ADJ" position="308" length="12"/>
//               </parts>
//             </bowTerm>
//           </tokens>
//           <properties>
//             <property name="ContentId" type="int" value="1"/>
//             …
//           </properties>
//         </content>
void MultXmlReaderPrivate::readContent()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readContent" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("content"));

  auto contentType = getQStringAttribute(m_reader, "type");
  if (contentType == "tokens")
  {
    m_currentBoWText.reset(new BoWText());
    while (m_reader.readNextStartElement())
    {
        if (m_reader.name() == QLatin1String("tokens"))
            readTokens();
        else if (m_reader.name() == QLatin1String("properties"))
            readProperties();
        else
            m_reader.raiseError(QObject::tr("Expected a tokens or a properties but got a %1.").arg(m_reader.name()));
    }
  }

  Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::END_BLOC);
  if (m_currentBoWText !=0)
  {
    BoWBinaryWriter writer;
    writer.writeBoWText(m_outputStream, *m_currentBoWText);
    m_currentBoWText = boost::shared_ptr< BoWText >();
  }
}

//           <properties>
//             <property name="ContentId" type="int" value="1"/>
//             …
//           </properties>
void MultXmlReaderPrivate::readProperties()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readProperties" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("properties"));

  m_currentProperties.reinit();
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("property"))
          readProperty();
      else
          m_reader.raiseError(QObject::tr("Expected a property but got a %1.").arg(m_reader.name()));
  }

//     m_outputStream << NODE_PROPERTIES_BLOC;
//     m_currentProperties.write(m_outputStream);
  // Closing a properties token with a langPrpty property when
  // m_currentBoWText is not null means that we just read this text
  // properties, and particularly its language. We can now set its lang
  // write it to the stream
  auto langPrpty = m_currentProperties.getStringValue("langPrpty");
  if (langPrpty.second && m_currentBoWText != 0)
  {
    m_currentBoWText->lang = langPrpty.first;

    Common::Misc::writeOneByteInt(m_outputStream,BOW_TEXT_BLOC);
    BoWBinaryWriter writer;
    writer.writeBoWText(m_outputStream,*m_currentBoWText);
    m_currentBoWText = 0;
  }

}

//             <property name="ContentId" type="int" value="1"/>
void MultXmlReaderPrivate::readProperty()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readProperty" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("property"));

  addProperty(m_reader);
  m_reader.skipCurrentElement();
}

//           <tokens>
//             <bowNamedEntity id="1" lemma="Italie" category="NP" position="280" length="6" type="Location.LOCATION">
//               <parts head="0">
//                 <bowToken id="2" lemma="Italie" category="NP" position="280" length="6"/>
//               </parts>
//               <feature name="name" value="Italie"/>
//             </bowNamedEntity>
//             <bowToken id="3" lemma="" category="NP" position="280" length="6"/>
//             <bowTerm id="9" lemma="monarchiste_démocratique" category="NC" position="308" length="24">
//               <parts head="0">
//                 <bowToken id="10" lemma="monarchiste" category="NC" position="321" length="11"/>
//                 <bowRelation realization="" type="3"/>
//                 <bowToken id="11" lemma="démocratique" category="ADJ" position="308" length="12"/>
//               </parts>
//             </bowTerm>
//           </tokens>
void MultXmlReaderPrivate::readTokens()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readTokens" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("tokens"));

  if (m_currentBoWText != nullptr)
  {
    m_currentBoWText->clear();
  }
  else
  {
    LERROR << "<tokens> found whereas no BoWText initialized";
  }
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("bowToken"))
          readBowToken();
      else if (m_reader.name() == QLatin1String("bowTerm"))
          readBowTerm();
      else if (m_reader.name() == QLatin1String("bowNamedEntity"))
          readBowNamedEntity();
      else if (m_reader.name() == QLatin1String("bowRelation"))
          readBowRelation();
      else
          m_reader.raiseError(QObject::tr("Expected a bowToken, bowTerm, bowRelation or a bowNamedEntity but got a %1.")
                                .arg(m_reader.name()));
  }
  Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::BOW_TEXT_BLOC);
  //@todo
  m_currentBoWText->writeBoWText(m_outputStream);
}

//                 <bowRelation realization="" type="3"/>
void MultXmlReaderPrivate::readBowRelation()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowRelation" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowRelation"));

  auto realization = getQStringAttribute(m_reader, "realization");
  auto type = getIntAttribute(m_reader, "type");
  m_currentBoWRelation = boost::shared_ptr< BoWRelation >(new BoWRelation(realization,type));
  m_reader.skipCurrentElement();
}

//             <bowToken id="3" lemma="" category="NP" position="280" length="6"/>
void MultXmlReaderPrivate::readBowToken()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowToken" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowToken"));

  getTokenAttributes(m_reader, lemma, category, position, length, id);
  LDEBUG << lemma << category << position << length << id ;
  auto token = boost::shared_ptr< BoWToken >(new BoWToken(lemma,category,position,length));
  m_refMap[id] = token;
  if (m_currentComplexToken.empty())
  {
    m_currentBoWText->push_back(token);
  }
  else
  {
    bool isHead = (m_currentComplexToken.back().currentPart == m_currentComplexToken.back().head);
    if (m_currentBoWRelation != 0)
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart with relation";
#endif
      m_currentComplexToken.back().token->addPart(m_currentBoWRelation, token, isHead);
      m_currentBoWRelation = boost::shared_ptr< BoWRelation >();
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart without relation";
#endif
      m_currentComplexToken.back().token->addPart(token,isHead);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in complex token
  }
  m_reader.skipCurrentElement();
}


//             <bowTerm id="9" lemma="monarchiste_démocratique" category="NC" position="308" length="24">
//               <parts head="0">
//                 <bowToken id="10" lemma="monarchiste" category="NC" position="321" length="11"/>
//                 <bowRelation realization="" type="3"/>
//                 <bowToken id="11" lemma="démocratique" category="ADJ" position="308" length="12"/>
//               </parts>
//             </bowTerm>
void MultXmlReaderPrivate::readBowTerm()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowTerm" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowTerm"));

  getTokenAttributes(m_reader, lemma, category, position, length, id);
  // use empty lemma: no need to store lemma for compound
  auto term = boost::shared_ptr< BoWTerm >(new BoWTerm(QString(), category, position, length));
  m_refMap[id] = term;
  m_currentComplexToken.push_back(CurrentComplexToken(term));
  m_relMap[term]=m_currentBoWRelation;
  m_currentBoWRelation = boost::shared_ptr< BoWRelation >();

  while (m_reader.readNextStartElement())
  {
      if (m_reader.name() == QLatin1String("parts"))
          readParts();
      else
          m_reader.raiseError(QObject::tr("Expected a parts but got a %1.").arg(m_reader.name()));
  }

  auto token = m_currentComplexToken.back().token;
  uint64_t id = m_currentComplexToken.back().currentPart;
  boost::shared_ptr< BoWRelation > relation = (m_relMap.find(token)==m_relMap.end())?boost::shared_ptr< BoWRelation >():(m_relMap[token]);
#ifdef DEBUG_LP
  LDEBUG << "id=" << id << "; relation=" << *relation;
#endif
  m_currentComplexToken.pop_back();
  if (m_currentComplexToken.empty())
  {
    m_currentBoWText->push_back(token);
  }
  else
  { // this complex token is a part of another
    bool isHead = (id == m_currentComplexToken.back().head);
    if (relation != 0)
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart with relation";
#endif
      m_currentComplexToken.back().token->addPart(relation,token,isHead);
      m_relMap.erase(token);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart without relation";
#endif
      m_currentComplexToken.back().token->addPart(token,isHead);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in addPart => delete it
  }
}

//             <bowNamedEntity id="1" lemma="Italie" category="NP" position="280" length="6" type="Location.LOCATION">
//               <parts head="0">
//                 <bowToken id="2" lemma="Italie" category="NP" position="280" length="6"/>
//               </parts>
//               <feature name="name" value="Italie"/>
//             </bowNamedEntity>
void MultXmlReaderPrivate::readBowNamedEntity()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowNamedEntity" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowNamedEntity"));

  getTokenAttributes(m_reader, lemma, category, position, length, id);
  auto typeName = getQStringAttribute(m_reader, "type");
  Lima::Common::MediaticData::EntityType entityType;
  try
  {
    entityType = Common::MediaticData::MediaticData::single().getEntityType(typeName);
  }
  catch (const LimaException& e)
  {
    // use empty lemma: no need to store lemma for compound
    QString errorString;
    QTextStream qts(&errorString);
    qts << __FILE__ << ", line" << __LINE__
        << "Unknown entity type" << typeName;
    LERROR << errorString;
    throw LimaException(errorString);
  }
  auto ne = boost::shared_ptr< BoWNamedEntity >(
    new BoWNamedEntity(QString(),
                       category, entityType,
                       position, length));
  m_refMap[id] = ne;
  m_currentComplexToken.push_back(CurrentComplexToken(ne));
  m_relMap[ne]=m_currentBoWRelation;
  m_currentBoWRelation = boost::shared_ptr< BoWRelation >();

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("parts"))
          readParts();
      else if (m_reader.name() == QLatin1String("feature"))
          readFeature();
      else
          m_reader.raiseError(QObject::tr("Expected a parts or feature but got a %1.").arg(m_reader.name()));
  }

  auto token = m_currentComplexToken.back().token;
  uint64_t id = m_currentComplexToken.back().currentPart;
  boost::shared_ptr< BoWRelation > relation = (m_relMap.find(token)==m_relMap.end())?boost::shared_ptr< BoWRelation >():(m_relMap[token]);
#ifdef DEBUG_LP
  LDEBUG << "id=" << id << "; relation=" << *relation;
#endif
  m_currentComplexToken.pop_back();
  if (m_currentComplexToken.empty())
  {
    m_currentBoWText->push_back(token);
  }
  else
  { // this complex token is a part of another
    bool isHead = (id == m_currentComplexToken.back().head);
    if (relation != 0)
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart with relation";
#endif
      m_currentComplexToken.back().token->addPart(relation,token,isHead);
      m_relMap.erase(token);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "addPart without relation";
#endif
      m_currentComplexToken.back().token->addPart(token,isHead);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in addPart => delete it
  }
}

//               <parts head="0">
//                 <bowToken id="2" lemma="Italie" category="NP" position="280" length="6"/>
//               </parts>
void MultXmlReaderPrivate::readParts()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readParts" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("parts"));

  m_currentComplexToken.back().head = getIntAttribute(m_reader,"head");
  m_currentComplexToken.back().currentPart = 0;

  while (m_reader.readNextStartElement()) {
    if (m_reader.name() == QLatin1String("bowToken"))
        readBowToken();
    else if (m_reader.name() == QLatin1String("bowRelation"))
        readBowRelation();
    else if (m_reader.name() == QLatin1String("bowTerm"))
        readBowTerm();
    else if (m_reader.name() == QLatin1String("bowNamedEntity"))
        readBowNamedEntity();
    else
        m_reader.raiseError(QObject::tr("Expected a bowToken, bowTerm, bowRelation or a bowNamedEntity but got a %1.")
                              .arg(m_reader.name()));
  }
}

void MultXmlReaderPrivate::readBowTokenRef()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowTokenRef" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("readBowTokenRef"));

  uint64_t refId = getIntAttribute(m_reader,"refId");
  m_currentComplexToken.back().token->addPart(m_refMap[refId], true);
  m_reader.skipCurrentElement();
}

void MultXmlReaderPrivate::readFeature()
{
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readFeature" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("feature"));

  auto name = getStringAttribute(m_reader,"name");
  auto value = getQStringAttribute(m_reader,"value");
  boost::dynamic_pointer_cast<BoWNamedEntity>(m_currentComplexToken.back().token)->
    setFeature(name,value);
  m_reader.skipCurrentElement();
}

// -----------------------------------------------------------------------
//  helper functions for parsing
// -----------------------------------------------------------------------
bool MultXmlReaderPrivate::addProperty(const QXmlStreamReader& attributes)
{
  auto propName=getStringAttribute(attributes,"name");
  auto typName=getStringAttribute(attributes,"type");
  try
  {
    if (typName == "int")
    {
      m_currentProperties.setIntValue(propName, getIntAttribute(attributes, "value"));
    }
    else if (typName == "string")
    {
      m_currentProperties.setStringValue(propName, getStringAttribute(attributes, "value"));
    }
    else if (typName == "date")
    {
      m_currentProperties.setDateValue(propName, getDateAttribute(attributes, "value"));
    }
    else if (typName == "dateRange")
    {
      auto dateBegin = getDateAttribute(attributes, "valueBegin");
      auto dateEnd = getDateAttribute(attributes, "valueEnd");
      m_currentProperties.setDateIntervalValue(propName, std::make_pair(dateBegin, dateEnd));
    }
    else if (typName == "multString")
    {
      m_currentProperties.addStringValue(propName, getStringAttribute(attributes, "value"));
    }
    else if (typName == "multWeightedId")
    {
      auto val = getStringAttribute(attributes, "value");
      auto score = getFloatAttribute(attributes, "weight");
      m_currentProperties.addWeightedPropValue(propName, std::make_pair(val, score));
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
  catch (std::exception& e)
  {
    BOWLOGINIT;
    LERROR << "Error reading doc infos: " << e.what();
    return false;
  }
  return true;
}

uint64_t MultXmlReaderPrivate::getTokenAttributes(const QXmlStreamReader& attributes,
                                                 LimaString& lemma,
                                                 LinguisticCode& category,
                                                 uint64_t& position,
                                                 uint64_t& length,
                                                 uint64_t& id) const
{
  lemma = getQStringAttribute(attributes,"lemma");
  category = LinguisticCode::fromUInt(getIntAttribute(attributes,"category"));
  position = getIntAttribute(attributes,"position");
  length = getIntAttribute(attributes,"length");
  id = getIntAttribute(attributes,"id");
  return 0;
}

uint64_t MultXmlReaderPrivate::getIntAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  // do not make conversion at once in case the getValue fails
  bool ok;
  auto attr = attributes.attributes().value(name).toULongLong(&ok);

  if (!ok)
  {
    std::ostringstream oss;
    oss << "Cannot convert attribute \""<<name<<"\" to uint64_t";
    throw NoAttributeException(oss.str());
  }

  return attr;
}

float MultXmlReaderPrivate::getFloatAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  // do not make conversion at once in case the getValue fails
  bool ok;
  auto attr = attributes.attributes().value(name).toFloat(&ok);

  if (!ok)
  {
    std::ostringstream oss;
    oss << "Cannot convert attribute \""<<name<<"\" to float";
    throw NoAttributeException(oss.str());
  }

  return attr;
}

std::string MultXmlReaderPrivate::getStringAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  return getQStringAttribute(attributes,name).toStdString();
}

QDate MultXmlReaderPrivate::getDateAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  BOWLOGINIT;
  auto strDate = attributes.attributes().value(name).toString();

  if (strDate == "NotADate")
  {
    LERROR << "BoWXMLHandler::getDateAttribute read strDate \""<<strDate<<"\"";
    return QDate();
  }

  LDEBUG << "BoWXMLHandler::getDateAttribute read strDate \""<<strDate<<"\" with format \"yyyyMMdd\"";
  auto date = QDate::fromString(strDate, "yyyyMMdd");
  if (!date.isValid())
  {
    BOWLOGINIT;
    LERROR << "BoWXMLHandler::getDateAttribute Error trying to read date: " << strDate;
    return QDate();
  }
  return date;
}

QString MultXmlReaderPrivate::getQStringAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  return attributes.attributes().value(name).toString();
}

QString MultXmlReaderPrivate::errorString() const
{
  PROPERTYCODELOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

} // end namespace
} // end namespace
