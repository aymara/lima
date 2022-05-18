// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                                        *
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
  return code & m_d->m_mask;
}

void PropertyAccessor::writeValue(
  const LinguisticCode& value,
  LinguisticCode& code) const
{
  code = ( code & ( ~ m_d->m_mask ) ) | ( value & m_d->m_mask );
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
