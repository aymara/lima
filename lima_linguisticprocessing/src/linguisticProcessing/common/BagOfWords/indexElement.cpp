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
 * @file       indexElement.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 *
 ***********************************************************************/

#include "indexElement.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/LimaString.h"
#include "common/MediaticData/mediaticData.h"

using namespace Lima::Common::Misc;
using namespace Lima::Common::BagOfWords;

namespace Lima {
namespace Common {
namespace BagOfWords {


class IndexElementPrivate
{
  friend class IndexElement;
  friend std::ostream& operator<<(std::ostream& os, const IndexElement& elt);
  friend QDebug& operator<<(QDebug& os, const IndexElement& elt);
  friend QTextStream& operator<<(QTextStream& os, const IndexElement& elt);

  IndexElementPrivate();
  IndexElementPrivate(const uint64_t id,
               const Lima::Common::BagOfWords::BoWType type,
               const LimaString& word,
               const uint64_t cat=0,
               const uint64_t position=0,
               const uint64_t length=0,
               const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType());
  IndexElementPrivate(const uint64_t id,
               const Lima::Common::BagOfWords::BoWType type,
               const std::vector<uint64_t>& structure,
               const std::vector<uint64_t>& relations,
               const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType());
  IndexElementPrivate(const IndexElementPrivate& iep);
  IndexElementPrivate& operator=(const IndexElementPrivate& iep);
  ~IndexElementPrivate() {}

  uint64_t m_id;
  Lima::Common::BagOfWords::BoWType m_type;
  LimaString m_word;
  // for simple term, keep also some informations that
  // may be useful, such as category and position
  uint64_t m_category;
  uint64_t m_position;
  uint64_t m_length;
  Common::MediaticData::EntityType m_neType;
  Misc::PositionLengthList m_poslenlist;
  std::vector<uint64_t> m_structure;
  std::vector<uint64_t> m_relations;



};


IndexElementPrivate::IndexElementPrivate():
m_id(0),
m_type(BoWType::BOW_NOTYPE),
m_word(),
m_category(0),
m_position(0),
m_length(0),
m_neType(),
m_poslenlist(0),
m_structure(),
m_relations()
{
}

IndexElementPrivate::IndexElementPrivate(const IndexElementPrivate& iep):
m_id(iep.m_id),
m_type(iep.m_type),
m_word(iep.m_word),
m_category(iep.m_category),
m_position(iep.m_position),
m_length(iep.m_length),
m_neType(iep.m_neType),
m_poslenlist(iep.m_poslenlist),
m_structure(iep.m_structure),
m_relations(iep.m_relations)
{
}

IndexElementPrivate::IndexElementPrivate(
             const uint64_t id,
             const Lima::Common::BagOfWords::BoWType type,
             const LimaString& word,
             const uint64_t cat,
             const uint64_t position,
             const uint64_t length,
             const Common::MediaticData::EntityType neType):
m_id(id),
m_type(type),
m_word(word),
m_category(cat),
m_position(position),
m_length(length),
m_neType(neType),
m_poslenlist(1, std::make_pair(
      Common::Misc::Position(position),
      Common::Misc::Length(length))),
m_structure(),
m_relations()
{
}

IndexElementPrivate::IndexElementPrivate(
             const uint64_t id,
             const Lima::Common::BagOfWords::BoWType type,
             const std::vector<uint64_t>& structure,
             const std::vector<uint64_t>& relations,
             const Common::MediaticData::EntityType neType):
m_id(id),
m_type(type),
m_word(),
m_category(0),
m_position(0),
m_length(0),
m_neType(neType),
m_poslenlist(0),
m_structure(structure),
m_relations(relations)
{
}


IndexElementPrivate& IndexElementPrivate::operator=(const IndexElementPrivate& iep)
{
  if (this != &iep)
  {
    m_id = iep.m_id;
    m_type = iep.m_type;
    m_word = iep.m_word;
    m_category = iep.m_category;
    m_position  = iep.m_position;
    m_length = iep.m_length;
    m_neType = iep.m_neType;
    m_poslenlist = iep.m_poslenlist;
    m_structure = iep.m_structure;
    m_relations = iep.m_relations;
  }
  return *this;
}

//***********************************************************************
// constructors and destructors
IndexElement::IndexElement(): m_d(new IndexElementPrivate())
{
}

IndexElement::IndexElement(
             const uint64_t id,
             const Lima::Common::BagOfWords::BoWType type,
             const LimaString& word,
             const uint64_t cat,
             const uint64_t position,
             const uint64_t length,
             const Common::MediaticData::EntityType neType):
    m_d(new IndexElementPrivate(id, type, word, cat, position, length, neType))
{
}

IndexElement::IndexElement(
             const uint64_t id,
             const Lima::Common::BagOfWords::BoWType type,
             const std::vector<uint64_t>& structure,
             const std::vector<uint64_t>& relations,
             const Common::MediaticData::EntityType neType):
    m_d(new IndexElementPrivate(id, type, structure, relations, neType))
{
}

IndexElement::IndexElement(const IndexElement& ie): m_d(new IndexElementPrivate(*ie.m_d))
{
}

IndexElement& IndexElement::operator=(const IndexElement& ie)
{
  if (this != &ie)
  {
    *m_d = *ie.m_d;
  }
  return *this;
}

IndexElement::~IndexElement()
{
  delete m_d;
}

bool IndexElement::operator==(const IndexElement& other) const
{
  if (isSimpleTerm()) {
    return (other.isSimpleTerm() &&
            m_d->m_type==other.m_d->m_type &&
            m_d->m_word==other.getSimpleTerm() &&
            m_d->m_category==other.getCategory());
  }
  else {
    return (!other.isSimpleTerm() &&
            m_d->m_type==other.m_d->m_type &&
            getSimpleTerm() == other.getSimpleTerm() &&
            m_d->m_structure==other.getStructure() &&
            m_d->m_relations==other.getRelations());
  }
}

bool IndexElement::empty() const { return m_d->m_id==0; }

uint64_t IndexElement::getId() const { return m_d->m_id; }

Lima::Common::BagOfWords::BoWType IndexElement::getType() const { return m_d->m_type; }

bool IndexElement::isSimpleTerm() const { return m_d->m_type == BoWType::BOW_TOKEN || (m_d->m_type == BoWType::BOW_NAMEDENTITY && m_d->m_structure.empty()); }

bool IndexElement::isComposedTerm() const { return m_d->m_type == BoWType::BOW_TERM || (m_d->m_type == BoWType::BOW_NAMEDENTITY && ! m_d->m_structure.empty()); }

bool IndexElement::isPredicate() const { return m_d->m_type == BoWType::BOW_PREDICATE; }

const LimaString& IndexElement::getSimpleTerm() const { return m_d->m_word; }

uint64_t IndexElement::getCategory() const { return m_d->m_category; }

uint64_t IndexElement::getPosition() const { return m_d->m_position; }

uint64_t IndexElement::getLength() const { return m_d->m_length; }

bool IndexElement::isNamedEntity() const { return m_d->m_type == BoWType::BOW_NAMEDENTITY; }

const Common::MediaticData::EntityType& IndexElement::getNamedEntityType() const { return m_d->m_neType; }

Misc::PositionLengthList& IndexElement::getPositionLengthList() { return m_d->m_poslenlist; }

const Misc::PositionLengthList& IndexElement::getPositionLengthList() const { return m_d->m_poslenlist; }

const std::vector<uint64_t>& IndexElement::getStructure() const { return m_d->m_structure; }

std::vector<uint64_t>& IndexElement::getStructure() { return m_d->m_structure; }

const std::vector<uint64_t>& IndexElement::getRelations() const { return m_d->m_relations; }

std::vector<uint64_t>& IndexElement::getRelations() { return m_d->m_relations; }

void IndexElement::setId(const uint64_t id) { m_d->m_id=id; }

void IndexElement::setSimpleTerm(const LimaString& t) { m_d->m_word=t; }

void IndexElement::setCategory(uint64_t category) { m_d->m_category = category; }

void IndexElement::setStructure(const std::vector<uint64_t>& s,
                  const std::vector<uint64_t>& r)
  { m_d->m_structure=s; m_d->m_relations=r; }

void IndexElement::addInStructure(uint64_t id, uint64_t rel)
  { m_d->m_structure.push_back(id); m_d->m_relations.push_back(rel); }

bool IndexElement::hasSamePosition(const IndexElement& other) const
{
  if (isSimpleTerm()) {
    return (other.isSimpleTerm() &&
            m_d->m_position==other.getPosition() &&
            m_d->m_length==other.getLength());
  }
  else {
    return (!other.isSimpleTerm() &&
            m_d->m_poslenlist==other.getPositionLengthList());
  }
}

bool IndexElement::hasNearlySamePosition(const IndexElement& other) const
{
  // if both are simple terms or complex terms, compare them. Otherwise return false
  if (isSimpleTerm() && other.isSimpleTerm()) {
    // both are simple terms

    // terms intersects: the beginning of one of them is inside the other one span
    return (   ( (m_d->m_position<=other.getPosition()) && (other.getPosition() < m_d->m_position+m_d->m_length) )
             ||( (other.getPosition()<=m_d->m_position) && (m_d->m_position < other.getPosition()+other.getLength()) )
            );
  }
  else if (!isSimpleTerm() && !other.isSimpleTerm()) {
    // both are complex terms

    // compute the minimum and maximum positions in this index element as the
    // minimal value of all positions and the maximum value of all positions
    // plus the corresponding length
    PositionLengthList::const_iterator pplIt = getPositionLengthList().begin();
    Position posMin = pplIt->first;
    Position posMax = static_cast<Position>(posMin + pplIt->second - 1);
    for( ; pplIt != getPositionLengthList().end() ; pplIt++ ) {
      if( pplIt->first < posMin ) {
        posMin = pplIt->first;
      }
      if( (pplIt->first+pplIt->second - 1) > posMax ) {
        posMax = pplIt->first+pplIt->second - 1;
      }
    }
    // compute the minimum and maximum positions in the other index element in
    // the same way
    PositionLengthList::const_iterator otherPplIt = other.getPositionLengthList().begin();
    Position otherPosMin = other.getPositionLengthList().back().first;
    Position otherPosMax = static_cast<Position>(otherPosMin + getPositionLengthList().back().second - 1);
    for( ; otherPplIt != other.getPositionLengthList().end() ; otherPplIt++ ) {
      if( otherPplIt->first < otherPosMin ) {
        otherPosMin = otherPplIt->first;
      }
      if( (otherPplIt->first+otherPplIt->second - 1) > otherPosMax ) {
        otherPosMax = otherPplIt->first+otherPplIt->second - 1;
      }
    }

    // The beginning of one of the complex terms is inside the other one span
    return(  ( (posMin <= otherPosMin) && (otherPosMin < posMax) )
          || ( (otherPosMin <= posMin) && (posMin < otherPosMax) ) );
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const IndexElement& elt)
{
  os << "[IndexElement" << elt.m_d->m_id << "," << elt.m_d->m_type ;
  if (elt.isSimpleTerm()) {
  os << ":" << Common::Misc::limastring2utf8stdstring(elt.m_d->m_word);
  os << "/" << elt.m_d->m_category;
    os << "/" << elt.m_d->m_position;
    os << "," << elt.m_d->m_length;
  }
  else {
    if (elt.m_d->m_structure.empty()) {
      return os << ":";
    }
    else {
      uint64_t i=0;
      os << ":" << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
      i++;
      while (i<elt.m_d->m_structure.size()) {
        os << "," << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
        i++;
      }
    }
    os << "/";
    os << elt.m_d->m_poslenlist;
  }
  if (! elt.m_d->m_neType.isNull()) {
    os << "/NE(" << Lima::Common::MediaticData::MediaticData::single().getEntityName(elt.m_d->m_neType).toUtf8().constData() << ")";
  }
  os << "]";
  return os;
}

QDebug& operator<<(QDebug& os, const IndexElement& elt) {
  os << "[IndexElement" << elt.m_d->m_id << "," << elt.m_d->m_type;
  os << ":" << elt.m_d->m_word;
  if (elt.m_d->m_category != 0)
  {
    os << "/" << elt.m_d->m_category;
  }
  os << "/" << elt.m_d->m_position;
  os << "," << elt.m_d->m_length;
  if (!elt.m_d->m_structure.empty())
  {
    uint64_t i=0;
    os << ":" << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
    i++;
    while (i<elt.m_d->m_structure.size()) {
      os << "," << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
      i++;
    }
  }
  os << "/" << elt.m_d->m_poslenlist;
  if (elt.isNamedEntity())
  {
    os << "/NE(" << Lima::Common::MediaticData::MediaticData::single().getEntityName(elt.m_d->m_neType) << ")";
  }
  else if (elt.isPredicate())
  {
    os << "/P(" << Lima::Common::MediaticData::MediaticData::single().getEntityName(elt.m_d->m_neType) << ")";
  }
  os << "]";
  return os;
}

QTextStream& operator<<(QTextStream& os, const IndexElement& elt) {
  os << "[IndexElement"  << elt.m_d->m_id << "," << elt.m_d->m_type;
  if (elt.isSimpleTerm())
  {
  os << ":" << elt.m_d->m_word;
    if (elt.m_d->m_category != 0) {
      os << "/" << elt.m_d->m_category;
    }
    os << "/" << elt.m_d->m_position;
    os << "," << elt.m_d->m_length;
  }
  else
  {
    if (elt.m_d->m_structure.empty()) {
      return os << ":";
    }
    if (!elt.m_d->m_structure.empty()) {
      uint64_t i=0;
      os << ":" << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
      i++;
      while (i<elt.m_d->m_structure.size()) {
        os << "," << elt.m_d->m_structure[i] << "  RE(" << elt.m_d->m_relations[i] << ")";
        i++;
      }
    }
    os << "/";
    os << elt.m_d->m_poslenlist;
  }
  if (! elt.m_d->m_neType.isNull()) {
    os << "/NE(" << Lima::Common::MediaticData::MediaticData::single().getEntityName(elt.m_d->m_neType) << ")";
  }
  os << "]";
  return os;
}


} // end namespace
} // end namespace
} // end namespace
