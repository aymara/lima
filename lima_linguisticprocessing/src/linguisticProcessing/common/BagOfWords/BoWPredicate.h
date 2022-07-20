// Copyright 2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

#include <boost/shared_ptr.hpp>

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
  BoWPredicate(const Common::MediaticData::EntityType theType, 
               QMultiMap<Common::MediaticData::EntityType, 
               boost::shared_ptr< AbstractBoWElement > > pRoles);

  virtual ~BoWPredicate();

  /** size of the AbstractBoWElement is the number of parts in the token, 0 for 
   *a predicate */
  virtual uint64_t size(void) const override {return 0;};

  virtual BoWPredicate* clone() const override;

  BoWPredicate& operator=(const BoWPredicate&);
  bool operator==(const BoWPredicate&);

  MediaticData::EntityType getPredicateType(void) const;
  void setPredicateType(const MediaticData::EntityType&);

  virtual BoWType getType() const override { return BoWType::BOW_PREDICATE; }

  virtual Lima::LimaString getString(void) const override;

  virtual uint64_t getPosition(void) const override;
  virtual uint64_t getLength(void) const override;
  virtual void setPosition(const uint64_t pos) override;
  virtual void setLength(const uint64_t len) override;

  /** get a string of the predicate for output function */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const override;
  virtual std::string getIdUTF8String(void) const override;

  const QMultiMap<Common::MediaticData::EntityType, boost::shared_ptr< AbstractBoWElement > >& roles() const;
  QMultiMap<Common::MediaticData::EntityType, boost::shared_ptr< AbstractBoWElement > >& roles();
  
  void setRoles(QMultiMap<Common::MediaticData::EntityType, boost::shared_ptr< Common::BagOfWords::AbstractBoWElement > >& pRoles);

  /**
   * returns the vertices of the predicate and the roles
   *
   * @return the vertices of the predicate and the roles
   */
  virtual std::set< uint64_t > getVertices() const override;


protected:
    BoWPredicate(BoWPredicatePrivate&);
    
private:
  BoWPredicatePrivate* m_d;
};


} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
