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
 * @file     AbstractBoWElement.h
 * @author   Gael de Chalendar <gael.de-chalendar@cea.fr>
 * @date     Thu Jul 24 2003
 * Copyright (C) 2014 by CEA LIST
 ***********************************************************************/

#ifndef ABSTRACTBOWELEMENT_H
#define ABSTRACTBOWELEMENT_H

#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/common/misc/positionLengthList.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <set>
#include <memory>

namespace Lima {
namespace Common {
namespace PropertyCode {
  class PropertyManager;
}
namespace BagOfWords {

/**
 * enum to characterize the type of the AbstractBoWElement
 */
typedef enum {
  BOW_NOTYPE,                   /**< the AbstractBoWElement is an abstract one that 
                                      should not be instanciated */
  BOW_TOKEN,                    /**< the AbstractBoWElement is a simple token */
  BOW_TERM,                     /**< the AbstractBoWElement is a multi-term */
  BOW_NAMEDENTITY,              /**< the AbstractBoWElement is a named entity */
  BOW_PREDICATE,                /**< the AbstractBoWElement is a predicate (n-ary 
                                      relation, template or semantic frame */
} BoWType;

/**
 * This class is the abstract base class of all elements that can be stored in 
 * a BoWText. This includes tokens like simple ones and complex ones as 
 * specific entities but also non-token elements like predicates and their 
 * arguments
 **/
class LIMA_BOW_EXPORT AbstractBoWElement
{
public:

  AbstractBoWElement() {}

  virtual ~AbstractBoWElement() {}

  virtual void clear() {}

  virtual AbstractBoWElement* clone() const = 0;

  /** size of the AbstractBoWElement is the number of parts in the token: 0 for 
   *  a predicate, 1 for a simple token, n for complex tokens */
  virtual uint64_t size(void) const = 0;

  virtual BoWType getType() const {return BOW_NOTYPE;}

  virtual Lima::LimaString getString(void) const = 0;

  /**
   * returns the set of vertices covered by this element
   *
   * @return the set of vertices covered by this element
   */
  virtual std::set< uint64_t > getVertices() const = 0;

  /** get a string of the AbstractBoWElement for output function
    * @param macroAccessor if not null, will output the human readable form of the macrocategory.
    * otherwise output the integer code
    */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const = 0;

  /** function used to identify a bowtoken in a text (used in BowDumper) */
  virtual std::string getIdUTF8String() const = 0;

};

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
