/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004 by CEA LIST                                        *
 *                                                                         *
 ***************************************************************************/

#include "PropertyManager.h"


using namespace std;

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class PropertyManagerPrivate
{
friend class PropertyManager;

  PropertyManagerPrivate(const std::string& name,
                  const LinguisticCode& mask,
                  const LinguisticCode& emptyNessMask,
                  const std::map<std::string,LinguisticCode> symbol2code);

  ~PropertyManagerPrivate();
  PropertyManagerPrivate(const PropertyManagerPrivate& pmp);
  PropertyManagerPrivate& operator=(const PropertyManagerPrivate& pmp);

  std::string m_name;
  LinguisticCode m_mask;
  LinguisticCode m_emptyNessMask;
  std::map<LinguisticCode,std::string> m_code2symbol;
  std::map<std::string,LinguisticCode> m_symbol2code;

  PropertyAccessor m_accessor;

  static std::string s_none;

};

std::string PropertyManagerPrivate::s_none("NONE");

PropertyManagerPrivate::PropertyManagerPrivate(
  const std::string& name,
  const LinguisticCode& mask,
  const LinguisticCode& emptyNessMask,
  const std::map<std::string,LinguisticCode> symbol2code) :
    m_name(name),
    m_mask(mask),
    m_emptyNessMask(emptyNessMask),
    m_code2symbol(),
    m_symbol2code(symbol2code),
    m_accessor(name,mask,emptyNessMask)
{
  // fill code2symbol table from symbol2code table
  for (map<string,LinguisticCode>::const_iterator it=symbol2code.begin();
       it!=symbol2code.end();
       it++)
  {
    m_code2symbol.insert(make_pair(it->second,it->first));
  }
}

PropertyManagerPrivate::~PropertyManagerPrivate()
{
}

PropertyManagerPrivate::PropertyManagerPrivate(const PropertyManagerPrivate& pmp) :
    m_name(pmp.m_name),
    m_mask(pmp.m_mask),
    m_emptyNessMask(pmp.m_emptyNessMask),
    m_code2symbol(pmp.m_code2symbol),
    m_symbol2code(pmp.m_symbol2code),
    m_accessor(pmp.m_accessor)
{
}

PropertyManagerPrivate& PropertyManagerPrivate::operator=(const PropertyManagerPrivate& pmp)
{
  m_name = pmp.m_name;
  m_mask = pmp.m_mask;
  m_emptyNessMask = pmp.m_emptyNessMask;
  m_code2symbol = pmp.m_code2symbol;
  m_symbol2code = pmp.m_symbol2code;
  m_accessor = pmp.m_accessor;
  return *this;
}

PropertyManager::PropertyManager(const std::string& name,
                                 const LinguisticCode& mask,
                                 const LinguisticCode& emptyNessMask,
                                 const std::map<std::string,LinguisticCode> symbol2code) :
    m_d(new PropertyManagerPrivate(name, mask, emptyNessMask, symbol2code))
{
}

PropertyManager::~PropertyManager()
{
  delete m_d;
}

PropertyManager::PropertyManager(const PropertyManager& pm)
{
  m_d = new PropertyManagerPrivate(*pm.m_d);
}

PropertyManager& PropertyManager::operator=(const PropertyManager& pm)
{
  *m_d = *pm.m_d;
  return *this;
}


const PropertyAccessor& PropertyManager::getPropertyAccessor() const
{
  return m_d->m_accessor;
}

LinguisticCode PropertyManager::getPropertyValue(
  const std::string& symbolicValue) const
{
  auto it = m_d->m_symbol2code.find(symbolicValue);
  if (it == m_d->m_symbol2code.end())
  {
    PROPERTYCODELOGINIT;
    LWARN << "Ask for unknown value " << symbolicValue
          << " for property " << m_d->m_name;
    return static_cast<LinguisticCode>(0);
  }
  return it->second;
}

const std::string& PropertyManager::getPropertySymbolicValue(
  const LinguisticCode& value) const
{
  LinguisticCode val = m_d->m_accessor.readValue(value);
  auto it = m_d->m_code2symbol.find(val);
  if (it == m_d->m_code2symbol.cend())
  {
    PROPERTYCODELOGINIT;
    LWARN << "Ask for unknown value " << val << " (extracted from " << value
          << ") for property " << m_d->m_name;
    return PropertyManagerPrivate::s_none;
  }
  return it->second;
}

LinguisticCode PropertyManager::getMask() const
{
  return m_d->m_mask;
}

LinguisticCode PropertyManager::getEmptyNessMask() const
{
  return m_d->m_emptyNessMask;
}

const std::map<LinguisticCode,std::string>& PropertyManager::getCode2Symbol() const
{
  return m_d->m_code2symbol;
}

const std::map<std::string,LinguisticCode>& PropertyManager::getSymbol2Code() const
{
  return m_d->m_symbol2code;
}

size_t PropertyManager::getNbValues() const
{
  return m_d->m_symbol2code.size();
}

} // PropertyCode
} // Common
} // Lima
