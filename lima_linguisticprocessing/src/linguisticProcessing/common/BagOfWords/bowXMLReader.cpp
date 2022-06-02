/*
    Copyright 2002-2022 CEA LIST

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
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 ***********************************************************************/

#include "bowXMLReader.h"
#include "bowTerm.h"
#include "bowNamedEntity.h"
#include "bowDocumentST.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include "common/MediaticData/mediaticData.h"

#include <QDate>
#include <QFile>
#include <QXmlStreamReader>

using namespace Lima::Common::Misc;

namespace Lima {
namespace Common {
namespace BagOfWords {

//***********************************************************************
// local exception
//***********************************************************************
class NoAttributeException : public std::runtime_error
{
public:
  NoAttributeException(const std::string& mess):runtime_error(mess) {}
};

class BoWXmlReaderPrivate
{
  friend class BoWXmlReader;

 public:
  BoWXmlReaderPrivate(const std::string& filename,
               std::ostream& output);
  ~BoWXmlReaderPrivate() = default;

  bool parse(QIODevice *device);
  QString errorString() const;

  void readBowDocument();
  void readBowText();
  void readHierarchy();
  void readProperty();
  void readProperties();
  void readTokens();
  void readBowToken();
  void readBowTerm();
  void readBowNamedEntity();
  void readParts();
  void readBowTokenRef();
  void readFeature();

  class CurrentComplexToken
  {
  public:
    CurrentComplexToken():token(),currentPart(0),head() {}
    CurrentComplexToken(boost::shared_ptr< BoWComplexToken > tok):
      token(tok),currentPart(0),head(0) {}
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
  std::ostream& m_outputStream;
  BoWDocument m_currentBoWDocument;
  boost::shared_ptr< BoWText > m_currentBoWText;
  Common::Misc::GenericDocumentProperties m_currentProperties;
  std::vector<CurrentComplexToken> m_currentComplexToken;
  std::map<uint64_t, boost::shared_ptr< BoWToken > > m_refMap;

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


BoWXmlReader::BoWXmlReader(const std::string& filename,
                           std::ostream& output):
m_d(new BoWXmlReaderPrivate(filename, output))
{
}

BoWXmlReader::~BoWXmlReader()
{
  delete m_d;
}

//**********************************************************************
// xerces handler functions
//**********************************************************************
BoWXmlReaderPrivate::BoWXmlReaderPrivate(const std::string& filename,
                                         std::ostream& output):
    m_reader(),
    m_outputStream(output),
    m_currentBoWDocument(),
    m_currentBoWText(),
    m_currentComplexToken(),
    m_refMap()
{
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    LIMA_EXCEPTION_SELECT_LOGINIT(
      BOWLOGINIT,
      QString(QLatin1String("Error opening %1")).arg(filename.c_str()).toUtf8().constData(),
      XMLException);
  if (!parse(&file))
  {
    BOWLOGINIT;
    throw XMLException(QLatin1String("Error parsing %1: %2")
              .arg(filename.c_str())
              .arg(errorString()).toUtf8().constData());
  }
}

bool BoWXmlReaderPrivate::parse(QIODevice *device)
{
  BOWLOGINIT;
  LDEBUG << "BoWXmlReaderPrivate::parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("bowDocument"))
      {
          readBowDocument();
      }
      else
      {
          m_reader.raiseError(QObject::tr("The file is not a LIMA BoW XML file."));
      }
  }
  return !m_reader.error();
}

void BoWXmlReaderPrivate::readBowDocument()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowDocument" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowDocument"));

  m_currentBoWDocument.clear();

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("bowText"))
          readBowText();
      else if (m_reader.name() == QLatin1String("hierarchy"))
          readHierarchy();
      else
          m_reader.raiseError(QObject::tr("Expected a bowText or a hierarchy but got a %1.").arg(m_reader.name()));
  }
  m_currentBoWText=boost::shared_ptr <Lima::Common::BagOfWords::BoWText >();
}

void BoWXmlReaderPrivate::readBowText()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowText" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowText"));

  m_currentBoWText= boost::shared_ptr< BoWText >(new BoWText());
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("tokens"))
          readTokens();
      else
          m_reader.raiseError(QObject::tr("Expected a tokens but got a %1.").arg(m_reader.name()));
  }
}

void BoWXmlReaderPrivate::readHierarchy()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readHierarchy" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("hierarchy"));

  m_currentBoWText = boost::shared_ptr< BoWText >(new BoWText());
  bool isIndexingNode(false);
  try {
    auto indexingNode = getStringAttribute(m_reader, "indexingNode");
    if (indexingNode == "yes")
    {
      isIndexingNode = true;
    }
  }
  catch (NoAttributeException& )
  {
    // do nothing -> is not indexing node
  }
  if (isIndexingNode)
  {
    Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::INDEXING_BLOC);
    m_currentBoWText = boost::shared_ptr< BoWText >(new BoWText());
  }
  else
  {
    Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::HIERARCHY_BLOC);
  }
  std::string elementName("");
  try
  {
    elementName = getStringAttribute(m_reader, "elementName");
  }
  catch (NoAttributeException& )
  {
    LERROR << "missing attribute elementName in " << m_reader.name();
  }
  Lima::Common::Misc::writeStringField(m_outputStream, elementName);
  while (m_reader.readNextStartElement())
  {
      if (m_reader.name() == QLatin1String("bowText"))
          readBowText();
      else if (m_reader.name() == QLatin1String("hierarchy"))
          readHierarchy();
      else
          m_reader.raiseError(QObject::tr("Expected a bowText or a hierarchy but got a %1.").arg(m_reader.name()));
  }

  Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::END_BLOC);
  if (m_currentBoWText !=0)
  {
    BoWBinaryWriter writer;
    writer.writeBoWText(m_outputStream, *m_currentBoWText);
    m_currentBoWText = boost::shared_ptr< BoWText >();
  }
}

void BoWXmlReaderPrivate::readProperty()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readProperty" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("property"));

  addProperty(m_reader);
}

void BoWXmlReaderPrivate::readProperties()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readProperties" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("properties"));

  m_currentProperties.reinit();
  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("property"))
          readProperty();
      else
          m_reader.raiseError(QObject::tr("Expected a property but got a %1.").arg(m_reader.name()));
  }
  Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::NODE_PROPERTIES_BLOC);
  m_currentProperties.write(m_outputStream);
}

void BoWXmlReaderPrivate::readTokens()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readTokens" << m_reader.name();
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
      else
          m_reader.raiseError(QObject::tr("Expected a bowToken, bowTerm or a bowNamedEntity but got a %1.")
                                .arg(m_reader.name()));
  }
  Common::Misc::writeOneByteInt(m_outputStream, Common::BagOfWords::BOW_TEXT_BLOC);
  //@todo
  m_currentBoWText->writeBoWText(m_outputStream);
}

void BoWXmlReaderPrivate::readBowToken()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowToken" << m_reader.name();
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
    if (m_currentComplexToken.back().currentPart ==
        m_currentComplexToken.back().head) {
      m_currentComplexToken.back().token->addPart(token,true);
    }
    else
    {
      m_currentComplexToken.back().token->addPart(token);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in complex token
  }
  m_reader.skipCurrentElement();
}

void BoWXmlReaderPrivate::readBowTerm()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowTerm" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowTerm"));

  getTokenAttributes(m_reader, lemma, category, position, length, id);
  // use empty lemma: no need to store lemma for compound
  auto term = boost::shared_ptr< BoWTerm >(new BoWTerm(QString(), category, position, length));
  m_refMap[id] = term;
  m_currentComplexToken.push_back(CurrentComplexToken(term));
  while (m_reader.readNextStartElement())
  {
      if (m_reader.name() == QLatin1String("parts"))
          readParts();
      else
          m_reader.raiseError(QObject::tr("Expected a parts but got a %1.").arg(m_reader.name()));
  }
  auto token = m_currentComplexToken.back().token;
  m_currentComplexToken.pop_back();
  if (m_currentComplexToken.empty())
  {
    m_currentBoWText->push_back(token);
  }
  else
  { // this complex token is a part of another
    if (m_currentComplexToken.back().currentPart == m_currentComplexToken.back().head)
    {
      m_currentComplexToken.back().token->addPart(token,true);
    }
    else
    {
      m_currentComplexToken.back().token->addPart(token);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in addPart => delete it
  }
}

void BoWXmlReaderPrivate::readBowNamedEntity()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowNamedEntity" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowNamedEntity"));

  getTokenAttributes(m_reader, lemma, category, position, length, id);
  auto typeName = getQStringAttribute(m_reader, "type");
  Lima::Common::MediaticData::EntityType entityType;
  try
  {
    entityType = MediaticData::MediaticData::single().getEntityType(typeName);
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

  while (m_reader.readNextStartElement()) {
      if (m_reader.name() == QLatin1String("parts"))
          readParts();
      else if (m_reader.name() == QLatin1String("feature"))
          readFeature();
      else
          m_reader.raiseError(QObject::tr("Expected a parts or feature but got a %1.").arg(m_reader.name()));
  }
  auto token = m_currentComplexToken.back().token;
  m_currentComplexToken.pop_back();
  if (m_currentComplexToken.empty())
  {
    m_currentBoWText->push_back(token);
  }
  else
  { // this complex token is a part of another
    if (m_currentComplexToken.back().currentPart == m_currentComplexToken.back().head)
    {
      m_currentComplexToken.back().token->addPart(token,true);
    }
    else
    {
      m_currentComplexToken.back().token->addPart(token);
    }
    m_currentComplexToken.back().currentPart++;
    // token has been cloned in addPart => delete it
  }
  m_reader.skipCurrentElement();
}

void BoWXmlReaderPrivate::readParts()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readParts" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("parts"));

  m_currentComplexToken.back().head = getIntAttribute(m_reader,"head");
  m_currentComplexToken.back().currentPart = 0;

  while (m_reader.readNextStartElement()) {
    if (m_reader.name() == QLatin1String("bowToken"))
        readBowToken();
    else if (m_reader.name() == QLatin1String("bowTerm"))
        readBowTerm();
    else if (m_reader.name() == QLatin1String("bowNamedEntity"))
        readBowNamedEntity();
    else
        m_reader.raiseError(QObject::tr("Expected a bowToken, bowTerm or a bowNamedEntity but got a %1.")
                              .arg(m_reader.name()));
  }
}

void BoWXmlReaderPrivate::readBowTokenRef()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readBowTokenRef" << m_reader.name();
  Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("bowTokenRef"));

  uint64_t refId = getIntAttribute(m_reader,"refId");
  m_currentComplexToken.back().token->addPart(m_refMap[refId]);
  m_reader.skipCurrentElement();
}

void BoWXmlReaderPrivate::readFeature()
{
  BOWLOGINIT;
  LTRACE << "BoWXmlReaderPrivate::readFeature" << m_reader.name();
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
bool BoWXmlReaderPrivate::addProperty(const QXmlStreamReader& attributes)
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
  }
  catch (std::exception& e)
  {
    BOWLOGINIT;
    LERROR << "Error reading doc infos: " << e.what();
    return false;
  }
  return true;
}

uint64_t BoWXmlReaderPrivate::getTokenAttributes(const QXmlStreamReader& attributes,
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

uint64_t BoWXmlReaderPrivate::getIntAttribute(const QXmlStreamReader& attributes, const char* name) const
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

float BoWXmlReaderPrivate::getFloatAttribute(const QXmlStreamReader& attributes, const char* name) const
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

std::string BoWXmlReaderPrivate::getStringAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  return getQStringAttribute(attributes,name).toStdString();
}

QDate BoWXmlReaderPrivate::getDateAttribute(const QXmlStreamReader& attributes, const char* name) const
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

QString BoWXmlReaderPrivate::getQStringAttribute(const QXmlStreamReader& attributes, const char* name) const
{
  return attributes.attributes().value(name).toString();
}

QString BoWXmlReaderPrivate::errorString() const
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
} // end namespace
