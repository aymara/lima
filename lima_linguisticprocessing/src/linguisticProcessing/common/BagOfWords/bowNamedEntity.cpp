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
 * @file     bowNamedEntity.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "bowNamedEntity.h"
#include "bowComplexTokenPrivate.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWNamedEntityPrivate : public BoWComplexTokenPrivate
{
public:
  friend class BoWNamedEntity;

  BoWNamedEntityPrivate();
// //   BoWNamedEntityPrivate(const BoWNamedEntity&);
// //   BoWNamedEntityPrivate(const BoWNamedEntity&,
// //                  const std::map<BoWToken*,BoWToken*>&);
// 
//   BoWNamedEntityPrivate(const Lima::LimaString& lemma,
//                  const Common::MediaticData::EntityType theType,
//                  const uint64_t position,
//                  const uint64_t length);
// 
//   BoWNamedEntityPrivate(const Lima::LimaString& lemma,
//                  const LinguisticCode theCategory,
//                  const Common::MediaticData::EntityType theType,
//                  const uint64_t position,
//                  const uint64_t length);
// 
//   virtual ~BoWNamedEntityPrivate();

  BoWNamedEntityPrivate(const BoWNamedEntityPrivate&);
//   BoWNamedEntityPrivate& operator=(const BoWNamedEntityPrivate&);
  
  Common::MediaticData::EntityType m_type;
  std::map<std::string, LimaString> m_features;

  // private functions
  void readNEProperties(std::istream& file);
  void writeNEProperties(std::ostream& file) const;

};

BoWNamedEntityPrivate::BoWNamedEntityPrivate():
BoWComplexTokenPrivate(),
m_type(),
m_features()
{
}

BoWNamedEntityPrivate::BoWNamedEntityPrivate(const BoWNamedEntityPrivate& bnep):
BoWComplexTokenPrivate(bnep),
m_type(bnep.m_type),
m_features(bnep.m_features)
{
}

// BoWNamedEntityPrivate::BoWNamedEntityPrivate(const Lima::LimaString& lemma,
//                                const Common::MediaticData::EntityType theType,
//                                const uint64_t position,
//                                const uint64_t length) :
// BoWComplexTokenPrivate(lemma, 0, position, length),
// m_type(theType),
// m_features()
// {
// }
// 
// BoWNamedEntityPrivate::BoWNamedEntityPrivate(const Lima::LimaString& lemma,
//                                const LinguisticCode theCategory,
//                                const Common::MediaticData::EntityType theType,
//                                const uint64_t position,
//                                const uint64_t length):
// BoWComplexTokenPrivate(lemma, theCategory, position, length),
// m_type(theType),
// m_features()
// {
// }
// 
// 
// // BoWNamedEntityPrivate::BoWNamedEntityPrivate(const BoWNamedEntity& ne):
// // BoWComplexTokenPrivate(ne),
// // m_type(ne.m_d->m_type),
// // m_features(ne.m_d->m_features)
// // {
// // }
// // BoWNamedEntityPrivate::BoWNamedEntityPrivate(const BoWNamedEntity& ne,
// //                                const std::map<BoWToken*,BoWToken*>& refmap):
// // BoWComplexTokenPrivate(ne,refmap),
// // m_type(ne.m_d->m_type),
// // m_features(ne.m_d->m_features)
// // {
// // }
// 
// BoWNamedEntityPrivate::~BoWNamedEntityPrivate()
// {
//   m_features.clear();
// }


//**********************************************************************
// constructors

BoWNamedEntity::BoWNamedEntity():
BoWComplexToken(*new BoWNamedEntityPrivate())
{
}

BoWNamedEntity::BoWNamedEntity(const Lima::LimaString& lemma,
                               const Common::MediaticData::EntityType theType,
                               const uint64_t position,
                               const uint64_t length) :
    BoWComplexToken(*new BoWNamedEntityPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = static_cast<Lima::LinguisticCode>(0);
  m_d->m_position = position;
  m_d->m_length = length;
  static_cast<BoWNamedEntityPrivate*>(m_d)->m_type = theType;
}

BoWNamedEntity::BoWNamedEntity(const Lima::LimaString& lemma,
                               const LinguisticCode theCategory,
                               const Common::MediaticData::EntityType theType,
                               const uint64_t position,
                               const uint64_t length):
    BoWComplexToken(*new BoWNamedEntityPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = theCategory;
  m_d->m_position = position;
  m_d->m_length = length;
  static_cast<BoWNamedEntityPrivate*>(m_d)->m_type = theType;
}


BoWNamedEntity::BoWNamedEntity(const BoWNamedEntity& ne):
BoWComplexToken(*new BoWNamedEntityPrivate(static_cast<BoWNamedEntityPrivate&>(*ne.m_d)))
{
  static_cast<BoWNamedEntityPrivate&>(*m_d).copy(ne);
}

BoWNamedEntity::BoWNamedEntity(const BoWNamedEntity& ne,
                               const std::map<QSharedPointer< BoWToken >, QSharedPointer< BoWToken > >& refmap):
    BoWComplexToken(*new BoWNamedEntityPrivate(static_cast<BoWNamedEntityPrivate&>(*ne.m_d)))
{
  static_cast<BoWNamedEntityPrivate&>(*m_d).copy(ne,refmap);
}

BoWNamedEntity::BoWNamedEntity(BoWNamedEntityPrivate& d) :
BoWComplexToken(d)
{
}


BoWNamedEntity::~BoWNamedEntity()
{
  BoWComplexToken::clear();
}

BoWNamedEntity& BoWNamedEntity::operator=(const BoWNamedEntity& t)
{
  if (&t != this) {
    BoWComplexToken::operator=(t);
    static_cast<BoWNamedEntityPrivate*>(m_d)->m_type=static_cast<BoWNamedEntityPrivate*>(t.m_d)->m_type;
    static_cast<BoWNamedEntityPrivate*>(m_d)->m_features=static_cast<BoWNamedEntityPrivate*>(t.m_d)->m_features;
  }
  return *this;
}

bool BoWNamedEntity::operator==(const BoWNamedEntity& t)
{
  return( BoWComplexToken::operator==(t) && static_cast<BoWNamedEntityPrivate*>(m_d)->m_type==static_cast<BoWNamedEntityPrivate*>(t.m_d)->m_type && static_cast<BoWNamedEntityPrivate*>(m_d)->m_features==static_cast<BoWNamedEntityPrivate*>(t.m_d)->m_features);
}
//**************************************************************
// functions
//**************************************************************
Common::MediaticData::EntityType
BoWNamedEntity::getNamedEntityType(void) const
{
  return static_cast<BoWNamedEntityPrivate*>(m_d)->m_type;
}

void BoWNamedEntity::setNamedEntityType(const Common::MediaticData::EntityType& t)
{
  static_cast<BoWNamedEntityPrivate*>(m_d)->m_type=t;
}

const
std::map<std::string, LimaString>& BoWNamedEntity::getFeatures() const {
  return static_cast<BoWNamedEntityPrivate*>(m_d)->m_features;
}

void BoWNamedEntity::addFeature(const std::string& attribute,
                                       const LimaString& value) {
  static_cast<BoWNamedEntityPrivate*>(m_d)->m_features[attribute]=value;
}

BoWNamedEntity* BoWNamedEntity::clone() const
{
  return new BoWNamedEntity(*(new BoWNamedEntityPrivate(static_cast<BoWNamedEntityPrivate&>(*(this->m_d)))));
}
// BoWNamedEntity*
// BoWNamedEntity::clone(const std::map<BoWToken*,BoWToken*>& map) const
// {
//   return new BoWNamedEntity(*this,map);
// }


//**********************************************************************
// input/output functions
//**********************************************************************
std::string BoWNamedEntity::getFeaturesUTF8String() const {
  std::ostringstream oss;
  if (! static_cast<BoWNamedEntityPrivate*>(m_d)->m_features.empty()) { 
    std::map<std::string, LimaString>::const_iterator i=static_cast<BoWNamedEntityPrivate*>(m_d)->m_features.begin();   
    oss << (*i).first << "=" 
      << Misc::limastring2utf8stdstring((*i).second);
    i++;
    while (i != static_cast<BoWNamedEntityPrivate*>(m_d)->m_features.end()) {
      oss << ";" 
          << (*i).first << "=" 
        << Misc::limastring2utf8stdstring((*i).second);
      i++;
    }
  }
  return oss.str();
}

std::string BoWNamedEntity::getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager) const {
  std::ostringstream oss;
  oss << BoWToken::getOutputUTF8String(macroManager) << "->" << getUTF8StringParts(macroManager)
  << ":" << Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(static_cast<BoWNamedEntityPrivate*>(m_d)->m_type)) << ":" << getFeaturesUTF8String();
  return oss.str();
}

std::string BoWNamedEntity::getIdUTF8String() const {
  std::ostringstream oss;
  oss << BoWToken::getOutputUTF8String() << "->" << getUTF8StringParts()
  << ":" << static_cast<BoWNamedEntityPrivate*>(m_d)->m_type << ":" << getFeaturesUTF8String();
  return oss.str();
}

} // namespace BagOfWords
} // namespace Common
} // namespace Lima
