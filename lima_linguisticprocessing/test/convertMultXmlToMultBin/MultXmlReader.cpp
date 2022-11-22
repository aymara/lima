// Copyright (C) 2002-2022 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 * Parser for XML representation of Amose analysed multimedia documents
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlReaderPrivate::parse";
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowDocument" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("MultimediaDocuments"));
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readNode" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("node"));
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readContent" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("content"));
#endif
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
}

//           <properties>
//             <property name="ContentId" type="int" value="1"/>
//             …
//           </properties>
void MultXmlReaderPrivate::readProperties()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlReaderPrivate::readProperties" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("properties"));
#endif
  m_currentProperties.reinit();
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("property"))
          readProperty();
      else
          m_reader.raiseError(QObject::tr("Expected a property but got a %1.").arg(m_reader.name()));
  }

  // Closing a properties token with a langPrpty property when
  // m_currentBoWText is not null means that we just read this text
  // properties, and particularly its language. We can now set its lang
  // write it to the stream
  auto langPrpty = m_currentProperties.getStringValue("langPrpty");
  if (langPrpty.second && m_currentBoWText != 0)
  {
#ifdef DEBUG_LP
    LDEBUG << "MultXmlReaderPrivate::readProperties set current text lang to" << langPrpty.first
            << "and write the BoW text to stream";
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readProperty" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("property"));
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readTokens" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("tokens"));
#endif
  if (m_currentBoWText != nullptr)
  {
    m_currentBoWText->clear();
  }
  else
  {
    BOWLOGINIT;
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
}

//                 <bowRelation realization="" type="3"/>
void MultXmlReaderPrivate::readBowRelation()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowRelation" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowRelation"));
#endif
  auto realization = getQStringAttribute(m_reader, "realization");
  auto type = getIntAttribute(m_reader, "type");
  m_currentBoWRelation = boost::shared_ptr< BoWRelation >(new BoWRelation(realization,type));
  m_reader.skipCurrentElement();
}

//             <bowToken id="3" lemma="" category="NP" position="280" length="6"/>
void MultXmlReaderPrivate::readBowToken()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowToken" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowToken"));
#endif
  getTokenAttributes(m_reader, lemma, category, position, length, id);
#ifdef DEBUG_LP
  LDEBUG << lemma << category << position << length << id ;
#endif
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
      LDEBUG << "MultXmlReaderPrivate::readBowToken addPart with relation";
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowTerm" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowTerm"));
#endif
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
  LDEBUG << "MultXmlReaderPrivate::readBowTerm id=" << id;
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
      LDEBUG << "MultXmlReaderPrivate::readBowTerm addPart with relation";
#endif
      m_currentComplexToken.back().token->addPart(relation,token,isHead);
      m_relMap.erase(token);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "MultXmlReaderPrivate::readBowTerm addPart without relation";
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowNamedEntity" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowNamedEntity"));
#endif
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
#ifdef DEBUG_LP
      LERROR << errorString;
#endif
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
      LDEBUG << "MultXmlReaderPrivate::readBowNamedEntity addPart with relation";
#endif
      m_currentComplexToken.back().token->addPart(relation,token,isHead);
      m_relMap.erase(token);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "MultXmlReaderPrivate::readBowNamedEntity addPart without relation";
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readParts" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("parts"));
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readBowTokenRef" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("readBowTokenRef"));
#endif
  uint64_t refId = getIntAttribute(m_reader,"refId");
  m_currentComplexToken.back().token->addPart(m_refMap[refId], true);
  m_reader.skipCurrentElement();
}

void MultXmlReaderPrivate::readFeature()
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::readFeature" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("feature"));
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LTRACE << "MultXmlReaderPrivate::addProperty";
#endif
  auto propName=getStringAttribute(attributes,"name");
  auto typName=getStringAttribute(attributes,"type");
#ifdef DEBUG_LP
  LTRACE << "MultXmlReaderPrivate::addProperty" << propName << typName;
#endif
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
    oss << "Cannot convert attribute \"" << name << "\"=\""
        << attributes.attributes().value(name).toString().toStdString()
        << "\" to uint64_t. " << errorString().toStdString();
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
    oss << "Cannot convert attribute \"" << name << "\"=\""
        << attributes.attributes().value(name).toString().toStdString()
        << "\" to float. " << errorString().toStdString();
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
#ifdef DEBUG_LP
  BOWLOGINIT;
#endif
  auto strDate = attributes.attributes().value(name).toString();

  if (strDate == "NotADate")
  {
    BOWLOGINIT;
    LERROR << "BoWXMLHandler::getDateAttribute read strDate \""<<strDate<<"\"";
    return QDate();
  }

#ifdef DEBUG_LP
  LDEBUG << "BoWXMLHandler::getDateAttribute read strDate \""<<strDate<<"\" with format \"yyyyMMdd\"";
#endif
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
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultXmlReaderPrivate::getQStringAttribute" << name << attributes.attributes().value(name);
#endif
  return attributes.attributes().value(name).toString();
}

QString MultXmlReaderPrivate::errorString() const
{
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  BOWLOGINIT;
  LERROR << errorStr;
  return errorStr;
}

} // end namespace
} // end namespace
