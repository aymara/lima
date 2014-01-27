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
 * @file       itemWithAttributes.cpp
 * @author     besancon (romaric.besancon@cea.fr)
 * @date       Thu Dec  8 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "itemWithAttributes.h"
#include "xmlConfigurationFileExceptions.h"

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class ItemWithAttributesPrivate
{
  friend class ItemWithAttributes;
  
  ItemWithAttributesPrivate();
  ItemWithAttributesPrivate(const std::string& name);
  ItemWithAttributesPrivate(const ItemWithAttributesPrivate& item);
  ItemWithAttributesPrivate& operator=(const ItemWithAttributesPrivate& item);
  ~ItemWithAttributesPrivate();

  std::string m_name;
  std::map<std::string,std::string> m_attributes;
};

ItemWithAttributesPrivate::ItemWithAttributesPrivate():
m_name(""),
m_attributes()
{
}

ItemWithAttributesPrivate::ItemWithAttributesPrivate(const std::string& name):
m_name(name),
m_attributes()
{
}

ItemWithAttributesPrivate::ItemWithAttributesPrivate(const ItemWithAttributesPrivate& item):
m_name(item.m_name),
m_attributes(item.m_attributes)
{
}

ItemWithAttributesPrivate& ItemWithAttributesPrivate::operator=(const ItemWithAttributesPrivate& item)
{
  m_name = item.m_name;
  m_attributes = item.m_attributes;
  return *this;
}

ItemWithAttributesPrivate::~ItemWithAttributesPrivate() {
}

//***********************************************************************
// constructors and destructors
ItemWithAttributes::ItemWithAttributes():
    m_d(new ItemWithAttributesPrivate())
{
}

ItemWithAttributes::ItemWithAttributes(const std::string& name):
    m_d(new ItemWithAttributesPrivate(name))
{
}

ItemWithAttributes::ItemWithAttributes(const ItemWithAttributes& item):
    m_d(new ItemWithAttributesPrivate(*item.m_d))
{
}

ItemWithAttributes& ItemWithAttributes::operator=(const ItemWithAttributes& item)
{
  *m_d = *item.m_d;
  return *this;
}

ItemWithAttributes::~ItemWithAttributes()
{
  delete m_d;
}

//***********************************************************************
const std::string& ItemWithAttributes::getName() const { return m_d->m_name; }

uint64_t ItemWithAttributes::nbAttributes() const { return m_d->m_attributes.size(); }

const std::map<std::string,std::string>& ItemWithAttributes::getAttributes() const
  { return m_d->m_attributes; }

const std::string& ItemWithAttributes::
getAttribute(const std::string& attributeName) const
{
  std::map<std::string,std::string>::const_iterator 
    it=m_d->m_attributes.find(attributeName);
  if (it == m_d->m_attributes.end()) {
    throw NoSuchAttribute(attributeName);
  }
  return (*it).second;
}

bool ItemWithAttributes::
hasAttribute(const std::string& attributeName) const
{
  std::map<std::string,std::string>::const_iterator 
    it=m_d->m_attributes.find(attributeName);
  return !(it == m_d->m_attributes.end());
}

void ItemWithAttributes::
addAttribute(const std::string& attributeName,
             const std::string& value) 
{
  m_d->m_attributes.insert(std::make_pair(attributeName,value));
}


} // end namespace
} // end namespace
} // end namespace
