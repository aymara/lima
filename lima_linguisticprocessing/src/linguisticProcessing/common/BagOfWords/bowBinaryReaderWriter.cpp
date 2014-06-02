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
 * @file       bowBinaryReaderWriter.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Mar 20 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 *
 ***********************************************************************/

#include "bowBinaryReaderWriter.h"
#include "AbstractBoWDocumentHandler.h"
#include "bowText.h"
#include "bowDocument.h"
#include "bowToken.h"
#include "bowTerm.h"
#include "bowNamedEntity.h"
#include "BoWRelation.h"

#include "common/LimaCommon.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

using namespace Lima::Common::MediaticData;

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWBinaryReaderPrivate
{
  friend class BoWBinaryReader;

  BoWBinaryReaderPrivate();
  virtual ~BoWBinaryReaderPrivate();


  std::string m_version;
  BoWFileType m_fileType;

  // entity types mapping
  std::map<MediaticData::EntityGroupId,MediaticData::EntityGroupId> m_entityGroupIdMapping;
  std::map<MediaticData::EntityType,MediaticData::EntityType> m_entityTypeMapping;

  // private member functions
  void readComplexTokenParts(std::istream& file,
                             BoWComplexToken* token,
                             std::vector<BoWToken*>& refMap);
  void readNamedEntityProperties(std::istream& file,
                                 BoWNamedEntity* ne);
  BoWRelation* readBoWRelation(std::istream& file);
  BoWToken* readBoWToken(std::istream& file,
                         std::vector<BoWToken*>& refMap);
  void readSimpleToken(std::istream& file,
                       BoWToken* token);

};

BoWBinaryReaderPrivate::BoWBinaryReaderPrivate():
        m_version(""),
        m_fileType(BOWFILE_NOTYPE),
        m_entityGroupIdMapping(),
        m_entityTypeMapping()
{
}

BoWBinaryReaderPrivate::~BoWBinaryReaderPrivate()
{
}

//***********************************************************************
// reader
BoWBinaryReader::BoWBinaryReader():
        m_d(new BoWBinaryReaderPrivate)
{}

BoWBinaryReader::~BoWBinaryReader()
{
  delete m_d;
}

std::string BoWBinaryReader::getFileTypeString() const
{
    switch (m_d->m_fileType) {
    case BOWFILE_NOTYPE:
        return "BOWFILE_NOTYPE";
    case BOWFILE_TEXT:
        return "BOWFILE_TEXT";
    case BOWFILE_DOCUMENT:
        return "BOWFILE_DOCUMENT";
    case BOWFILE_DOCUMENTST:
        return "BOWFILE_DOCUMENTST";
    case BOWFILE_SDOCUMENT:
        return "BOWFILE_SDOCUMENT";
    }
    return "";
}

void BoWBinaryReader::readHeader(std::istream& file)
{
    Misc::readString(file,m_d->m_version);
    if (m_d->m_version != BOW_VERSION) {
        std::ostringstream oss;
        oss << "uncompatible version : file is in version " << m_d->m_version
        << "; current version is " << BOW_VERSION;
        BOWLOGINIT;
        LERROR << oss.str();
        throw LimaException();
    }
    m_d->m_fileType=static_cast<BoWFileType>(Misc::readOneByteInt(file));
    BOWLOGINIT;

    // read entity types
    Common::MediaticData::MediaticData::changeable().
    readEntityTypes(file,m_d->m_entityGroupIdMapping,m_d->m_entityTypeMapping);
   
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryReader::readHeader type mapping is";
    std::ostringstream oss;
    for (std::map<MediaticData::EntityType,MediaticData::EntityType>::const_iterator
            it=m_d->m_entityTypeMapping.begin(),it_end=m_d->m_entityTypeMapping.end();
            it!=it_end; it++) {
        oss << (*it).first << " -> " << (*it).second << std::endl;
    }
    LDEBUG << oss.str();
#endif
    LDEBUG << "BoWBinaryReader::readHeader end file at: " << file.tellg();

}

void BoWBinaryReader::readBoWText(std::istream& file,
            BoWText& bowText)
{
    uint64_t size=Misc::readCodedInt(file);
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryReader::readBoWText file at: " << file.tellg();
    LDEBUG << "BoWBinaryReader::readBoWText got size=" << size;
#endif
    Misc::readString(file,bowText.lang);
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryReader::readBoWText got lang=" << bowText.lang;
    LDEBUG << "BoWBinaryReader::readBoWText file at: " << file.tellg();
#endif
    // need to store the tokens to handle the references in the part
    // list of the complex tokens
    std::vector<BoWToken*> refMap;
    for (uint64_t i(0); i<size; i++) {
#ifdef DEBUG_LP
        LDEBUG << "BoWBinaryReader::readBoWText reading token " << i;
#endif
        BoWToken* token=readBoWToken(file,refMap);
        if (token) {
            bowText.push_back(token);
        }
        else {
#ifdef DEBUG_LP
            LWARN << "BoWBinaryReader::readBoWText Warning: read null token in file";
#endif
        }
    }
}

void BoWBinaryReader::readBoWDocumentBlock(std::istream& file,
                     BoWDocument& document,
                     AbstractBoWDocumentHandler& handler,
                     bool useIterator)
{
    BoWBlocType blocType;
    file >> blocType;
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryReader::readBoWDocumentBlock: read blocType"
    << (int)blocType;
#endif
    // new format
    switch ( blocType )
    {
    case HIERARCHY_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "HIERARCHY_BLOC";
#endif
        std::string elementName;
        Misc::readStringField(file,elementName);
        handler.openSBoWNode(&document, elementName);
        break;
    }
    case INDEXING_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "INDEXING_BLOC";
#endif
        std::string elementName;
        Misc::readStringField(file,elementName);
        handler.openSBoWIndexingNode(&document, elementName);
        break;
    }
    case BOW_TEXT_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "BOW_TEXT_BLOC";
#endif
        document.clear();
        readBoWText(file,document);
        handler.processSBoWText(&document, useIterator);
        break;
    }
    case NODE_PROPERTIES_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "NODE_PROPERTIES_BLOC";
#endif
        document.Misc::GenericDocumentProperties::read(file);
        handler.processProperties(&document, useIterator);
        break;
    }
    case END_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "END_BLOC";
#endif
        handler.closeSBoWNode();
        break;
    }
    case DOCUMENT_PROPERTIES_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
        LDEBUG << "DOCUMENT_PROPERTIES_BLOC";
#endif
    }
    case ST_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
        LDEBUG << "ST_BLOC";
#endif
        break;
    }
    default:;
    }
}

BoWToken* BoWBinaryReader::readBoWToken(std::istream& file,
             std::vector<BoWToken*>& refMap)
{
  return m_d->readBoWToken(file, refMap);
}

BoWToken* BoWBinaryReaderPrivate::readBoWToken(std::istream& file,
             std::vector<BoWToken*>& refMap)
{
  BOWLOGINIT;
  BoWType type=static_cast<BoWType>(Misc::readOneByteInt(file));
  LDEBUG << "BoWBinaryReader::readBoWToken token type is " << type;
  BoWToken* token(0);
  switch (type)  {
  case BOW_TOKEN: {
      token=new BoWToken;
      readSimpleToken(file,token);
      refMap.push_back(token);
      break;
  }
  case BOW_TERM: {
      token=new BoWTerm;
      //     LDEBUG << "BoWToken: calling read(file,refMap) on term";
      readSimpleToken(file,token);
      refMap.push_back(token);
      readComplexTokenParts(file,static_cast<BoWComplexToken*>(token),refMap);
      break;
  }
  case BOW_NAMEDENTITY: {
      token=new BoWNamedEntity;
//         LDEBUG << "BoWToken: calling read(file,refMap) on NE";
      readSimpleToken(file,token);
      refMap.push_back(token);
      readComplexTokenParts(file,static_cast<BoWComplexToken*>(token),refMap);
      readNamedEntityProperties(file,static_cast<BoWNamedEntity*>(token));
      break;
  }
  default:
    LERROR << "Read error: unknown token type.";
  }
  if (token != 0) {LDEBUG << "BoWToken: token=" << *token;}
  return token;
}

void BoWBinaryReader::readSimpleToken(std::istream& file,
                BoWToken* token)
{
  m_d->readSimpleToken(file, token);
}

void BoWBinaryReaderPrivate::readSimpleToken(std::istream& file,
                BoWToken* token)
{

  LimaString lemma,inflectedForm;
  Misc::readUTF8StringField(file,lemma);
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryReader::readSimpleToken file at: " << file.tellg();
  LDEBUG << "BoWBinaryReader::readSimpleToken read lemma: " << lemma;
#endif
  Misc::readUTF8StringField(file,inflectedForm);
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryReader::readSimpleToken read infl: " << inflectedForm;
#endif
  LinguisticCode category;
  uint64_t position,length;
  category=static_cast<LinguisticCode>(Misc::readCodedInt(file));
  position=Misc::readCodedInt(file);
  length=Misc::readCodedInt(file);
  token->setLemma(lemma);
  token->setInflectedForm(inflectedForm);
  token->setCategory(category);
  token->setPosition(position);
  token->setLength(length);
}

void BoWBinaryReaderPrivate::readComplexTokenParts(std::istream& file,
                      BoWComplexToken* token,
                      std::vector<BoWToken*>& refMap)
{
//     BOWLOGINIT;
//     LDEBUG << "BoWBinaryReader::readComplexTokenParts";
    // read parts
    uint64_t head=Misc::readCodedInt(file);
    token->setHead(head);
    uint64_t nbParts=Misc::readCodedInt(file);
    for (uint64_t i(0); i<nbParts; i++) {
        BoWRelation* rel=readBoWRelation(file);
        bool isInList;
        file.read((char*) &(isInList), sizeof(bool));
        if (! isInList) {
            BoWToken *tok=readBoWToken(file,refMap);
            token->addPart(rel,tok,isInList);
            // tok is copied during addition. Must be deleted here.
            delete tok;
        }
        else {
            uint64_t refnum=Misc::readCodedInt(file);
            if (refnum >= refMap.size()) {
                std::ostringstream oss;
                oss << "readComplexTokenParts(): undefined reference in complex token ("
                << refnum <<"): maybe forward reference";
                BOWLOGINIT;
                LERROR << oss.str();
                throw LimaException();
            }
            token->addPart(rel,refMap[refnum],isInList);
        }
    }
}

void BoWBinaryReaderPrivate::readNamedEntityProperties(std::istream& file,
                          BoWNamedEntity* ne)
{
    BOWLOGINIT;
    EntityGroupId groupId=static_cast<EntityGroupId>(Misc::readCodedInt(file));
    EntityTypeId typeId=static_cast<EntityTypeId>(Misc::readCodedInt(file));
    Common::MediaticData::EntityType oldType=Common::MediaticData::EntityType(typeId,groupId);
    Common::MediaticData::EntityType seType=m_entityTypeMapping[oldType];
    LDEBUG << "BoWBinaryReader::readNamedEntityProperties read type " << oldType << "-->" << seType;
    ne->setNamedEntityType(seType);
    uint64_t nbFeatures=Misc::readCodedInt(file);
    for (uint64_t i(0); i<nbFeatures; i++) {
        std::string attribute;
        LimaString value;
        Misc::readStringField(file,attribute);
        Misc::readUTF8StringField(file,value);
        ne->addFeature(attribute,value);
    }
}

BoWRelation* BoWBinaryReaderPrivate::readBoWRelation(std::istream& file)
{
    BoWRelation* rel=0;
    uint64_t hasRelation=Misc::readCodedInt(file);
    if (hasRelation) {
        rel = new BoWRelation();
        rel->read(file);
    }
    return rel;
}

BoWFileType BoWBinaryReader::getFileType() const
{
  return m_d->m_fileType;
}


//***********************************************************************
// writer
class BoWBinaryWriterPrivate
{
  friend class BoWBinaryWriter;

  BoWBinaryWriterPrivate();
  ~BoWBinaryWriterPrivate();

    // private member functions
  void writeComplexTokenParts(std::ostream& file,
                              const BoWComplexToken* token,
                              std::map<BoWToken*,uint64_t>& refMap) const;
  void writeNamedEntityProperties(std::ostream& file,
                                  const BoWNamedEntity* ne) const;
  void writeBoWRelation(std::ostream& file,
                        BoWRelation* rel) const;
  void writeBoWToken(std::ostream& file,
                     const BoWToken* bowToken,
                     std::map<BoWToken*,uint64_t>& refMap) const;
  void writeSimpleToken(std::ostream& file,
                        const BoWToken* token) const;
};

BoWBinaryWriterPrivate::BoWBinaryWriterPrivate()
{}

BoWBinaryWriterPrivate::~BoWBinaryWriterPrivate()
{}


BoWBinaryWriter::BoWBinaryWriter() :
    m_d(new BoWBinaryWriterPrivate())
{}

BoWBinaryWriter::~BoWBinaryWriter()
{
  delete m_d;
}

void BoWBinaryWriter::writeHeader(std::ostream& file, BoWFileType type) const
{
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeHeader version=" << BOW_VERSION << " ; type=" << type;
    Misc::writeString(file,BOW_VERSION);
    Misc::writeOneByteInt(file,type);
    // write entity types
    MediaticData::MediaticData::single().writeEntityTypes(file);
    LDEBUG << "BoWBinaryWriter::writeHeader end file at: " << file.tellp();
}

void BoWBinaryWriter::writeBoWText(std::ostream& file,
             const BoWText& bowText) const
{
    BOWLOGINIT;
    Misc::writeCodedInt(file,bowText.size());
    Misc::writeString(file,bowText.lang);
    LDEBUG << "BoWBinaryWriter::writeBoWText wrote lang file at: " << file.tellp();
    uint64_t tokenCounter(0);
    // build reverse map to store in file numbers instead of pointers
    std::map<BoWToken*,uint64_t> refMap;
    for (BoWText::const_iterator it=bowText.begin(),
            it_end=bowText.end(); it!=it_end; it++) {
        refMap[(*it)]=tokenCounter;
        writeBoWToken(file,*it,refMap);
        tokenCounter++;
    }
}

void BoWBinaryWriter::writeBoWDocument(std::ostream& file,
                 const BoWDocument& doc) const
{
    BOWLOGINIT;
    LERROR << "BoWBinaryWriter: writeBoWDocument non implemented"<<  LENDL;
    LERROR << "Can not write "<< doc << " into "<< file;
}

void BoWBinaryWriter::writeBoWToken(std::ostream& file,
              const BoWToken* token,
              std::map<BoWToken*,uint64_t>& refMap) const
{
  m_d->writeBoWToken(file, token, refMap);
}

void BoWBinaryWriterPrivate::writeBoWToken(std::ostream& file,
              const BoWToken* token,
              std::map<BoWToken*,uint64_t>& refMap) const
{
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeBoWToken token type is " << token->getType();
  Misc::writeOneByteInt(file,token->getType());
  switch (token->getType()) {
    case BOW_TOKEN: {
        // do not use refMap
        writeSimpleToken(file,token);
        break;
    }
    case BOW_TERM: {
        writeSimpleToken(file,token);
        writeComplexTokenParts(file,static_cast<const BoWComplexToken*>(token),refMap);
        break;
    }
    case BOW_NAMEDENTITY: {
        const BoWNamedEntity* ne=static_cast<const BoWNamedEntity*>(token);
        writeSimpleToken(file,token);
        writeComplexTokenParts(file,ne,refMap);
        writeNamedEntityProperties(file,ne);
        break;
    }
    default: {
        BOWLOGINIT;
        LERROR << "BoWBinaryWriter: cannot handle BoWType " << token->getType();
        throw LimaException();
    }
    }
}

void BoWBinaryWriter::writeSimpleToken(std::ostream& file,
                 const BoWToken* token) const
{
  m_d->writeSimpleToken(file, token);
}

void BoWBinaryWriterPrivate::writeSimpleToken(std::ostream& file,
                 const BoWToken* token) const
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeSimpleToken write lemma: " << token->getLemma();
#endif
    Misc::writeUTF8StringField(file,token->getLemma());
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryWriter::writeSimpleToken write infl: " << token->getInflectedForm();
#endif
    Misc::writeUTF8StringField(file,token->getInflectedForm());
    Misc::writeCodedInt(file,token->getCategory());
    Misc::writeCodedInt(file,token->getPosition());
    Misc::writeCodedInt(file,token->getLength());
}

void BoWBinaryWriterPrivate::writeComplexTokenParts(std::ostream& file,
                       const BoWComplexToken* token,
                       std::map<BoWToken*,uint64_t>& refMap) const
{
    BOWLOGINIT;
    // write parts
    Misc::writeCodedInt(file,token->getHead());
    const std::deque<BoWComplexToken::Part>& parts=token->getParts();
    LDEBUG << "BoWBinaryWriter::writeComplexTokenParts nb parts is " << parts.size();
    Misc::writeCodedInt(file,parts.size());

    for (uint64_t i(0); i<parts.size(); i++) {
        writeBoWRelation(file,parts[i].get<0>());
        bool isInList=parts[i].isInList();
        // LDEBUG << "writing isInList=" << isInList;
        file.write((char*) &(isInList), sizeof(bool));
        if (! isInList) {
            writeBoWToken(file,parts[i].getBoWToken(),refMap);
        }
        else {
            std::map<BoWToken*,uint64_t>::const_iterator
            ref=refMap.find(parts[i].getBoWToken());
            if (ref == refMap.end()) {
                std::ostringstream oss;
                oss << "write():undefined pointer in complex token ("<< parts[i].getBoWToken()
                <<"/"<< Misc::limastring2utf8stdstring(parts[i].getBoWToken()->getLemma())
                <<"): maybe forward reference";
                BOWLOGINIT;
                LERROR << oss.str();
                throw LimaException();
            }
            uint64_t refnum=(*ref).second;
            //       LDEBUG << "writing ref " << refnum <<" for part "
            //                  << parts[i].getBoWToken()->getLemma();
            Misc::writeCodedInt(file,refnum);
        }
    }
}

void BoWBinaryWriterPrivate::writeNamedEntityProperties(std::ostream& file,
                           const BoWNamedEntity* ne) const
{
    BOWLOGINIT;
    MediaticData::EntityType type=ne->getNamedEntityType();
    LDEBUG << "BoWBinaryReader : write type " << type.getGroupId() << "." << type.getTypeId();
    Misc::writeCodedInt(file,type.getGroupId());
    Misc::writeCodedInt(file,type.getTypeId());
    const std::map<std::string, LimaString>& features=ne->getFeatures();
    Misc::writeCodedInt(file,features.size());
    for (std::map<std::string, LimaString>::const_iterator
            it=features.begin(), it_end=features.end(); it!=it_end; it++) {
        Misc::writeStringField(file,(*it).first);
        Misc::writeUTF8StringField(file,(*it).second);
    }
}

void BoWBinaryWriterPrivate::writeBoWRelation(std::ostream& file,
                 BoWRelation* rel) const
{
    uint64_t hasRelation=(rel!=0);
    Misc::writeCodedInt(file, hasRelation);
    if (hasRelation) {
        Misc::writeUTF8StringField(file,rel->getRealization());
        Misc::writeCodedInt(file,rel->getType());
        Misc::writeCodedInt(file,rel->getSynType());
    }
}


} // end namespace
} // end namespace
} // end namespace
