/*
    Copyright 2002-2020 CEA LIST

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
#include "BoWPredicate.h"

#include "common/LimaCommon.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

using namespace Lima::Common::MediaticData;

namespace Lima {
namespace Common {
namespace BagOfWords {

#define BOW_VERSION "0.9"


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
                             boost::shared_ptr< BoWComplexToken > token);
  void readNamedEntityProperties(std::istream& file,
                                 boost::shared_ptr< BoWNamedEntity >  ne);
  boost::shared_ptr< BoWRelation > readBoWRelation(std::istream& file);
  boost::shared_ptr< AbstractBoWElement > readBoWToken(std::istream& file);
  void readSimpleToken(std::istream& file,
                       boost::shared_ptr< BoWToken > token);
  void readPredicate(std::istream& file,
                    boost::shared_ptr< BoWPredicate > bowPred);

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
        BOWLOGINIT;
        LIMA_EXCEPTION("BoWBinaryReader::readHeader - incompatible version : file is in version "
                        << m_d->m_version.c_str() << "; current version is "
                        << BOW_VERSION );
    }
    m_d->m_fileType=static_cast<BoWFileType>(Misc::readOneByteInt(file));

    // read entity types
    Common::MediaticData::MediaticData::changeable().
    readEntityTypes(file,m_d->m_entityGroupIdMapping,m_d->m_entityTypeMapping);

#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryReader::readHeader type mapping is (shown if logger = TRACE)";
    std::ostringstream oss;
    for (std::map<MediaticData::EntityType,MediaticData::EntityType>::const_iterator
            it=m_d->m_entityTypeMapping.begin(),it_end=m_d->m_entityTypeMapping.end();
            it!=it_end; it++) {
        oss << (*it).first << " -> " << (*it).second << std::endl;
    }
    LTRACE << oss.str();
    LDEBUG << "BoWBinaryReader::readHeader end file at: " << file.tellg();
#endif

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
    for (uint64_t i(0); i<size; i++)
    {
#ifdef DEBUG_LP
        LDEBUG << "BoWBinaryReader::readBoWText reading token " << i;
#endif
        boost::shared_ptr< AbstractBoWElement > token = readBoWToken(file);
        if (token)
        {
//           BOWLOGINIT;
//           LERROR << __FILE__ << __LINE__ << "BoWBinaryReader::readBoWText do not push token. It should be  deleted right now";
          bowText.push_back(token);
        }
        else
        {
#ifdef DEBUG_LP
          LWARN << "BoWBinaryReader::readBoWText Warning: read null token in file";
#endif
        }
    }
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryReader::readBoWText DONE";
#endif
}

void BoWBinaryReader::readBoWDocumentBlock(std::istream& file,
                     BoWDocument& document,
                     AbstractBoWDocumentHandler& handler,
                     bool useIterator,
                     bool useIndexIterator)
{
    BoWBlocType blocType = static_cast<BoWBlocType>( Misc::readOneByteInt(file) );
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryReader::readBoWDocumentBlock: read blocType" << blocType;
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
        handler.processSBoWText(&document, useIterator, useIndexIterator);
        break;
    }
    case NODE_PROPERTIES_BLOC:
    {
#ifdef DEBUG_LP
        LDEBUG << "NODE_PROPERTIES_BLOC";
#endif
        document.Misc::GenericDocumentProperties::read(file);
        handler.processProperties(&document, useIterator, useIndexIterator);
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
        break;
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

boost::shared_ptr< AbstractBoWElement > BoWBinaryReader::readBoWToken(std::istream& file)
{
  return m_d->readBoWToken(file);
}

boost::shared_ptr< AbstractBoWElement > BoWBinaryReaderPrivate::readBoWToken( std::istream& file )
{
#ifdef DEBUG_LP
  BOWLOGINIT;
#endif
  BoWType type=static_cast<BoWType>(Misc::readOneByteInt(file));
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryReader::readBoWToken token type is " << type;
#endif
  boost::shared_ptr< AbstractBoWElement > token;
  switch (type)  {
  case BoWType::BOW_TOKEN: {
      token=boost::shared_ptr< BoWToken >( new BoWToken);
      readSimpleToken(file, boost::dynamic_pointer_cast<BoWToken>(token));
      break;
  }
  case BoWType::BOW_TERM: {
      token=boost::shared_ptr< BoWTerm >(new BoWTerm);
      //     LDEBUG << "BoWToken: calling read(file) on term";
      readSimpleToken(file,boost::dynamic_pointer_cast<BoWToken>(token));
      readComplexTokenParts(file,boost::dynamic_pointer_cast<BoWComplexToken>(token));
      break;
  }
  case BoWType::BOW_NAMEDENTITY: {
      token=boost::shared_ptr< BoWNamedEntity >(new BoWNamedEntity);
//         LDEBUG << "BoWToken: calling read(file) on NE";
      readSimpleToken(file,boost::dynamic_pointer_cast<BoWToken>(token));
      readComplexTokenParts(file,boost::dynamic_pointer_cast<BoWComplexToken>(token));
      readNamedEntityProperties(file,boost::dynamic_pointer_cast<BoWNamedEntity>(token));
      break;
  }
  case BoWType::BOW_PREDICATE:{
      token=boost::shared_ptr< BoWPredicate >(new BoWPredicate);
      readPredicate(file,boost::dynamic_pointer_cast<BoWPredicate>(token));
      break;
  }
  default:
    BOWLOGINIT;
    LERROR << "Read error: unknown token type.";
  }
#ifdef DEBUG_LP
  if (token != 0) {
    LDEBUG << "BoWToken: token=" << &*token;
  }
#endif
  return token;
}

void BoWBinaryReader::readSimpleToken(std::istream& file,
                boost::shared_ptr< BoWToken > token)
{
  m_d->readSimpleToken(file, token);
}

void BoWBinaryReaderPrivate::readSimpleToken(std::istream& file,
                boost::shared_ptr< BoWToken > token)
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
  category = LinguisticCode::decodeFromBinary(file);
  position=Misc::readCodedInt(file);
  length=Misc::readCodedInt(file);
  token->setLemma(lemma);
  token->setInflectedForm(inflectedForm);
  token->setCategory(category);
  token->setPosition(position);
  token->setLength(length);
}

void BoWBinaryReader::readPredicate(std::istream& file,
                   boost::shared_ptr< BoWPredicate > bowPred){
  m_d->readPredicate(file, bowPred);
}

void BoWBinaryReaderPrivate::readPredicate(std::istream& file,
                                    boost::shared_ptr< BoWPredicate > bowPred){
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryReaderPrivate::readPredicate";
#endif
  EntityGroupId groupId = static_cast<EntityGroupId>( Misc::readCodedInt(file));
  EntityTypeId typeId = static_cast<EntityTypeId>(Misc::readCodedInt(file));
  Common::MediaticData::EntityType oldType=Common::MediaticData::EntityType(typeId,groupId);
  Common::MediaticData::EntityType predType=m_entityTypeMapping[oldType];

  bowPred->setPosition(Misc::readCodedInt(file));
  bowPred->setLength(Misc::readCodedInt(file));
  uint64_t roleNb = Misc::readCodedInt(file);
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryReader::readPredicate Read "<< roleNb<< "roles associated to the predicate";
#endif
  bowPred->setPredicateType(predType);
  for (uint64_t i = 0; i < roleNb; i++)
  {
    EntityGroupId roleGrId = static_cast<EntityGroupId>(Misc::readCodedInt(file));
    EntityTypeId roleTypId = static_cast<EntityTypeId>(Misc::readCodedInt(file));
    Common::MediaticData::EntityType oldRoleType=Common::MediaticData::EntityType(roleTypId,roleGrId);
    Common::MediaticData::EntityType roleType=m_entityTypeMapping[oldRoleType];
    boost::shared_ptr< AbstractBoWElement > role = readBoWToken(file);
    bowPred->roles().insert(roleType,role);
  }
}

void BoWBinaryReaderPrivate::readComplexTokenParts(std::istream& file,
    boost::shared_ptr< Lima::Common::BagOfWords::BoWComplexToken > token)
{
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryReader::readComplexTokenParts";
#endif
    // read parts
    uint64_t head=Misc::readCodedInt(file);
    token->setHead(head);
    uint64_t nbParts=Misc::readCodedInt(file);
    for (uint64_t i(0); i<nbParts; i++) {
        boost::shared_ptr< BoWRelation> rel=readBoWRelation(file);
        bool isInList;
        file.read((char*) &(isInList), sizeof(bool));
        boost::shared_ptr< BoWToken > tok = boost::dynamic_pointer_cast<BoWToken>(readBoWToken(file));
        if (tok)
          token->addPart(rel,tok);
    }
}

void BoWBinaryReaderPrivate::readNamedEntityProperties(std::istream& file, boost::shared_ptr< Lima::Common::BagOfWords::BoWNamedEntity > ne)
{
#ifdef DEBUG_LP
    BOWLOGINIT;
#endif
    EntityGroupId groupId=static_cast<EntityGroupId>(Misc::readCodedInt(file));
    EntityTypeId typeId=static_cast<EntityTypeId>(Misc::readCodedInt(file));
    Common::MediaticData::EntityType oldType=Common::MediaticData::EntityType(typeId,groupId);
    Common::MediaticData::EntityType seType=m_entityTypeMapping[oldType];
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryReader::readNamedEntityProperties read type " << oldType << "-->" << seType;
#endif
    ne->setNamedEntityType(seType);
    uint64_t nbFeatures=Misc::readCodedInt(file);
    for (uint64_t i(0); i<nbFeatures; i++) {
        std::string attribute;
        LimaString value;
        Misc::readStringField(file,attribute);
        Misc::readUTF8StringField(file,value);
        ne->setFeature(attribute,value);
    }
}

boost::shared_ptr< BoWRelation > BoWBinaryReaderPrivate::readBoWRelation(std::istream& file)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
#endif
  boost::shared_ptr< BoWRelation > rel=boost::shared_ptr< BoWRelation >();
  uint64_t hasRelation=Misc::readCodedInt(file);
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryReaderPrivate::readBoWRelation: read hasRelation" << hasRelation;
#endif
  if (hasRelation) {
      rel = boost::shared_ptr< BoWRelation >(new BoWRelation());
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

  BoWBinaryWriterPrivate(const QMap< uint64_t,uint64_t >& shiftFrom);
  ~BoWBinaryWriterPrivate();

    // private member functions
  void writeComplexTokenParts(std::ostream& file,
                              const boost::shared_ptr< BoWComplexToken > token) const;
  void writeNamedEntityProperties(std::ostream& file,
                                  const boost::shared_ptr< BoWNamedEntity > ne) const;
  void writeBoWRelation(std::ostream& file,
                        boost::shared_ptr< BoWRelation > rel) const;
  void writeBoWToken(std::ostream& file,
                     const boost::shared_ptr< AbstractBoWElement > bowToken) const;
  void writeSimpleToken(std::ostream& file,
                        const boost::shared_ptr< BoWToken > token) const;
  void writePredicate(std::ostream& file,
                        const boost::shared_ptr< BoWPredicate > predicate) const;

  QMap< uint64_t,uint64_t > m_shiftFrom;
};

BoWBinaryWriterPrivate::BoWBinaryWriterPrivate(const QMap< uint64_t, uint64_t >& shiftFrom) :
    m_shiftFrom(shiftFrom)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriterPrivate::BoWBinaryWriterPrivate" << shiftFrom.size();
  LDEBUG << "BoWBinaryWriterPrivate::BoWBinaryWriterPrivate" << m_shiftFrom.size();
#endif
}

BoWBinaryWriterPrivate::~BoWBinaryWriterPrivate()
{}


BoWBinaryWriter::BoWBinaryWriter(const QMap< uint64_t, uint64_t >& shiftFrom) :
    m_d(new BoWBinaryWriterPrivate(shiftFrom))
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::BoWBinaryWriter" << m_d->m_shiftFrom.size();
#endif
}

BoWBinaryWriter::~BoWBinaryWriter()
{
  delete m_d;
}

void BoWBinaryWriter::writeHeader(std::ostream& file, BoWFileType type) const
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeHeader version=" << BOW_VERSION << " ; type=" << type;
#endif
  Misc::writeString(file,BOW_VERSION);
  Misc::writeOneByteInt(file,type);
  // write entity types
  MediaticData::MediaticData::single().writeEntityTypes(file);
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryWriter::writeHeader end on file " << &file << "at: " << file.tellp();
#endif
}

void BoWBinaryWriter::writeBoWText(std::ostream& file,
             const BoWText& bowText) const
{
#ifdef DEBUG_LP
    BOWLOGINIT;
#endif
    Misc::writeCodedInt(file,bowText.size());
    Misc::writeString(file,bowText.lang);
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryWriter::writeBoWText wrote lang on file"<<&file<<" at: " << file.tellp();
#endif
    uint64_t tokenCounter(0);
    // build reverse map to store in file numbers instead of pointers
    for (BoWText::const_iterator it=bowText.begin(),
            it_end=bowText.end(); it!=it_end; it++) {
        writeBoWToken(file,*it);
        tokenCounter++;
    }
}

void BoWBinaryWriter::writeBoWDocument(std::ostream& file,
                 const BoWDocument& doc) const
{
    BOWLOGINIT;
    LERROR << "BoWBinaryWriter: writeBoWDocument non implemented";
    LERROR << "Can not write "<< doc << " into stream"<< &file;
}

void BoWBinaryWriter::writeBoWToken(std::ostream& file,
              const boost::shared_ptr< AbstractBoWElement > token) const
{
  m_d->writeBoWToken(file, token);
}

void BoWBinaryWriterPrivate::writeBoWToken( std::ostream& file, const boost::shared_ptr< Lima::Common::BagOfWords::AbstractBoWElement > token ) const
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeBoWToken token type is " << token->getType() << &file;
#endif
  Misc::writeOneByteInt(file,toInt(token->getType()));
  switch (token->getType()) {
    case BoWType::BOW_TOKEN: {
        writeSimpleToken(file,boost::dynamic_pointer_cast<BoWToken>(token));
        break;
    }
    case BoWType::BOW_TERM: {
        writeSimpleToken(file,boost::dynamic_pointer_cast<BoWToken>(token));
        writeComplexTokenParts(file,boost::dynamic_pointer_cast<BoWComplexToken>(token));
        break;
    }
    case BoWType::BOW_NAMEDENTITY: {
        boost::shared_ptr< BoWNamedEntity > ne=boost::dynamic_pointer_cast<BoWNamedEntity>(token);
        writeSimpleToken(file,boost::dynamic_pointer_cast<BoWToken>(token));
        writeComplexTokenParts(file,ne);
        writeNamedEntityProperties(file,ne);
        break;
    }
    case BoWType::BOW_PREDICATE:{
        writePredicate(file,boost::dynamic_pointer_cast<BoWPredicate>(token));
      break;
    }
    default: {
        BOWLOGINIT;
        LIMA_EXCEPTION( "BoWBinaryWriter: cannot handle BoWType " << token->getType() );
    }
    }
}

void BoWBinaryWriter::writeSimpleToken(std::ostream& file,
                 const boost::shared_ptr< BoWToken > token) const
{
  m_d->writeSimpleToken(file, token);
}

void BoWBinaryWriterPrivate::writeSimpleToken(std::ostream& file,
                 const boost::shared_ptr< BoWToken > token) const
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWBinaryWriter::writeSimpleToken write lemma: " << &file << token->getLemma();
#endif
  Misc::writeUTF8StringField(file,token->getLemma());
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryWriter::writeSimpleToken write infl: " << token->getInflectedForm();
#endif
  Misc::writeUTF8StringField(file,token->getInflectedForm());
  LinguisticCode::encodeToBinary(file, token->getCategory());

  auto beg = token->getPosition();
  auto end = token->getLength() + beg;

  if (m_shiftFrom.empty())
  {
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom is empty";
#endif
  }
  else
  {
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from begin" << beg;
    LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from end" << end;
#endif
    auto const shiftForBeginIt = m_shiftFrom.lowerBound(beg-1);
    if (shiftForBeginIt == m_shiftFrom.constBegin())
    {
#ifdef DEBUG_LP
      LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from begin: NO shift";
#endif
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from begin: shift by" << (shiftForBeginIt-1).value();
#endif
      beg += (shiftForBeginIt-1).value();
    }
    auto const shiftForEndIt = m_shiftFrom.lowerBound(end-1);
    if (shiftForEndIt == m_shiftFrom.constBegin())
    {
#ifdef DEBUG_LP
      LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from end: NO shift";
#endif
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "BoWBinaryWriter::writeSimpleToken shiftFrom from end: shift by" << (shiftForEndIt-1).value();
#endif
      end += (shiftForEndIt-1).value();
    }
  }

  Misc::writeCodedInt(file, beg-1);
  Misc::writeCodedInt(file, end-beg);
}

void BoWBinaryWriter::writePredicate(std::ostream& file,
                        const boost::shared_ptr< BoWPredicate > predicate) const{
  m_d->writePredicate(file, predicate);
}

void BoWBinaryWriterPrivate::writePredicate(std::ostream& file,
                        const boost::shared_ptr< BoWPredicate > predicate) const{
#ifdef DEBUG_LP
  BOWLOGINIT;
#endif
  MediaticData::EntityType type=predicate->getPredicateType();
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryWriter::writePredicate type" << type;
#endif
  Misc::writeCodedInt(file,type.getGroupId());
  Misc::writeCodedInt(file,type.getTypeId());
  Misc::writeCodedInt(file,predicate->getPosition()-1);
  Misc::writeCodedInt(file,predicate->getLength());
  const QMultiMap<Common::MediaticData::EntityType, boost::shared_ptr< AbstractBoWElement > >& pRoles = predicate->roles();
#ifdef DEBUG_LP
  LDEBUG << "BoWBinaryWriter::writePredicate nb  roles" << pRoles.size();
#endif
  Misc::writeCodedInt(file,pRoles.size());
  for (auto it = pRoles.begin(); it != pRoles.end(); it++)
  {
    Misc::writeCodedInt(file,it.key().getGroupId());
    Misc::writeCodedInt(file,it.key().getTypeId());
    writeBoWToken(file,it.value());
  }
}


void BoWBinaryWriterPrivate::writeComplexTokenParts(std::ostream& file,
                       const boost::shared_ptr< BoWComplexToken > token) const
{
#ifdef DEBUG_LP
    BOWLOGINIT;
#endif
    // write parts
    Misc::writeCodedInt(file,token->getHead());
    const std::deque<BoWComplexToken::Part>& parts=token->getParts();
#ifdef DEBUG_LP
    LDEBUG << "BoWBinaryWriter::writeComplexTokenParts nb parts is " << parts.size();
#endif
    Misc::writeCodedInt(file,parts.size());

    for (uint64_t i(0); i<parts.size(); i++) {
        writeBoWRelation(file,parts[i].get<0>());
        bool isInList=false;
#ifdef DEBUG_LP
        LDEBUG << "BoWBinaryWriter::writeComplexTokenParts writing";
#endif
        file.write((char*) &(isInList), sizeof(bool));
        writeBoWToken(file,parts[i].getBoWToken());
    }
}

void BoWBinaryWriterPrivate::writeNamedEntityProperties(std::ostream& file,
                           const boost::shared_ptr< BoWNamedEntity > ne) const
{
    MediaticData::EntityType type=ne->getNamedEntityType();
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryWriter::writeNamedEntityProperties : write type " << type.getGroupId() << "." << type.getTypeId();
#endif
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
                 boost::shared_ptr< BoWRelation > rel) const
{
    uint64_t hasRelation=(rel!=0);
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWBinaryWriter::writeBoWRelation: write hasRelation" << hasRelation;
#endif
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
