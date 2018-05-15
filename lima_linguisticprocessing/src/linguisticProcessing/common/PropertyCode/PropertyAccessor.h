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
/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROPERTYCODE_PROPERTYACCESSOR_H
#define LIMA_COMMON_PROPERTYCODE_PROPERTYACCESSOR_H

#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

/**
 * Provide function to read write and check a property.
 * @brief Class that provide read/write methods for a property
 * @author Benoit Mathieu
 */
class LIMA_PROPERTYCODE_EXPORT PropertyAccessor
{
public:

  /**
   * @brief Constructor
   * @param name property name
   * @param mask property mask
   * @param mask mask use to test emptyNess of property. Different from mask \
   * only for subproperties.
   */
  PropertyAccessor(const std::string& name,
                   const LinguisticCode& mask,
                   const LinguisticCode& emptyNessMask);

  /**
   * @brief Get the property name
   * @return property name
   */
  const std::string& getPropertyName() const;

  /**
   * @brief read a property in a coded int.
   * This function extract the coded value of a property, which allows
   * to perform comparison.
   * @param code coded int from which to read the property
   * @return coded value of the property
   */
  LinguisticCode readValue(const LinguisticCode& code) const;
  
  /**
   * @brief write a property value in a coded int.
   * This function writes the given coded property value in the given coded int,
   * <b>overriding previous value of the property</b>.
   * @param value coded property value to write. This value can hold several property data.
   * @param code coded int to write to.
   */
  void writeValue(const LinguisticCode& value, LinguisticCode& code) const;
  
  /**
   * @brief check property equality for two linguisticCode
   * @param l1 
   * @param l2 
   * @return true if l1 and l2 have same property value
   */
  bool equal(const LinguisticCode& l1, const LinguisticCode& l2) const;

  /**
   * Test if the given code has property data. In the case of a simple property,
   * property is empty if equal to 0. In the case of a subproperty, the subproperty
   * can be empty and have a non null value, if the parent property is non null.
   * @brief return true if the given code has an empty property
   * @param l 
   * @return true if property is empty for l.
   */
  bool empty(const LinguisticCode& l) const;

private:

  LinguisticCode m_mask;
  LinguisticCode m_emptyNessMask;
  std::string m_name;

};

inline LinguisticCode PropertyAccessor::readValue(const LinguisticCode& code) const
{
  return static_cast<LinguisticCode>(code & m_mask);
}

inline void PropertyAccessor::writeValue(
  const LinguisticCode& value,
  LinguisticCode& code) const
{
  code = static_cast<LinguisticCode>(( code & ( ~ m_mask )) | (value & m_mask));
}

inline bool PropertyAccessor::equal(const LinguisticCode& l1,
                                    const LinguisticCode& l2) const {
  return !(m_mask & (l1 ^ l2));
}

inline bool PropertyAccessor::empty(const LinguisticCode& l) const {
  return !(l & m_emptyNessMask);
}

} // PropertyCode
} // Common
} // Lima


#endif
