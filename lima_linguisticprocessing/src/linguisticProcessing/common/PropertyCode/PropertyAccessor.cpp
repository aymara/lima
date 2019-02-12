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

#include "PropertyAccessor.h"
#include <iostream>

using namespace std;

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class PropertyAccessorPrivate
{
  friend class PropertyAccessor;

  /**
   * @brief Constructor
   * @param name property name
   * @param mask property mask
   * @param mask mask use to test emptyNess of property. Different from mask \
   * only for subproperties.
   */
  PropertyAccessorPrivate(const std::string& name,
                   const LinguisticCode& mask,
                   const LinguisticCode& emptyNessMask);

  LinguisticCode m_mask;
  LinguisticCode m_emptyNessMask;
  std::string m_name;

};


PropertyAccessorPrivate::PropertyAccessorPrivate(
  const std::string& name,
  const LinguisticCode& mask,
  const LinguisticCode& emptyNessMask) :
    m_mask(mask),
    m_emptyNessMask(emptyNessMask),
    m_name(name)
{}

PropertyAccessor::PropertyAccessor(
  const std::string& name,
  const LinguisticCode& mask,
  const LinguisticCode& emptyNessMask) :
    m_d(new PropertyAccessorPrivate(name, mask, emptyNessMask))
{}

PropertyAccessor::~PropertyAccessor()
{
  delete m_d;
}

PropertyAccessor::PropertyAccessor(const PropertyAccessor& pa)
{
  m_d = new PropertyAccessorPrivate(*pa.m_d);
}

PropertyAccessor& PropertyAccessor::operator=(const PropertyAccessor& pa)
{
  *m_d = *pa.m_d;
  return *this;
}

const std::string& PropertyAccessor::getPropertyName() const
{
  return m_d->m_name;
}

LinguisticCode PropertyAccessor::readValue(const LinguisticCode& code) const
{
  return static_cast<LinguisticCode>(code & m_d->m_mask);
}

void PropertyAccessor::writeValue(
  const LinguisticCode& value,
  LinguisticCode& code) const
{
  code = static_cast<LinguisticCode>(
    ( code & ( ~ m_d->m_mask )) | (value & m_d->m_mask));
}

bool PropertyAccessor::equal(const LinguisticCode& l1,
                             const LinguisticCode& l2) const
{
  return !(m_d->m_mask & (l1 ^ l2));
}

bool PropertyAccessor::empty(const LinguisticCode& l) const
{
  return !(l & m_d->m_emptyNessMask);
}


} // PropertyCode
} // Common
} // Lima
