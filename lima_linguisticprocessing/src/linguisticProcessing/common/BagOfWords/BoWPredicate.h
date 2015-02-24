/*
    Copyright 2014 CEA LIST

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
 * @file     BoWPredicate.h
 * @author   Gael de Chalendar
 * @date     Mon Jul  7 2014
 * copyright Copyright (C) 2014 by CEA LIST
 ***********************************************************************/

#ifndef BOWPREDICATE_H
#define BOWPREDICATE_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/BagOfWords/AbstractBoWElement.h"
#include "common/MediaticData/EntityType.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWPredicatePrivate;
/**
  * This is a BoW element used to represent a predicate (n-ary relation, 
  * template or semantic frame). It has a type which is an entity type. It gives access 
  * to a collection of typed arguments, its roles.
  */
class LIMA_BOW_EXPORT BoWPredicate : public AbstractBoWElement
{
  friend class BoWPredicatePrivate;
public:
  BoWPredicate();
  BoWPredicate(const BoWPredicate&);
  BoWPredicate(const Common::MediaticData::EntityType theType);
  BoWPredicate(const Common::MediaticData::EntityType theType, QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*> pRoles);

  virtual ~BoWPredicate();

  /** size of the AbstractBoWElement is the number of parts in the token, 0 for 
   *a predicate */
  virtual uint64_t size(void) const {return 0;};

  virtual BoWPredicate* clone() const;

  BoWPredicate& operator=(const BoWPredicate&);
  bool operator==(const BoWPredicate&);

  MediaticData::EntityType getPredicateType(void) const;
  void setPredicateType(const MediaticData::EntityType&);

  virtual BoWType getType() const { return BOW_PREDICATE; }

  virtual Lima::LimaString getString(void) const;

  virtual uint64_t getPosition(void) const;
  virtual uint64_t getLength(void) const;
  virtual void setPosition(const uint64_t pos);
  virtual void setLength(const uint64_t len);

  /** get a string of the predicate for output function */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const;
  virtual std::string getIdUTF8String(void) const;

  const QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>& roles() const;
  QMultiMap<Common::MediaticData::EntityType, AbstractBoWElement*>& roles();
  
  void setRoles(QMultiMap<Common::MediaticData::EntityType, Common::BagOfWords::AbstractBoWElement*>& pRoles);

  /**
   * returns the vertices of the predicate and the roles
   *
   * @return the vertices of the predicate and the roles
   */
  virtual std::set< uint64_t > getVertices() const;


protected:
    BoWPredicate(BoWPredicatePrivate&);
    
private:
  BoWPredicatePrivate* m_d;
};


} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
