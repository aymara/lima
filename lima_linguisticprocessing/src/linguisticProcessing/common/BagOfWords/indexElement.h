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
 * @file       indexElement.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
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
#include "common/Data/LimaString.h"
#include "common/MediaticData/EntityType.h"
#include <vector>
#include <iostream>

namespace Lima {
namespace Common {
namespace BagOfWords {

class IndexElementPrivate;
class LIMA_BOW_EXPORT IndexElement
{
 public:
  IndexElement(); 
  IndexElement(const uint64_t id,
               const LimaString& word,
               const uint64_t cat=0,
               const uint64_t position=0,
               const uint64_t length=0,
               const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType(),
               const uint64_t reType=0); 
  IndexElement(const uint64_t id,
               const std::vector<uint64_t>& structure,
               const std::vector<uint64_t>& relations,
               const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType(),
               const uint64_t reType=0);
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
  bool isSimpleTerm() const;
  bool isComposedTerm() const;

  const LimaString& getSimpleTerm() const;
  uint64_t getCategory() const;
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
  void setCategory(uint64_t category);
  void setStructure(const std::vector<uint64_t>& s,
                    const std::vector<uint64_t>& r);
  void addInStructure(uint64_t id, uint64_t rel);
  
  friend LIMA_BOW_EXPORT std::ostream& operator<<(std::ostream& os, const IndexElement& elt);
  friend LIMA_BOW_EXPORT QDebug& operator<<(QDebug& os, const IndexElement& elt);

private:
  IndexElementPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
