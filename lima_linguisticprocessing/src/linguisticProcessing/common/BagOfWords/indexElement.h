// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       indexElement.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2020 by CEA LIST
 * Project     BagOfWords
 *
 * @brief a class to represent the element to index in the bag of
 * words structure
 *
 *
 ***********************************************************************/

#ifndef INDEXELEMENT_H
#define INDEXELEMENT_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/misc/positionLengthList.h"
#include "linguisticProcessing/common/BagOfWords/AbstractBoWElement.h"
#include "common/Data/LimaString.h"
#include "common/MediaticData/EntityType.h"
#include <vector>
#include <iostream>

namespace Lima {
namespace Common {
namespace BagOfWords {

class IndexElementPrivate;

/**
 * Represent an element of an index
 * If it is a predicate, its simple term is "PredicateElement" and its neType is the
 * type of the predicate. Its structure and relations allow to find its roles
 * If it is a role, its simple term is "PredicateRole" and its neType is the
 * type of the type of the. Its structure and relations are empty except if it is
 * itself a predicate (in case of reified predicates).
 */
class LIMA_BOW_EXPORT IndexElement
{
public:
  IndexElement();
  IndexElement(const uint64_t id,
               const BagOfWords::BoWType type,
               const LimaString& word,
               const LinguisticCode cat = LinguisticCode::ZERO,
               const uint64_t position=0,
               const uint64_t length=0,
               const MediaticData::EntityType neType=MediaticData::EntityType());
  IndexElement(const uint64_t id,
               const Lima::Common::BagOfWords::BoWType type,
               const std::vector<uint64_t>& structure,
               const std::vector<uint64_t>& relations,
               const MediaticData::EntityType neType=MediaticData::EntityType());
  IndexElement(const IndexElement& ie);
  IndexElement& operator=(const IndexElement&);
  ~IndexElement();

  /** equality operator just compare content, not positions */
  bool operator==(const IndexElement& other) const;
  /** compare position and length */
  bool hasSamePosition(const IndexElement& other) const;
  /** compare position and length: thereis some intersaction in position */
  bool hasNearlySamePosition(const IndexElement& other) const;

  bool empty() const;
  uint64_t getId() const;
  Lima::Common::BagOfWords::BoWType getType() const;
  bool isSimpleTerm() const;
  bool isComposedTerm() const;
  bool isPredicate() const;

  const LimaString& getSimpleTerm() const;
  LinguisticCode getCategory() const;
  uint64_t getPosition() const;
  uint64_t getLength() const;
  bool isNamedEntity() const;
  const Common::MediaticData::EntityType& getNamedEntityType() const;
  Lima::Common::Misc::PositionLengthList& getPositionLengthList();
  const Misc::PositionLengthList& getPositionLengthList() const;
  const std::vector<uint64_t>& getStructure() const;
  std::vector<uint64_t>& getStructure();

  const std::vector<uint64_t>& getRelations() const;
  std::vector<uint64_t>& getRelations();

  void setId(const uint64_t id);
  void setSimpleTerm(const LimaString& t);
  void setCategory(LinguisticCode category);
  void setStructure(const std::vector<uint64_t>& s,
                    const std::vector<uint64_t>& r);
  void addInStructure(uint64_t id, uint64_t rel);

  friend LIMA_BOW_EXPORT std::ostream& operator<<(std::ostream& os, const IndexElement& elt);
  friend LIMA_BOW_EXPORT QDebug& operator<<(QDebug& os, const IndexElement& elt);
  friend LIMA_BOW_EXPORT QTextStream& operator<<(QTextStream& os, const IndexElement& elt);

private:
  IndexElementPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
