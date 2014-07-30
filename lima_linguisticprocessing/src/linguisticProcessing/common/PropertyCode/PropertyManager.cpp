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
 *   Copyright (C) 2004 by CEA LIST                       *
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

std::string PropertyManager::s_none("NONE");

PropertyManager::PropertyManager(const std::string& name,
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

PropertyManager::~PropertyManager() {}

const PropertyAccessor& PropertyManager::getPropertyAccessor() const
{
  return m_accessor;
}

LinguisticCode PropertyManager::getPropertyValue(const std::string& symbolicValue) const
{
  map<string,LinguisticCode>::const_iterator it=m_symbol2code.find(symbolicValue);
  if (it == m_symbol2code.end())
  {
    PROPERTYCODELOGINIT;
    LWARN << "Ask for unknown value " << symbolicValue << " for property " << m_name;
    return static_cast<LinguisticCode>(0);
  }
  return it->second;
}

const std::string& PropertyManager::getPropertySymbolicValue(const LinguisticCode& value) const
{
  LinguisticCode val=m_accessor.readValue(value);
  map<LinguisticCode,string>::const_iterator it=m_code2symbol.find(val);
  if (it == m_code2symbol.end())
  {
    PROPERTYCODELOGINIT;
    LWARN << "Ask for unknown value " << val << " (extracted from " << value << ") for property " << m_name;
    return s_none;
  }
  return it->second;
}

} // PropertyCode
} // Common
} // Lima
